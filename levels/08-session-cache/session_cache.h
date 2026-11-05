#pragma once

#include <cstddef>
#include <string>
#include <unordered_map>

struct Session {
    std::string user;
    std::string token;
    long last_seen;
};

class SessionCache {
public:
    explicit SessionCache(size_t capacity);
    ~SessionCache();

    SessionCache(const SessionCache&) = delete;
    SessionCache& operator=(const SessionCache&) = delete;

    void login(const std::string& user, const std::string& token, long now);
    const Session* touch(const std::string& user, long now);
    bool logout(const std::string& user);

    size_t size() const { return index_.size(); }
    long evictions() const { return evictions_; }

    template <typename F>
    void for_each_recent(F f) const {
        for (const Node* n = head_; n != nullptr; n = n->next) {
            f(n->session);
        }
    }

private:
    struct Node {
        Session session;
        Node* prev = nullptr;
        Node* next = nullptr;
    };

    void push_front(Node* node);
    void unlink(Node* node);
    void evict_oldest();

    size_t capacity_;
    Node* head_ = nullptr;
    Node* tail_ = nullptr;
    std::unordered_map<std::string, Node*> index_;
    long evictions_ = 0;
};
