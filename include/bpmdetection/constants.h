#ifndef CONSTANTS_HPP_
#define CONSTANTS_HPP_

#include <cstdint>
// Analysis is done in blocks to avoid dynamic allocation of memory
// depending on the track length. A block size of 4096 frames per block
// seems to do fine. Signal processing during analysis uses the same,
// fixed number of channels like the engine does, usually 2 = stereo.
constexpr uint8_t kAnalysisChannels = 2;
constexpr int kAnalysisFramesPerChunk = 4096;
constexpr int kAnalysisSamplesPerChunk =
        kAnalysisFramesPerChunk * kAnalysisChannels;

// Only analyze the first minute in fast-analysis mode.
constexpr int kFastAnalysisSecondsToAnalyze = 60;

#endif // CONSTANTS_HPP_
