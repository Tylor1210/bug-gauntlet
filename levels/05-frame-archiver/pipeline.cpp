#include "pipeline.h"

#include <cstdio>

Frame Decoder::decode(int index) const {
    Frame frame(index, width_, height_, index % gop_ == 0);
    uint8_t* p = frame.pixels();
    for (int y = 0; y < height_; ++y) {
        for (int x = 0; x < width_; ++x) {
            p[y * width_ + x] = static_cast<uint8_t>((x * 7 + y * 13 + index * 31) & 0xff);
        }
    }
    return frame;
}

void FrameStats::add(const Frame& f) {
    long sum = 0;
    for (size_t i = 0; i < f.size(); ++i) {
        sum += f.pixels()[i];
    }
    frames += 1;
    keyframes += f.is_keyframe() ? 1 : 0;
    mean_luma_sum += static_cast<double>(sum) / f.size();
}

namespace {

uint32_t adler32(const uint8_t* data, size_t len) {
    uint32_t a = 1, b = 0;
    for (size_t i = 0; i < len; ++i) {
        a = (a + data[i]) % 65521;
        b = (b + a) % 65521;
    }
    return (b << 16) | a;
}

}  // namespace

void Archive::store(Frame frame) {
    char entry[96];
    std::snprintf(entry, sizeof(entry), "frame-%05d.raw  %dx%d  adler32=%08x  corner=%u",
                  frame.index(), frame.width(), frame.height(),
                  adler32(frame.pixels(), frame.size()),
                  static_cast<unsigned>(frame.pixel(frame.width() - 1, frame.height() - 1)));
    manifest_.push_back(entry);
}
