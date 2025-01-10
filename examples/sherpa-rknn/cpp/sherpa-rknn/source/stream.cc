#include "sherpa-rknn/stream.h"


SherpaRknnOnlineStream::SherpaRknnOnlineStream()
{
    knf::FbankOptions fbank_opts;
    fbank_opts.frame_opts.samp_freq = 16000;
    fbank_opts.mel_opts.num_bins = 80;
    fbank_opts.mel_opts.high_freq = -400;
    fbank_opts.frame_opts.dither = 0;
    fbank_opts.frame_opts.snip_edges = false;
    fbank = std::make_unique<knf::OnlineFbank>(fbank_opts);

}


SherpaRknnOnlineStream::~SherpaRknnOnlineStream()
{

}


void SherpaRknnOnlineStream::AcceptWaveform(float sampling_rate, const float *waveform, int32_t n)
{
    return fbank->AcceptWaveform(sampling_rate, waveform, n);
}


int SherpaRknnOnlineStream::GetFbankFrames(int frame_index, int segment, float* frames)
{
    if (frame_index + segment > fbank->NumFramesReady())
    {
        return -1;
    }

    for (int i = 0; i < segment; ++i)
    {
        const float *frame = fbank->GetFrame(i + frame_index + start_frame_index_);
        memcpy(frames + i * N_MELS, frame, N_MELS * sizeof(float));
    }

    return 0;
}

size_t SherpaRknnOnlineStream::NumFramesReady()
{
    return fbank->NumFramesReady() - start_frame_index_;
}

int32_t& SherpaRknnOnlineStream::GetNumProcessedFrames()
{
    return num_processed_frames_;
}

void SherpaRknnOnlineStream::SetResult(const DecoderResult& r)
{
    int32_t offset = r.frame_offset;
    result = r;
    result.frame_offset = offset;
}

DecoderResult& SherpaRknnOnlineStream::GetResult()
{
    return result;
}

void SherpaRknnOnlineStream::Finalize()
{
    
}


void SherpaRknnOnlineStream::InputFinished()
{
    return fbank->InputFinished();
}

void SherpaRknnOnlineStream::Reset()
{
    start_frame_index_ += num_processed_frames_;
    num_processed_frames_ = 0;
}