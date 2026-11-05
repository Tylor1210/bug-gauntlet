// simulator.h: request simulator that hammers LinkStore + Analytics with a
// weighted mix of operations from N worker threads, timing each operation.
// Called by main.cpp. Depends on store.h, analytics.h, log.h.
#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <vector>

#include "analytics.h"
#include "store.h"

enum class Op { ViewPage, Click, AddLink, RemoveLink, CreateProfile, UpdateBio, kCount };
const char* op_name(Op op);

struct SimConfig {
    int64_t total_ops = 20000;
    int threads = 1;
    uint64_t seed = 42;
    int initial_profiles = 200;
};

struct OpResult {
    int64_t attempted = 0;
    int64_t succeeded = 0;
    std::vector<int64_t> latencies_ns;
};

struct SimReport {
    std::array<OpResult, static_cast<size_t>(Op::kCount)> ops;
    double wall_seconds = 0;
};

class Simulator {
public:
    Simulator(LinkStore& store, Analytics& analytics, SimConfig config);

    void seed_data();
    SimReport run();

private:
    void worker(int index, int64_t ops, SimReport& out);

    LinkStore& store_;
    Analytics& analytics_;
    SimConfig config_;
};
