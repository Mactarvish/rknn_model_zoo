cmake_minimum_required(VERSION 3.10)
project(sherpa-rknn-demo)


find_package(spdlog REQUIRED)

file(GLOB SRCS ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)

set(CMAKE_INSTALL_RPATH "$ORIGIN/lib")
add_executable(${PROJECT_NAME}
    main.cc
    wave-reader.cc
    alsa/alsa.cc
    alsa/resample.cc
)

target_link_libraries(${PROJECT_NAME}
    fileutils
    ${LIBRKNNRT}
    ${LIBKALDI_NATIVE_FBANK}
    asound
    spdlog::spdlog_header_only
    ${CMAKE_BINARY_DIR}/sherpa-rknn/libsherpa-rknn.so
)


if (CMAKE_SYSTEM_NAME STREQUAL "Android")
    target_link_libraries(${PROJECT_NAME}
    log
)
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    target_link_libraries(${PROJECT_NAME} Threads::Threads)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
    ${sherpa_rknn_include_dir}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${LIBRKNNRT_INCLUDES}
    ${LIBKALDI_NATIVE_FBANK_INCLUDES}
    ${LIBTIMER_INCLUDES}
)
