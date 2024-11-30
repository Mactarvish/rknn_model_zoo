#include "stream.h"


RknnStream::RknnStream()
{
    knf::FbankOptions fbank_opts;
    fbank_opts.frame_opts.samp_freq = 16000;
    fbank_opts.mel_opts.num_bins = 80;
    fbank_opts.mel_opts.high_freq = -400;
    fbank_opts.frame_opts.dither = 0;
    fbank_opts.frame_opts.snip_edges = false;
    fbank = std::make_unique<knf::OnlineFbank>(fbank_opts);

}


RknnStream::~RknnStream()
{

}


void RknnStream::AcceptWaveform(float sampling_rate, const float *waveform, int32_t n)
{
    return fbank->AcceptWaveform(sampling_rate, waveform, n);
}


int RknnStream::GetFbankFrames(int frame_index, int segment, float* frames)
{
    if (frame_index + segment > fbank->NumFramesReady())
    {
        return -1;
    }

    for (int i = 0; i < segment; ++i)
    {
        const float *frame = fbank->GetFrame(i + frame_index);
        memcpy(frames + i * N_MELS, frame, N_MELS * sizeof(float));
    }

    return 0;
}

size_t RknnStream::NumFramesReady()
{
    return fbank->NumFramesReady();
}


void RknnStream::Finalize()
{

}


void RknnStream::InputFinished()
{
    return fbank->InputFinished();
}