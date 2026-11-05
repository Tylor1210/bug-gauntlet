// analytics.h: per-profile page-view and click counters plus a "trending"
// top-N query. Fed by the simulator alongside the store. Thread-safe via its
// own mutex (separate from the store's).
#pragma once

#include <cstdint>
#include <mutex>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

class Analytics {
public:
    void record_view(const std::string& handle);
    void record_click(const std::string& handle);

    int64_t views(const std::string& handle) const;
    int64_t total_views() const;
    int64_t total_clicks() const;
    // Top n profiles by views, ties broken by handle.
    std::vector<std::pair<std::string, int64_t>> trending(size_t n) const;

private:
    struct Counters {
        int64_t views = 0;
        int64_t clicks = 0;
    };

    mutable std::mutex mu_;
    std::unordered_map<std::string, Counters> by_handle_;
};
