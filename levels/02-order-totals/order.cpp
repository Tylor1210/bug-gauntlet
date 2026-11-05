#include "order.h"

#include <utility>

namespace {

int shipping_for_zone(int zone) {
    switch (zone) {
        case 1: return 499;
        case 2: return 799;
        case 3: return 1299;
        default: return 1999;
    }
}

}  // namespace

Order::Order(int id, std::string customer, Fulfillment fulfillment)
    : id_(id), customer_(std::move(customer)), fulfillment_(fulfillment), coupon_percent_(0) {}

void Order::add_item(const std::string& sku, int quantity, int unit_price_cents) {
    for (LineItem& item : items_) {
        if (item.sku == sku) {
            item.quantity += quantity;
            return;
        }
    }
    items_.push_back({sku, quantity, unit_price_cents});
}

void Order::set_delivery_zone(int zone) {
    shipping_cents_ = fulfillment_ == Fulfillment::Delivery ? shipping_for_zone(zone) : 0;
}

void Order::apply_coupon(int percent_off) {
    if (percent_off > coupon_percent_ && percent_off <= 50) {
        coupon_percent_ = percent_off;
    }
}

int Order::item_count() const {
    int count = 0;
    for (const LineItem& item : items_) {
        count += item.quantity;
    }
    return count;
}

int Order::subtotal_cents() const {
    int subtotal = 0;
    for (const LineItem& item : items_) {
        subtotal += item.quantity * item.unit_price_cents;
    }
    return subtotal;
}

int Order::discount_cents() const {
    return subtotal_cents() * coupon_percent_ / 100;
}

int Order::total_cents() const {
    return subtotal_cents() - discount_cents() + shipping_cents_;
}
