// main.cpp: entry point for linkpage-sim. Parses flags, seeds the store, runs
// the simulator, prints a report, and checks cross-component invariants.
// Exit code: 0 = invariants hold, 1 = bad flags, 2 = invariant violated.
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <string>

#include "analytics.h"
#include "log.h"
#include "simulator.h"
#include "store.h"

namespace {

void usage() {
    std::fprintf(stderr,
                 "usage: linkpage-sim [--ops N] [--threads T] [--seed S] [--profiles P] [--verbose]\n");
}

int64_t percentile(std::vector<int64_t>& v, double p) {
    if (v.empty()) return 0;
    size_t k = static_cast<size_t>(p * (v.size() - 1));
    std::nth_element(v.begin(), v.begin() + k, v.end());
    return v[k];
}

// Order-independent fingerprint of every rendered page, for comparing runs.
uint64_t state_checksum(const LinkStore& store) {
    uint64_t sum = 0;
    for (const std::string& h : store.handles()) {
        if (auto page = store.render_page(h)) {
            sum += std::hash<std::string>{}(*page);
        }
    }
    return sum;
}

}  // namespace

int main(int argc, char** argv) {
    SimConfig config;
    for (int i = 1; i < argc; ++i) {
        auto next = [&](const char* flag) -> const char* {
            if (i + 1 >= argc) {
                std::fprintf(stderr, "%s needs a value\n", flag);
                std::exit(1);
            }
            return argv[++i];
        };
        if (!std::strcmp(argv[i], "--ops")) config.total_ops = std::atoll(next("--ops"));
        else if (!std::strcmp(argv[i], "--threads")) config.threads = std::atoi(next("--threads"));
        else if (!std::strcmp(argv[i], "--seed")) config.seed = std::strtoull(next("--seed"), nullptr, 10);
        else if (!std::strcmp(argv[i], "--profiles")) config.initial_profiles = std::atoi(next("--profiles"));
        else if (!std::strcmp(argv[i], "--verbose")) logging::set_level(LogLevel::Debug);
        else { usage(); return 1; }
    }
    if (config.threads < 1 || config.total_ops < 0 || config.initial_profiles < 1) {
        usage();
        return 1;
    }

    LinkStore store;
    Analytics analytics;
    Simulator sim(store, analytics, config);
    sim.seed_data();
    SimReport report = sim.run();

    std::printf("linkpage-sim  ops=%lld threads=%d seed=%llu\n",
                static_cast<long long>(config.total_ops), config.threads,
                static_cast<unsigned long long>(config.seed));
    std::printf("wall time %.3fs  (%.0f ops/s)\n\n", report.wall_seconds,
                report.wall_seconds > 0 ? config.total_ops / report.wall_seconds : 0.0);
    std::printf("%-15s %9s %9s %10s %10s\n", "OP", "ATTEMPTED", "OK", "p50(us)", "p99(us)");
    int64_t clicks_ok = 0, views_ok = 0;
    for (size_t i = 0; i < report.ops.size(); ++i) {
        OpResult& r = report.ops[i];
        std::printf("%-15s %9lld %9lld %10.1f %10.1f\n", op_name(static_cast<Op>(i)),
                    static_cast<long long>(r.attempted), static_cast<long long>(r.succeeded),
                    percentile(r.latencies_ns, 0.50) / 1000.0, percentile(r.latencies_ns, 0.99) / 1000.0);
        if (static_cast<Op>(i) == Op::Click) clicks_ok = r.succeeded;
        if (static_cast<Op>(i) == Op::ViewPage) views_ok = r.succeeded;
    }

    StoreStats s = store.stats();
    std::printf("\nstore: %zu profiles, %zu links, %lld live clicks, %lld archived clicks\n", s.profiles,
                s.links, static_cast<long long>(s.total_clicks), static_cast<long long>(s.archived_clicks));
    std::printf("trending:");
    for (const auto& [handle, views] : analytics.trending(5)) {
        std::printf("  @%s(%lld)", handle.c_str(), static_cast<long long>(views));
    }
    std::printf("\nstate checksum: %016llx\n\n", static_cast<unsigned long long>(state_checksum(store)));

    bool ok = true;
    auto check = [&](bool cond, const std::string& what) {
        std::printf("  [%s] %s\n", cond ? " ok " : "FAIL", what.c_str());
        ok = ok && cond;
    };
    std::printf("invariants:\n");
    check(s.total_clicks + s.archived_clicks == clicks_ok,
          "store clicks (live + archived) == successful click ops (" + std::to_string(clicks_ok) + ")");
    check(analytics.total_clicks() == clicks_ok, "analytics clicks == successful click ops");
    check(analytics.total_views() == views_ok,
          "analytics views == successful page views (" + std::to_string(views_ok) + ")");
    return ok ? 0 : 2;
}
