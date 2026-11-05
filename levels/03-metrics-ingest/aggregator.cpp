#include "aggregator.h"

bool Aggregator::accepts(const Sample& s) const {
    if (s.timestamp < window_start_) {
        return false;
    }
    for (const std::string& tag : s.tags) {
        if (tag == "synthetic") {
            return false;
        }
    }
    return true;
}

void Aggregator::add(const Sample& s) {
    Series& series = series_[s.host + "/" + s.metric];
    if (series.count == 0 || s.value < series.min) series.min = s.value;
    if (series.count == 0 || s.value > series.max) series.max = s.value;
    series.count += 1;
    series.sum += s.value;
    if (s.timestamp > series.last_timestamp) series.last_timestamp = s.timestamp;
}
