cmake_minimum_required(VERSION 3.10)
project(sherpa-rknn)


# set(rknpu_zipformer_file rknpu2/zipformer.cc)
# if(TARGET_SOC STREQUAL "rk1808" OR TARGET_SOC STREQUAL "rv1109" OR TARGET_SOC STREQUAL "rv1126")
#     set(rknpu_zipformer_file rknpu1/zipformer.cc)
# endif()


find_package(spdlog REQUIRED)

file(GLOB SRCS ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cc)

set(CMAKE_INSTALL_RPATH "$ORIGIN")
add_library(${PROJECT_NAME} SHARED
    ${SRCS}
)


target_link_libraries(${PROJECT_NAME}
    fileutils
    ${LIBRKNNRT}
    ${LIBKALDI_NATIVE_FBANK}
    asound
    spdlog::spdlog_header_only
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
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${LIBRKNNRT_INCLUDES}
    ${LIBKALDI_NATIVE_FBANK_INCLUDES}
    ${LIBTIMER_INCLUDES}
)
