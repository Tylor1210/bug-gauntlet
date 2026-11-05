#include <cstdio>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

#include "leaderboard.h"

namespace {

bool parse_line(const std::string& line, std::string& sku, int& units) {
    std::istringstream in(line);
    std::string units_field;
    if (!std::getline(in, sku, ',') || !std::getline(in, units_field)) {
        return false;
    }
    try {
        units = std::stoi(units_field);
    } catch (...) {
        return false;
    }
    return !sku.empty() && units >= 0;
}

}  // namespace

int main(int argc, char** argv) {
    const char* path = argc > 1 ? argv[1] : "sales.csv";
    std::ifstream file(path);
    if (!file) {
        std::cerr << "cannot open " << path << "\n";
        return 1;
    }

    std::map<std::string, int> units_by_sku;
    std::string line;
    int rejected = 0;
    std::getline(file, line);
    while (std::getline(file, line)) {
        std::string sku;
        int units = 0;
        if (!parse_line(line, sku, units)) {
            ++rejected;
            continue;
        }
        units_by_sku[sku] += units;
    }

    Leaderboard board;
    for (const auto& [sku, units] : units_by_sku) {
        board.record(sku, units);
    }

    std::printf("Top %d sellers (%zu SKUs, %d rows rejected)\n",
                Leaderboard::kCapacity, units_by_sku.size(), rejected);
    for (int rank = 0; rank < board.size(); ++rank) {
        const Entry& e = board.at(rank);
        std::printf("  #%d  %-10s %6d\n", rank + 1, e.sku, e.units);
    }
    std::printf("Units in top %d: %ld\n", Leaderboard::kCapacity, board.total_ranked_units());
    return 0;
}
