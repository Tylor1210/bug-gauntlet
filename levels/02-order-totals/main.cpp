#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "order.h"

namespace {

std::string money(long cents) {
    char buf[32];
    std::snprintf(buf, sizeof(buf), "%s$%ld.%02ld", cents < 0 ? "-" : "",
                  std::labs(cents) / 100, std::labs(cents) % 100);
    return buf;
}

}  // namespace

// Input format, one command per line:
//   order <id> <customer> <delivery|pickup>
//   item <id> <sku> <qty> <unit_price_cents>
//   zone <id> <zone>
//   coupon <id> <percent>
int main(int argc, char** argv) {
    const char* path = argc > 1 ? argv[1] : "orders.txt";
    std::ifstream file(path);
    if (!file) {
        std::cerr << "cannot open " << path << "\n";
        return 1;
    }

    std::map<int, std::unique_ptr<Order>> orders;
    std::string line;
    int line_no = 0;
    while (std::getline(file, line)) {
        ++line_no;
        if (line.empty() || line[0] == '#') {
            continue;
        }
        std::istringstream in(line);
        std::string cmd;
        int id = 0;
        in >> cmd >> id;

        if (cmd == "order") {
            std::string customer, mode;
            in >> customer >> mode;
            Fulfillment f = mode == "pickup" ? Fulfillment::Pickup : Fulfillment::Delivery;
            orders[id] = std::make_unique<Order>(id, customer, f);
            continue;
        }

        auto it = orders.find(id);
        if (it == orders.end()) {
            std::cerr << "line " << line_no << ": unknown order " << id << "\n";
            continue;
        }
        Order& order = *it->second;

        if (cmd == "item") {
            std::string sku;
            int qty = 0, price = 0;
            in >> sku >> qty >> price;
            order.add_item(sku, qty, price);
        } else if (cmd == "zone") {
            int zone = 0;
            in >> zone;
            order.set_delivery_zone(zone);
        } else if (cmd == "coupon") {
            int pct = 0;
            in >> pct;
            order.apply_coupon(pct);
        } else {
            std::cerr << "line " << line_no << ": unknown command " << cmd << "\n";
        }
    }

    long grand_total = 0;
    int pickups = 0;
    std::printf("%-6s %-10s %-9s %5s %12s\n", "ORDER", "CUSTOMER", "TYPE", "ITEMS", "TOTAL");
    for (const auto& [id, order] : orders) {
        bool pickup = order->fulfillment() == Fulfillment::Pickup;
        pickups += pickup ? 1 : 0;
        std::printf("%-6d %-10s %-9s %5d %12s\n", id, order->customer().c_str(),
                    pickup ? "pickup" : "delivery", order->item_count(),
                    money(order->total_cents()).c_str());
        grand_total += order->total_cents();
    }
    std::printf("\n%zu orders (%d pickup), revenue %s\n", orders.size(), pickups,
                money(grand_total).c_str());
    return 0;
}
