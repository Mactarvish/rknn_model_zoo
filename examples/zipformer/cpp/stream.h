#pragma once

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
// #include "zipformer.h"
// #include "audio_utils.h"
#include <iostream>
#include <vector>
#include "wave-reader.h"
#include "alsa/alsa-play.h"
#include "alsa/alsa.h"
#include "alsa/resample.h"

#include <string>
#include "process.h"
#include <iomanip>

#include "utils/logger.h"


class RknnStream
{
public:
    explicit RknnStream();
    virtual ~RknnStream();

    void AcceptWaveform(float sampling_rate, const float *waveform, int32_t n);
    int GetFbankFrames(int frame_index, int segment, float* frames);
    size_t NumFramesReady();
    void InputFinished();
    void Finalize();

private:
    std::unique_ptr<knf::OnlineFbank> fbank;
};