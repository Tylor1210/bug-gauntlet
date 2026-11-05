#pragma once

#include <string>
#include <vector>

struct Route {
    std::string method;
    std::string pattern;
    std::string handler;
    int priority;

    bool matches(const std::string& m, const std::string& path) const;
};

class Router {
public:
    Router();

    void add(const std::string& method, const std::string& pattern, const std::string& handler,
             int priority = 0);
    const Route& resolve(const std::string& method, const std::string& path) const;

private:
    std::vector<Route> candidates_for(const std::string& path) const;

    std::vector<Route> routes_;
    Route not_found_;
};
