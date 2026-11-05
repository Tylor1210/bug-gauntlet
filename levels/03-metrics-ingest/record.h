#pragma once

#include <string>
#include <vector>

struct Sample {
    std::string host;
    std::string metric;
    double value;
    long timestamp;
    std::vector<std::string> tags;
};

class LineParser {
public:
    explicit LineParser(char delimiter) : delimiter_(delimiter) {}

    Sample* parse(const std::string& line) const;

private:
    std::vector<std::string> split(const std::string& line) const;

    char delimiter_;
};
