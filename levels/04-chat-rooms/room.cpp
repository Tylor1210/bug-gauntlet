#include "room.h"

#include <cctype>

Member& Room::join(const std::string& handle) {
    if (Member* existing = find(handle)) {
        return *existing;
    }
    members_.push_back(Member{handle});
    return members_.back();
}

Member* Room::find(const std::string& handle) {
    for (Member& m : members_) {
        if (m.handle == handle) {
            return &m;
        }
    }
    return nullptr;
}

void Room::post(Member& author, const std::string& text) {
    record_mentions(text);
    author.messages_sent += 1;
    total_messages_ += 1;
}

void Room::record_mentions(const std::string& text) {
    size_t pos = 0;
    while ((pos = text.find('@', pos)) != std::string::npos) {
        size_t end = pos + 1;
        while (end < text.size() && (std::isalnum(static_cast<unsigned char>(text[end])) || text[end] == '_')) {
            ++end;
        }
        if (end > pos + 1) {
            join(text.substr(pos + 1, end - pos - 1)).mentions += 1;
        }
        pos = end;
    }
}
