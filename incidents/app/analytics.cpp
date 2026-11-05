#include "analytics.h"

#include <algorithm>

void Analytics::record_view(const std::string& handle) {
    std::lock_guard<std::mutex> lock(mu_);
    by_handle_[handle].views += 1;
}

void Analytics::record_click(const std::string& handle) {
    std::lock_guard<std::mutex> lock(mu_);
    by_handle_[handle].clicks += 1;
}

int64_t Analytics::views(const std::string& handle) const {
    std::lock_guard<std::mutex> lock(mu_);
    auto it = by_handle_.find(handle);
    return it == by_handle_.end() ? 0 : it->second.views;
}

int64_t Analytics::total_views() const {
    std::lock_guard<std::mutex> lock(mu_);
    int64_t total = 0;
    for (const auto& kv : by_handle_) total += kv.second.views;
    return total;
}

int64_t Analytics::total_clicks() const {
    std::lock_guard<std::mutex> lock(mu_);
    int64_t total = 0;
    for (const auto& kv : by_handle_) total += kv.second.clicks;
    return total;
}

std::vector<std::pair<std::string, int64_t>> Analytics::trending(size_t n) const {
    std::vector<std::pair<std::string, int64_t>> all;
    {
        std::lock_guard<std::mutex> lock(mu_);
        all.reserve(by_handle_.size());
        for (const auto& kv : by_handle_) all.emplace_back(kv.first, kv.second.views);
    }
    std::sort(all.begin(), all.end(), [](const auto& a, const auto& b) {
        return a.second != b.second ? a.second > b.second : a.first < b.first;
    });
    if (all.size() > n) all.resize(n);
    return all;
}
