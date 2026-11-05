#pragma once

#include <cstddef>
#include <cstdint>

class Frame {
public:
    Frame(int index, int width, int height, bool keyframe);
    ~Frame();

    int index() const { return index_; }
    int width() const { return width_; }
    int height() const { return height_; }
    bool is_keyframe() const { return keyframe_; }
    size_t size() const { return static_cast<size_t>(width_) * height_; }

    uint8_t* pixels() { return pixels_; }
    const uint8_t* pixels() const { return pixels_; }

    uint8_t pixel(int x, int y) const { return pixels_[y * width_ + x]; }

private:
    int index_;
    int width_;
    int height_;
    bool keyframe_;
    uint8_t* pixels_;
};
