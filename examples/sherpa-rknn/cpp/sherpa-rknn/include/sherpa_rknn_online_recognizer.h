// Copyright (c) 2024 by Rockchip Electronics Co., Ltd. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
#define USE_SHERPA_RKNN_RECOGNITION

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



struct SpeechRecognitionConfig
{
    int32_t expectedSampleRate;
    std::string modelPath;
    std::string vocabPath;
    std::string encoderPath;
    std::string decoderPath;
    std::string joinerPath;
};

class SherpaRknnOnlineRecognizer
{
public:
    explicit SherpaRknnOnlineRecognizer(const SpeechRecognitionConfig &config);
    virtual ~SherpaRknnOnlineRecognizer();

    void DecodeStream(RknnStream* s);
    bool IsReady(RknnStream* s);
    void Reset(RknnStream* s);
    std::string GetResult(RknnStream* s);
    bool IsEndpoint(RknnStream* s);
    std::unique_ptr<RknnStream> CreateStream();

private:
#ifdef USE_SHERPA_NCNN_RECOGNITION
    SherpaNcnnRecognizer* recognizer;
    SherpaNcnnStream* stream;
    SherpaNcnnDisplay* display;
#elif defined(USE_SHERPA_ONNX_RECOGNITION)
    SherpaOnnxOnlineRecognizer *recognizer;
    SherpaOnnxOnlineStream *stream;
    const SherpaOnnxDisplay *display;
#elif defined(USE_SHERPA_RKNN_RECOGNITION)
    rknn_zipformer_context_t rknn_app_ctx;
    VocabEntry vocab[VOCAB_NUM];
    knf::OnlineFbank* fbank;
    sherpa_ncnn::Endpoint endpoint_;

#endif
    const int32_t expectedSampleRate;
};

#endif //_RKNN_DEMO_ZIPFORMER_H_