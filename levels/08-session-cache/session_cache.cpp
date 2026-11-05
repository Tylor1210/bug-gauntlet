#include "session_cache.h"

SessionCache::SessionCache(size_t capacity) : capacity_(capacity) {}

SessionCache::~SessionCache() {
    Node* n = head_;
    while (n != nullptr) {
        Node* next = n->next;
        delete n;
        n = next;
    }
}

void SessionCache::push_front(Node* node) {
    node->prev = nullptr;
    node->next = head_;
    if (head_ != nullptr) {
        head_->prev = node;
    }
    head_ = node;
    if (tail_ == nullptr) {
        tail_ = node;
    }
}

void SessionCache::unlink(Node* node) {
    if (node->prev != nullptr) {
        node->prev->next = node->next;
    } else {
        head_ = node->next;
    }
    if (node->next != nullptr) {
        node->next->prev = node->prev;
    }
    node->prev = nullptr;
    node->next = nullptr;
}

void SessionCache::evict_oldest() {
    Node* victim = tail_;
    if (victim == nullptr) {
        return;
    }
    tail_ = victim->prev;
    if (tail_ != nullptr) {
        tail_->next = nullptr;
    } else {
        head_ = nullptr;
    }
    index_.erase(victim->session.user);
    delete victim;
    ++evictions_;
}

void SessionCache::login(const std::string& user, const std::string& token, long now) {
    auto it = index_.find(user);
    if (it != index_.end()) {
        Node* node = it->second;
        node->session.token = token;
        node->session.last_seen = now;
        unlink(node);
        push_front(node);
        return;
    }
    if (index_.size() >= capacity_) {
        evict_oldest();
    }
    Node* node = new Node{Session{user, token, now}};
    push_front(node);
    index_[user] = node;
}

const Session* SessionCache::touch(const std::string& user, long now) {
    auto it = index_.find(user);
    if (it == index_.end()) {
        return nullptr;
    }
    Node* node = it->second;
    node->session.last_seen = now;
    if (node != head_) {
        unlink(node);
        push_front(node);
    }
    return &node->session;
}

bool SessionCache::logout(const std::string& user) {
    auto it = index_.find(user);
    if (it == index_.end()) {
        return false;
    }
    Node* node = it->second;
    if (node == tail_) {
        tail_ = node->prev;
    }
    unlink(node);
    index_.erase(it);
    delete node;
    return true;
}
