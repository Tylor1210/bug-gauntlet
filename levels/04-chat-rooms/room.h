#pragma once

#include <string>
#include <vector>

struct Member {
    std::string handle;
    int messages_sent = 0;
    int mentions = 0;
    bool moderator = false;
};

class Room {
public:
    explicit Room(std::string name) : name_(std::move(name)) {}

    Member& join(const std::string& handle);
    Member* find(const std::string& handle);
    void post(Member& author, const std::string& text);

    const std::string& name() const { return name_; }
    const std::vector<Member>& members() const { return members_; }
    int total_messages() const { return total_messages_; }

private:
    void record_mentions(const std::string& text);

    std::string name_;
    std::vector<Member> members_;
    int total_messages_ = 0;
};
