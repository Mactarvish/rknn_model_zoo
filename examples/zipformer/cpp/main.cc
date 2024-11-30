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

/*-------------------------------------------
                Includes
-------------------------------------------*/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "zipformer.h"
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

static auto logger = GetLogger("speech_interaction_demo");

/*-------------------------------------------
                  Main Function
-------------------------------------------*/

int main(int argc, char **argv)
{
    if (argc != 5)
    {
        printf("%s <encoder_path> <decoder_path> <joiner_path> <audio_path>\n", argv[0]);
        // return -1;
    }

    const char *encoder_path = argv[1];
    const char *decoder_path = argv[2];
    const char *joiner_path = argv[3];
    const char *audio_path = argv[4];

    const char *device_name = argv[5];

    const char *wav_filename = audio_path;
    FILE *fp = fopen(wav_filename, "rb");
    if (!fp)
    {
        fprintf(stderr, "Failed to open %s\n", wav_filename);
        return -1;
    }

    // Assume the wave header occupies 44 bytes. 跳过元信息
    fseek(fp, 44, SEEK_SET);

    int ret;
    TIMER timer;
    float infer_time = 0.0;
    float audio_length = 0.0;
    float rtf = 0.0;
    int frame_shift_ms = 10;
    int subsampling_factor = 4;
    float frame_shift_s = frame_shift_ms / 1000.0 * subsampling_factor;
    std::vector<std::string> recognized_text;
    std::vector<float> timestamp;
    rknn_zipformer_context_t rknn_app_ctx;
    VocabEntry vocab[VOCAB_NUM];
    audio_buffer_t audio;
    memset(&rknn_app_ctx, 0, sizeof(rknn_zipformer_context_t));
    memset(vocab, 0, sizeof(vocab));
    memset(&audio, 0, sizeof(audio_buffer_t));

    bool isOk;
    auto ss = sherpa_onnx::ReadWave(std::string(audio_path), &audio.sample_rate, &isOk);
    audio.data = ss.data();
    audio.num_channels = 1;
    audio.num_frames = ss.size();

    timer.tik();
    // ret = read_audio(audio_path, &audio);
    if (ret != 0)
    {
        printf("read audio fail! ret=%d audio_path=%s\n", ret, audio_path);
        // goto out;
    }

    if (audio.num_channels == 2)
    {
        // ret = convert_channels(&audio);
        if (ret != 0)
        {
            printf("convert channels fail! ret=%d\n", ret, audio_path);
            // goto out;
        }
    }

    if (audio.sample_rate != SAMPLE_RATE)
    {
        // ret = resample_audio(&audio, audio.sample_rate, SAMPLE_RATE);
        if (ret != 0)
        {
            printf("resample audio fail! ret=%d\n", ret, audio_path);
            // goto out;
        }
    }

    ret = read_vocab(VOCAB_PATH, vocab);
    if (ret != 0)
    {
        printf("read vocab fail! ret=%d vocab_path=%s\n", ret, VOCAB_PATH);
        // goto out;
    }
    timer.tok();
    timer.print_time("read_audio & convert_channels & resample_audio & read_vocab");

    timer.tik();
    ret = init_zipformer_model(encoder_path, &rknn_app_ctx.encoder_context);
    if (ret != 0)
    {
        printf("init_zipformer_model fail! ret=%d encoder_path=%s\n", ret, encoder_path);
        // goto out;
    }
    build_input_output(&rknn_app_ctx.encoder_context);
    timer.tok();
    timer.print_time("init_zipformer_encoder_model");

    timer.tik();
    ret = init_zipformer_model(decoder_path, &rknn_app_ctx.decoder_context);
    if (ret != 0)
    {
        printf("init_zipformer_model fail! ret=%d decoder_path=%s\n", ret, decoder_path);
        // goto out;
    }
    build_input_output(&rknn_app_ctx.decoder_context);
    timer.tok();
    timer.print_time("init_zipformer_decoder_model");

    timer.tik();
    ret = init_zipformer_model(joiner_path, &rknn_app_ctx.joiner_context);
    if (ret != 0)
    {
        printf("init_zipformer_model fail! ret=%d oiner_path=%s\n", ret, joiner_path);
        // goto out;
    }
    build_input_output(&rknn_app_ctx.joiner_context);
    timer.tok();
    timer.print_time("init_zipformer_joiner_model");

    timer.tik();

    RknnStream stream;
    SpeechRecognizer recognizer(SpeechRecognitionConfig{
        .expectedSampleRate = 16000,
        .modelPath = "",
        .vocabPath = VOCAB_PATH,
        .encoderPath = encoder_path,
        .decoderPath = decoder_path,
        .joinerPath = joiner_path});

    // 执行推理
#define N 3200  // 0.2 s. Sample rate is fixed to 16 kHz

    // int16_t buffer[N];
    // float samples[N];

    // while (!feof(fp))
    // {
    //     size_t n = fread((void *)buffer, sizeof(int16_t), N, fp);
    //     if (n > 0)
    //     {
    //         for (size_t i = 0; i != n; ++i)
    //         {
    //             samples[i] = buffer[i] / 32768.;
    //         }
    //         audio_buffer_t f = {samples, N, 1, 16000};
    //         ret = inference_zipformer_model(&rknn_app_ctx, f, vocab, recognized_text, timestamp, audio_length);
    //         std::cout << recognized_text.size() << std::endl;
    //         for (const auto &str : recognized_text)
    //         {
    //             std::cout << str;
    //         }
    //         // AcceptWaveform(s, 16000, samples, n);
    //         // while (IsReady(recognizer, s))
    //         // {
    //         //     Decode(recognizer, s);
    //         // }

    //         // SherpaNcnnResult *r = GetResult(recognizer, s);
    //         // if (strlen(r->text))
    //         // {
    //         //     SherpaNcnnPrint(display, segment_id, r->text);
    //         // }
    //         // DestroyResult(r);
    //     }
    // }
    // fclose(fp);
    // std::cout << std::endl;
    logger->info("执行Recognizer推理");
    std::string text;
    recognizer.Recognize(audio, &stream, text);
    std::cout << text << std::endl;
    logger->info("执行demo推理");
    ret = inference_zipformer_model(&rknn_app_ctx, audio, vocab, recognized_text, timestamp, audio_length);
    for (const auto &str : recognized_text)
    {
        std::cout << str;
    }
    if (ret != 0)
    {
        printf("inference_zipformer_model fail! ret=%d\n", ret);
        // goto out;
    }
    timer.tok();
    timer.print_time("inference_zipformer_model");
    exit(0);

    // out:

    if (audio.data)
    {
        // free(audio.data);
    }

    for (int i = 0; i < VOCAB_NUM; i++)
    {
        if (vocab[i].token)
        {
            free(vocab[i].token);
            vocab[i].token = NULL;
        }
    }

    ret = release_zipformer_model(&rknn_app_ctx.encoder_context);
    if (ret != 0)
    {
        printf("release_zipformer_model encoder_context fail! ret=%d\n", ret);
    }

    ret = release_zipformer_model(&rknn_app_ctx.decoder_context);
    if (ret != 0)
    {
        printf("release_zipformer_model decoder_context fail! ret=%d\n", ret);
    }

    ret = release_zipformer_model(&rknn_app_ctx.joiner_context);
    if (ret != 0)
    {
        printf("release_zipformer_model joiner_context fail! ret=%d\n", ret);
    }
    return 0;
}
