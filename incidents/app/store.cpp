#include "store.h"

#include <algorithm>
#include <sstream>

#include "log.h"

bool LinkStore::create_profile(const std::string& handle, const std::string& display_name) {
    std::lock_guard<std::mutex> lock(mu_);
    if (handle.empty() || profiles_.count(handle) != 0) {
        return false;
    }
    Profile p;
    p.id = next_profile_id_++;
    p.handle = handle;
    p.display_name = display_name;
    profiles_.emplace(handle, std::move(p));
    return true;
}

bool LinkStore::update_bio(const std::string& handle, const std::string& bio) {
    std::lock_guard<std::mutex> lock(mu_);
    auto it = profiles_.find(handle);
    if (it == profiles_.end()) {
        return false;
    }
    it->second.bio = bio.substr(0, 160);
    return true;
}

std::optional<int64_t> LinkStore::add_link(const std::string& handle, const std::string& title,
                                           const std::string& url) {
    std::lock_guard<std::mutex> lock(mu_);
    auto it = profiles_.find(handle);
    if (it == profiles_.end()) {
        return std::nullopt;
    }
    Profile& p = it->second;
    if (p.links.size() >= kMaxLinksPerProfile) {
        logging::debug("store", "link limit reached for @" + handle);
        return std::nullopt;
    }
    Link link;
    link.id = next_link_id_++;
    link.title = title;
    link.url = url;
    p.links.push_back(std::move(link));
    return p.links.back().id;
}

bool LinkStore::remove_link(const std::string& handle, int64_t link_id) {
    std::lock_guard<std::mutex> lock(mu_);
    auto it = profiles_.find(handle);
    if (it == profiles_.end()) {
        return false;
    }
    auto& links = it->second.links;
    auto pos = std::find_if(links.begin(), links.end(),
                            [&](const Link& l) { return l.id == link_id; });
    if (pos == links.end()) {
        return false;
    }
    archived_clicks_ += pos->clicks;
    links.erase(pos);
    return true;
}

bool LinkStore::record_click(const std::string& handle, int64_t link_id) {
    std::lock_guard<std::mutex> lock(mu_);
    auto it = profiles_.find(handle);
    if (it == profiles_.end()) {
        return false;
    }
    for (Link& l : it->second.links) {
        if (l.id == link_id && l.active) {
            l.clicks += 1;
            return true;
        }
    }
    return false;
}

std::optional<std::string> LinkStore::render_page(const std::string& handle) const {
    std::lock_guard<std::mutex> lock(mu_);
    auto it = profiles_.find(handle);
    if (it == profiles_.end()) {
        return std::nullopt;
    }
    const Profile& p = it->second;
    std::ostringstream out;
    out << p.display_name << " (@" << p.handle << ")\n";
    if (!p.bio.empty()) {
        out << p.bio << "\n";
    }
    for (const Link& l : p.links) {
        if (l.active) {
            out << "  - " << l.title << " <" << l.url << ">\n";
        }
    }
    return out.str();
}

std::optional<Profile> LinkStore::get_profile(const std::string& handle) const {
    std::lock_guard<std::mutex> lock(mu_);
    auto it = profiles_.find(handle);
    if (it == profiles_.end()) {
        return std::nullopt;
    }
    return it->second;
}

std::vector<int64_t> LinkStore::link_ids(const std::string& handle) const {
    std::lock_guard<std::mutex> lock(mu_);
    std::vector<int64_t> ids;
    auto it = profiles_.find(handle);
    if (it != profiles_.end()) {
        for (const Link& l : it->second.links) {
            ids.push_back(l.id);
        }
    }
    return ids;
}

std::vector<std::string> LinkStore::handles() const {
    std::lock_guard<std::mutex> lock(mu_);
    std::vector<std::string> out;
    out.reserve(profiles_.size());
    for (const auto& kv : profiles_) {
        out.push_back(kv.first);
    }
    std::sort(out.begin(), out.end());
    return out;
}

StoreStats LinkStore::stats() const {
    std::lock_guard<std::mutex> lock(mu_);
    StoreStats s;
    s.profiles = profiles_.size();
    s.archived_clicks = archived_clicks_;
    for (const auto& kv : profiles_) {
        s.links += kv.second.links.size();
        for (const Link& l : kv.second.links) {
            s.total_clicks += l.clicks;
        }
    }
    return s;
}
