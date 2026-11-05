#pragma once

#include <string>
#include <vector>

struct LineItem {
    std::string sku;
    int quantity;
    int unit_price_cents;
};

enum class Fulfillment { Delivery, Pickup };

class Order {
public:
    Order(int id, std::string customer, Fulfillment fulfillment);

    void add_item(const std::string& sku, int quantity, int unit_price_cents);
    void set_delivery_zone(int zone);
    void apply_coupon(int percent_off);

    int id() const { return id_; }
    const std::string& customer() const { return customer_; }
    Fulfillment fulfillment() const { return fulfillment_; }
    int item_count() const;
    int subtotal_cents() const;
    int discount_cents() const;
    int total_cents() const;

private:
    int id_;
    std::string customer_;
    Fulfillment fulfillment_;
    std::vector<LineItem> items_;
    int shipping_cents_;
    int coupon_percent_;
};
