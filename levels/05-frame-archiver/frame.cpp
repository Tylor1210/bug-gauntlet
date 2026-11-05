#include "frame.h"

#include <cstring>

Frame::Frame(int index, int width, int height, bool keyframe)
    : index_(index), width_(width), height_(height), keyframe_(keyframe),
      pixels_(new uint8_t[static_cast<size_t>(width) * height]) {
    std::memset(pixels_, 0, size());
}

Frame::~Frame() {
    delete[] pixels_;
}
