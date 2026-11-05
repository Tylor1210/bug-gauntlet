#include "simulator.h"

#include <chrono>
#include <random>
#include <thread>

#include "log.h"

namespace {

constexpr std::array<int, static_cast<size_t>(Op::kCount)> kWeights = {60, 25, 8, 4, 2, 1};

const std::array<const char*, 8> kTitles = {"Portfolio", "Newsletter", "Shop", "Podcast",
                                            "YouTube", "Book a call", "Blog", "Tip jar"};

Op pick_op(std::mt19937_64& rng) {
    static const std::discrete_distribution<int> dist(kWeights.begin(), kWeights.end());
    auto d = dist;  // distributions carry state; copy so the static one stays const
    return static_cast<Op>(d(rng));
}

std::string handle_for(int n) { return "user" + std::to_string(n); }

}  // namespace

const char* op_name(Op op) {
    switch (op) {
        case Op::ViewPage: return "view_page";
        case Op::Click: return "click";
        case Op::AddLink: return "add_link";
        case Op::RemoveLink: return "remove_link";
        case Op::CreateProfile: return "create_profile";
        case Op::UpdateBio: return "update_bio";
        case Op::kCount: break;
    }
    return "?";
}

Simulator::Simulator(LinkStore& store, Analytics& analytics, SimConfig config)
    : store_(store), analytics_(analytics), config_(config) {}

void Simulator::seed_data() {
    std::mt19937_64 rng(config_.seed);
    for (int i = 0; i < config_.initial_profiles; ++i) {
        std::string handle = handle_for(i);
        store_.create_profile(handle, "User " + std::to_string(i));
        int links = 1 + static_cast<int>(rng() % 6);
        for (int j = 0; j < links; ++j) {
            store_.add_link(handle, kTitles[rng() % kTitles.size()],
                            "https://example.com/" + handle + "/" + std::to_string(j));
        }
    }
    logging::info("sim", "seeded " + std::to_string(config_.initial_profiles) + " profiles");
}

SimReport Simulator::run() {
    std::vector<SimReport> partials(config_.threads);
    std::vector<std::thread> threads;
    int64_t per_thread = config_.total_ops / config_.threads;
    int64_t remainder = config_.total_ops % config_.threads;

    auto start = std::chrono::steady_clock::now();
    for (int t = 0; t < config_.threads; ++t) {
        int64_t ops = per_thread + (t < remainder ? 1 : 0);
        threads.emplace_back(&Simulator::worker, this, t, ops, std::ref(partials[t]));
    }
    for (auto& th : threads) {
        th.join();
    }
    auto end = std::chrono::steady_clock::now();

    SimReport merged;
    for (const SimReport& p : partials) {
        for (size_t i = 0; i < merged.ops.size(); ++i) {
            merged.ops[i].attempted += p.ops[i].attempted;
            merged.ops[i].succeeded += p.ops[i].succeeded;
            merged.ops[i].latencies_ns.insert(merged.ops[i].latencies_ns.end(),
                                              p.ops[i].latencies_ns.begin(),
                                              p.ops[i].latencies_ns.end());
        }
    }
    merged.wall_seconds = std::chrono::duration<double>(end - start).count();
    return merged;
}

void Simulator::worker(int index, int64_t ops, SimReport& out) {
    std::mt19937_64 rng(config_.seed * 1000003ULL + static_cast<uint64_t>(index) + 1);
    std::vector<std::string> handles = store_.handles();
    int next_new_profile = 0;

    for (int64_t i = 0; i < ops; ++i) {
        if (i % 1000 == 0) {
            handles = store_.handles();
        }
        Op op = pick_op(rng);
        const std::string& handle = handles[rng() % handles.size()];
        OpResult& result = out.ops[static_cast<size_t>(op)];
        bool ok = false;

        auto t0 = std::chrono::steady_clock::now();
        switch (op) {
            case Op::ViewPage: {
                ok = store_.render_page(handle).has_value();
                if (ok) analytics_.record_view(handle);
                break;
            }
            case Op::Click: {
                std::vector<int64_t> ids = store_.link_ids(handle);
                if (!ids.empty()) {
                    ok = store_.record_click(handle, ids[rng() % ids.size()]);
                    if (ok) analytics_.record_click(handle);
                }
                break;
            }
            case Op::AddLink: {
                ok = store_.add_link(handle, kTitles[rng() % kTitles.size()],
                                     "https://example.com/" + handle + "/n" + std::to_string(i))
                         .has_value();
                break;
            }
            case Op::RemoveLink: {
                std::vector<int64_t> ids = store_.link_ids(handle);
                if (!ids.empty()) {
                    ok = store_.remove_link(handle, ids[rng() % ids.size()]);
                }
                break;
            }
            case Op::CreateProfile: {
                std::string h = "new" + std::to_string(index) + "_" + std::to_string(next_new_profile++);
                ok = store_.create_profile(h, "New user " + h);
                break;
            }
            case Op::UpdateBio: {
                ok = store_.update_bio(handle, "Updated bio #" + std::to_string(i) + " from worker " +
                                                   std::to_string(index));
                break;
            }
            case Op::kCount:
                break;
        }
        auto t1 = std::chrono::steady_clock::now();

        result.attempted += 1;
        result.succeeded += ok ? 1 : 0;
        result.latencies_ns.push_back(
            std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count());
    }
    logging::debug("sim", "worker " + std::to_string(index) + " finished " + std::to_string(ops) + " ops");
}
