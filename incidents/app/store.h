// store.h: in-memory store of profiles and their links (the "database" of the
// link page service). Called by the simulator's request handlers. Thread-safe:
// every public method takes the store mutex. Depends on log.h.
#pragma once

#include <cstdint>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

struct Link {
    int64_t id = 0;
    std::string title;
    std::string url;
    int64_t clicks = 0;
    bool active = true;
};

struct Profile {
    int64_t id = 0;
    std::string handle;
    std::string display_name;
    std::string bio;
    std::vector<Link> links;  // display order
};

struct StoreStats {
    size_t profiles = 0;
    size_t links = 0;
    int64_t total_clicks = 0;
    int64_t archived_clicks = 0;  // clicks on links that were later removed
};

class LinkStore {
public:
    static constexpr size_t kMaxLinksPerProfile = 50;

    bool create_profile(const std::string& handle, const std::string& display_name);
    bool update_bio(const std::string& handle, const std::string& bio);
    std::optional<int64_t> add_link(const std::string& handle, const std::string& title,
                                    const std::string& url);
    bool remove_link(const std::string& handle, int64_t link_id);
    bool record_click(const std::string& handle, int64_t link_id);

    // Renders the public page as plain text. nullopt if the handle is unknown.
    std::optional<std::string> render_page(const std::string& handle) const;
    std::optional<Profile> get_profile(const std::string& handle) const;
    std::vector<int64_t> link_ids(const std::string& handle) const;
    std::vector<std::string> handles() const;
    StoreStats stats() const;

private:
    mutable std::mutex mu_;
    std::unordered_map<std::string, Profile> profiles_;
    int64_t next_profile_id_ = 1;
    int64_t next_link_id_ = 1;
    int64_t archived_clicks_ = 0;
};
