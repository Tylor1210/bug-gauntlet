#include <cstdio>
#include <cstring>

#include "contact.h"

int main(int argc, char** argv) {
    const char* path = argc > 1 ? argv[1] : "contacts.txt";
    FILE* in = std::fopen(path, "r");
    if (in == nullptr) {
        std::fprintf(stderr, "cannot open %s\n", path);
        return 1;
    }

    char line[512];
    int printed = 0, skipped = 0;
    while (std::fgets(line, sizeof(line), in) != nullptr) {
        if (line[0] == '#' || line[0] == '\n') {
            continue;
        }
        Contact contact;
        if (!parse_contact(line, &contact)) {
            ++skipped;
            continue;
        }

        char label[96];
        format_label(contact, label, sizeof(label));
        std::printf("+------------------------------\n");
        std::fputs(label, stdout);
        size_t len = std::strlen(label);
        if (len == 0 || label[len - 1] != '\n') {
            std::fputc('\n', stdout);
        }
        ++printed;
    }
    std::fclose(in);

    std::printf("+------------------------------\n");
    std::printf("%d labels printed, %d skipped\n", printed, skipped);
    return 0;
}
