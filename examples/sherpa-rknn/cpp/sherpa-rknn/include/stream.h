#pragma once

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include <vector>

#include <string>
#include "process.h"
#include <iomanip>


struct DecoderResult {
  /// Number of frames we have decoded so far, counted after subsampling
  int32_t frame_offset = 0;

  /// The decoded token IDs so far
  std::vector<int32_t> tokens;

  /// number of trailing blank frames decoded so far
  int32_t num_trailing_blanks = 0;

  std::vector<int32_t> timestamps;

  std::string text;

  // Cache the decoder_out just before endpointing
//   ncnn::Mat decoder_out;

  // used only for modified_beam_search
//   Hypotheses hyps;
};

class RknnStream
{
public:
    explicit RknnStream();
    virtual ~RknnStream();

    void AcceptWaveform(float sampling_rate, const float *waveform, int32_t n);
    DecoderResult& GetResult();
    void SetResult(const DecoderResult& r);
    int GetFbankFrames(int frame_index, int segment, float* frames);
    size_t NumFramesReady();
    void InputFinished();
    void Finalize();
    int32_t &GetNumProcessedFrames();
    void Reset();

private:
    DecoderResult result;
    int32_t num_processed_frames_ = 0;  // before subsampling
    int32_t start_frame_index_ = 0;
    std::unique_ptr<knf::OnlineFbank> fbank;
};