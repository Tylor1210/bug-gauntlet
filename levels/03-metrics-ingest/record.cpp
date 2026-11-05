#include "record.h"

#include <cstdlib>

std::vector<std::string> LineParser::split(const std::string& line) const {
    std::vector<std::string> fields;
    std::string current;
    for (char c : line) {
        if (c == delimiter_) {
            fields.push_back(current);
            current.clear();
        } else {
            current += c;
        }
    }
    fields.push_back(current);
    return fields;
}

Sample* LineParser::parse(const std::string& line) const {
    std::vector<std::string> fields = split(line);
    if (fields.size() < 4) {
        return nullptr;
    }

    Sample* sample = new Sample;
    sample->host = fields[0];
    sample->metric = fields[1];
    sample->value = std::strtod(fields[2].c_str(), nullptr);
    sample->timestamp = std::strtol(fields[3].c_str(), nullptr, 10);
    for (size_t i = 4; i < fields.size(); ++i) {
        if (!fields[i].empty()) {
            sample->tags.push_back(fields[i]);
        }
    }
    return sample;
}
