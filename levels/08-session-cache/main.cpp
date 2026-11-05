#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "session_cache.h"

// Replays an auth event stream: "<time> login <user> <token>",
// "<time> request <user>", "<time> logout <user>".
int main(int argc, char** argv) {
    const char* path = argc > 1 ? argv[1] : "events.txt";
    std::ifstream file(path);
    if (!file) {
        std::cerr << "cannot open " << path << "\n";
        return 1;
    }

    SessionCache cache(4);
    int served = 0, rejected = 0;
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream in(line);
        long now = 0;
        std::string op, user, token;
        if (!(in >> now >> op >> user)) {
            continue;
        }
        if (op == "login") {
            in >> token;
            cache.login(user, token, now);
        } else if (op == "request") {
            const Session* s = cache.touch(user, now);
            if (s != nullptr) {
                ++served;
            } else {
                ++rejected;
                std::printf("t=%ld  401 for %s (session expired)\n", now, user.c_str());
            }
        } else if (op == "logout") {
            cache.logout(user);
        }
    }

    std::printf("\nserved=%d rejected=%d evictions=%ld active=%zu\n", served, rejected,
                cache.evictions(), cache.size());
    std::printf("most recent first:\n");
    cache.for_each_recent([](const Session& s) {
        std::printf("  %-6s token=%s last_seen=%ld\n", s.user.c_str(), s.token.c_str(), s.last_seen);
    });
    return 0;
}
