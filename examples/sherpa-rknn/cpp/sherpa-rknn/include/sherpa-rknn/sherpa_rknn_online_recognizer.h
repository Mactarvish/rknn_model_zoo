#ifndef _RKNN_DEMO_ZIPFORMER_H_
#define _RKNN_DEMO_ZIPFORMER_H_

#include <iostream>
#include <vector>
#include <string>
#include "rknn_api.h"
#include "stream.h"
#include "endpoint.h"

#define BLANK_ID 0
#define UNK_ID 2
// #define USE_SHERPA_RKNN_RECOGNITION

typedef struct
{
    rknn_context rknn_ctx;
    rknn_input_output_num io_num;
    rknn_tensor_attr *input_attrs;
    rknn_tensor_attr *output_attrs;
    rknn_input *inputs;
    rknn_output *outputs;
} rknn_app_context_t;

typedef struct
{
    rknn_app_context_t encoder_context;
    rknn_app_context_t decoder_context;
    rknn_app_context_t joiner_context;
} rknn_zipformer_context_t;

int init_zipformer_model(const char *model_path, rknn_app_context_t *app_ctx);
int release_zipformer_model(rknn_app_context_t *app_ctx);
void build_input_output(rknn_app_context_t *app_ctx);



struct SherpaRknnOnlineRecognizerConfig
{
    int32_t expectedSampleRate;
    std::string vocabPath;
    std::string encoderPath;
    std::string decoderPath;
    std::string joinerPath;
};

class SherpaRknnOnlineRecognizer
{
public:
    explicit SherpaRknnOnlineRecognizer(const SherpaRknnOnlineRecognizerConfig &config);
    virtual ~SherpaRknnOnlineRecognizer();

    void DecodeStream(SherpaRknnOnlineStream* s);
    bool IsReady(SherpaRknnOnlineStream* s);
    void Reset(SherpaRknnOnlineStream* s);
    std::string GetResult(SherpaRknnOnlineStream* s);
    bool IsEndpoint(SherpaRknnOnlineStream* s);
    std::unique_ptr<SherpaRknnOnlineStream> CreateStream();

private:
    rknn_zipformer_context_t rknn_app_ctx;
    VocabEntry vocab[VOCAB_NUM];
    knf::OnlineFbank* fbank;
    sherpa_ncnn::Endpoint endpoint_;
    const int32_t expectedSampleRate;
};

#endif //_RKNN_DEMO_ZIPFORMER_H_