#pragma once

#include <map>
#include <string>

#include "record.h"

struct Series {
    long count = 0;
    double sum = 0;
    double min = 0;
    double max = 0;
    long last_timestamp = 0;
};

class Aggregator {
public:
    explicit Aggregator(long window_start) : window_start_(window_start) {}

    bool accepts(const Sample& s) const;
    void add(const Sample& s);
    const std::map<std::string, Series>& series() const { return series_; }

private:
    long window_start_;
    std::map<std::string, Series> series_;
};
