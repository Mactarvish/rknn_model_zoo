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
#include <iostream>
#include <vector>

#include "zipformer.h"
#include "wave-reader.h"
#include "alsa/alsa-play.h"
#include "alsa/alsa.h"
#include "alsa/resample.h"

#include <string>
#include "process.h"
#include <iomanip>

#include "../utils/logger.h"

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

    SpeechRecognizer recognizer(SpeechRecognitionConfig{
        .expectedSampleRate = SAMPLE_RATE,
        .modelPath = "",
        .vocabPath = VOCAB_PATH,
        .encoderPath = encoder_path,
        .decoderPath = decoder_path,
        .joinerPath = joiner_path});

    auto stream = recognizer.CreateStream();

    // 执行推理
#define N 3200 // 0.2 s. Sample rate is fixed to 16 kHz

    sherpa_onnx::Alsa alsa(device_name);
    std::string lastText;
    int32_t segment_index = 0;
    while (true)
    {
        int32_t chunk = 0.1 * alsa.GetActualSampleRate();
        const std::vector<float> samples = alsa.Read(chunk);

        stream->AcceptWaveform(SAMPLE_RATE, samples.data(), samples.size());
        while (recognizer.IsReady(stream.get()))
        {
            recognizer.DecodeStream(stream.get());
        }

        bool is_endpoint = recognizer.IsEndpoint(stream.get());

        if (is_endpoint)
        {
            stream->Finalize();
        }
        auto text = recognizer.GetResult(stream.get());

        if (!text.empty() && lastText != text)
        {
            lastText = text;

            std::transform(text.begin(), text.end(), text.begin(),
                           [](auto c)
                           { return std::tolower(c); });
            // logger->info("{} {}", segment_index, text);
            // display.Print(segment_index, text);
        }

        if (is_endpoint)
        {
            if (!text.empty())
            {
                ++segment_index;
                logger->info("{} {}", segment_index, text);
            }

            recognizer.Reset(stream.get());
        }
    }

    return 0;
}