#include "contact.h"

#include <cstdio>
#include <cstring>

namespace {

const char* copy_field(const char* src, char* dst, size_t dstlen) {
    size_t i = 0;
    while (*src != '\0' && *src != ';' && *src != '\n') {
        if (i + 1 < dstlen) {
            dst[i++] = *src;
        }
        ++src;
    }
    dst[i] = '\0';
    return *src == ';' ? src + 1 : src;
}

}  // namespace

bool parse_contact(const char* line, Contact* out) {
    std::memset(out, 0, sizeof(*out));
    const char* p = line;
    p = copy_field(p, out->name, sizeof(out->name));
    p = copy_field(p, out->company, sizeof(out->company));
    p = copy_field(p, out->street, sizeof(out->street));
    p = copy_field(p, out->city, sizeof(out->city));
    p = copy_field(p, out->region, sizeof(out->region));
    p = copy_field(p, out->postal, sizeof(out->postal));
    copy_field(p, out->country, sizeof(out->country));
    return out->name[0] != '\0' && out->street[0] != '\0' && out->postal[0] != '\0';
}

void format_label(const Contact& c, char* buf, size_t buflen) {
    size_t n = 0;
    n += std::snprintf(buf + n, buflen - n, "%s\n", c.name);
    if (c.company[0] != '\0') {
        n += std::snprintf(buf + n, buflen - n, "%s\n", c.company);
    }
    n += std::snprintf(buf + n, buflen - n, "%s\n", c.street);
    n += std::snprintf(buf + n, buflen - n, "%s, %s %s\n", c.city, c.region, c.postal);
    if (std::strcmp(c.country, "US") != 0) {
        n += std::snprintf(buf + n, buflen - n, "%s\n", c.country);
    }
}
