#pragma once

#include <cstddef>

struct Contact {
    char name[48];
    char company[48];
    char street[64];
    char city[32];
    char region[8];
    char postal[12];
    char country[4];
};

bool parse_contact(const char* line, Contact* out);
void format_label(const Contact& c, char* buf, size_t buflen);
