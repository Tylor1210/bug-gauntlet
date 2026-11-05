#include "leaderboard.h"

#include <cstdio>

Leaderboard::Leaderboard() : entries_{}, size_(0), min_units_(0) {}

void Leaderboard::record(const std::string& sku, int units) {
    if (size_ == kCapacity && units <= min_units_) {
        return;
    }

    int pos = size_;
    while (pos > 0 && entries_[pos - 1].units < units) {
        --pos;
    }
    
    for (int i = kCapacity - 1; i > pos; --i) {
        entries_[i] = entries_[i - 1];
    }

    Entry& slot = entries_[pos];
    std::snprintf(slot.sku, sizeof(slot.sku), "%s", sku.c_str());
    slot.units = units;

    if (size_ < kCapacity) {
        ++size_;
    }
    min_units_ = entries_[size_ - 1].units;
}

long Leaderboard::total_ranked_units() const {
    long total = 0;
    for (int i = 0; i < size_; ++i) {
        total += entries_[i].units;
    }
    return total;
}
