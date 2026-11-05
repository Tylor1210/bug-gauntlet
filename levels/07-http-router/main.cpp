#include <cstdio>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

#include "router.h"

int main(int argc, char** argv) {
    const char* path = argc > 1 ? argv[1] : "access.log";

    Router router;
    router.add("GET", "/", "pages.home");
    router.add("GET", "/u/:handle", "profiles.show_public_profile");
    router.add("GET", "/u/:handle/links", "profiles.list_links_for_profile");
    router.add("POST", "/u/:handle/links", "profiles.create_link_for_profile");
    router.add("GET", "/u/admin", "admin.dashboard_requires_auth", 10);
    router.add("GET", "/api/v1/health", "ops.healthcheck", 5);
    router.add("*", "/api/*", "api.dispatch_versioned_request");
    router.add("GET", "/static/*", "static.serve_asset_from_bucket");

    std::ifstream file(path);
    if (!file) {
        std::cerr << "cannot open " << path << "\n";
        return 1;
    }

    std::map<std::string, int> hits;
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream in(line);
        std::string method, target;
        if (!(in >> method >> target)) {
            continue;
        }
        const Route& route = router.resolve(method, target);
        std::printf("%-5s %-26s -> %s\n", method.c_str(), target.c_str(), route.handler.c_str());
        hits[route.handler] += 1;
    }

    std::printf("\nhandler hit counts:\n");
    for (const auto& [handler, count] : hits) {
        std::printf("  %4d  %s\n", count, handler.c_str());
    }
    return 0;
}
