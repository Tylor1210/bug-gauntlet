#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

#include "aggregator.h"
#include "record.h"

int main(int argc, char** argv) {
    const char* path = argc > 1 ? argv[1] : "metrics.log";
    int passes = argc > 2 ? std::atoi(argv[2]) : 1;

    LineParser parser('|');
    Aggregator agg(1761700000);
    long malformed = 0, dropped = 0, accepted = 0;

    for (int pass = 0; pass < passes; ++pass) {
        std::ifstream file(path);
        if (!file) {
            std::cerr << "cannot open " << path << "\n";
            return 1;
        }
        std::string line;
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') {
                continue;
            }
            Sample* sample = parser.parse(line);
            if (sample == nullptr) {
                ++malformed;
                continue;
            }
            if (!agg.accepts(*sample)) {
                ++dropped;
                continue;
            }
            agg.add(*sample);
            ++accepted;
            delete sample;
        }
    }

    std::printf("accepted=%ld dropped=%ld malformed=%ld\n", accepted, dropped, malformed);
    std::printf("%-22s %6s %10s %10s %10s\n", "SERIES", "COUNT", "MIN", "AVG", "MAX");
    for (const auto& [key, s] : agg.series()) {
        std::printf("%-22s %6ld %10.2f %10.2f %10.2f\n", key.c_str(), s.count, s.min,
                    s.sum / s.count, s.max);
    }
    return 0;
}
