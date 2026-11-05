#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "frame.h"

class Decoder {
public:
    Decoder(int width, int height, int gop) : width_(width), height_(height), gop_(gop) {}
    Frame decode(int index) const;

private:
    int width_;
    int height_;
    int gop_;
};

struct FrameStats {
    long frames = 0;
    long keyframes = 0;
    double mean_luma_sum = 0;
    void add(const Frame& f);
};

class Archive {
public:
    void store(Frame frame);
    const std::vector<std::string>& manifest() const { return manifest_; }

private:
    std::vector<std::string> manifest_;
};
