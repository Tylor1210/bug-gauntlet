#include "router.h"

#include <algorithm>

namespace {

std::vector<std::string> segments(const std::string& path) {
    std::vector<std::string> out;
    size_t start = 0;
    while (start <= path.size()) {
        size_t slash = path.find('/', start);
        if (slash == std::string::npos) slash = path.size();
        if (slash > start) out.push_back(path.substr(start, slash - start));
        start = slash + 1;
    }
    return out;
}

std::string first_segment(const std::string& path) {
    std::vector<std::string> segs = segments(path);
    return segs.empty() ? "" : segs.front();
}

}  // namespace

bool Route::matches(const std::string& m, const std::string& path) const {
    if (method != "*" && method != m) return false;
    std::vector<std::string> want = segments(pattern);
    std::vector<std::string> got = segments(path);
    for (size_t i = 0; i < want.size(); ++i) {
        if (want[i] == "*") return true;
        if (i >= got.size()) return false;
        if (want[i][0] == ':') continue;
        if (want[i] != got[i]) return false;
    }
    return want.size() == got.size();
}

Router::Router() : not_found_{"*", "/*", "static.not_found_page", -1} {}

void Router::add(const std::string& method, const std::string& pattern, const std::string& handler,
                 int priority) {
    routes_.push_back({method, pattern, handler, priority});
}

std::vector<Route> Router::candidates_for(const std::string& path) const {
    std::string head = first_segment(path);
    std::vector<Route> out;
    for (const Route& r : routes_) {
        std::string route_head = first_segment(r.pattern);
        if (route_head == head || route_head == "*" || route_head.empty()) {
            out.push_back(r);
        }
    }
    std::stable_sort(out.begin(), out.end(),
                     [](const Route& a, const Route& b) { return a.priority > b.priority; });
    return out;
}

const Route& Router::resolve(const std::string& method, const std::string& path) const {
    std::vector<Route> candidates = candidates_for(path);
    for (const Route& r : candidates) {
        if (r.matches(method, path)) {
            return r;
        }
    }
    return not_found_;
}
