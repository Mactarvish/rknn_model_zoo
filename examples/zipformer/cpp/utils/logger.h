#pragma once
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"

#ifdef ANDROID
#include "spdlog/sinks/android_sink.h"
#endif


static inline std::shared_ptr<spdlog::logger> GetLogger(const std::string& tag = "")
{
    auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    stdout_sink->set_level(spdlog::level::info);

#if defined(WIN32)
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("log.txt", false);
    file_sink->set_level(spdlog::level::debug);
#elif defined(ANDROID)
    auto android_sink = std::make_shared<spdlog::sinks::android_sink_mt>(tag);
    stdout_sink->set_level(spdlog::level::debug);
#elif defined(__aarch64__) || defined(__linux__)
    // aarch64 can use file_sink like Linux
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("log_aarch64.txt", false);
    file_sink->set_level(spdlog::level::debug);
#endif

    auto logger = std::make_shared<spdlog::logger>(tag, spdlog::sinks_init_list{
         stdout_sink,
#if defined(WIN32) || defined(__aarch64__) || defined(__linux__)
         file_sink,
#endif
#ifdef ANDROID
         android_sink,
#endif
    });

    logger->set_level(spdlog::level::debug);
    logger->flush_on(spdlog::level::debug);
    return logger;
}
