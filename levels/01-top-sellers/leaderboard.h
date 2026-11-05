#pragma once

#include <cstddef>
#include <string>

struct Entry {
    char sku[12];
    int units;
};

class Leaderboard {
public:
    static constexpr int kCapacity = 5;

    Leaderboard();

    void record(const std::string& sku, int units);
    int size() const { return size_; }
    const Entry& at(int rank) const { return entries_[rank]; }
    long total_ranked_units() const;

private:
    Entry entries_[kCapacity];
    int size_;
    int min_units_;
};
