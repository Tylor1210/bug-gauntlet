#include <cstdio>
#include <cstdlib>
#include <string>

#include "pipeline.h"

int main(int argc, char** argv) {
    int frame_count = argc > 1 ? std::atoi(argv[1]) : 48;
    int width = 64, height = 36, gop = 12;

    Decoder decoder(width, height, gop);
    FrameStats stats;
    Archive archive;

    for (int i = 0; i < frame_count; ++i) {
        Frame frame = decoder.decode(i);
        stats.add(frame);
        if (frame.is_keyframe()) {
            archive.store(frame);
        }
    }

    std::printf("decoded %ld frames (%ld keyframes), mean luma %.3f\n", stats.frames,
                stats.keyframes, stats.frames ? stats.mean_luma_sum / stats.frames : 0.0);
    std::printf("archive manifest:\n");
    for (const std::string& line : archive.manifest()) {
        std::printf("  %s\n", line.c_str());
    }
    return 0;
}
