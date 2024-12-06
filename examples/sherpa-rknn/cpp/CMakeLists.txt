cmake_minimum_required(VERSION 3.10)
project(sherpa-rknn-alg)

add_compile_definitions(SHERPA_ONNX_ENABLE_ALSA)

if (ENABLE_ASAN)
	message(STATUS "BUILD WITH ADDRESS SANITIZER")
	set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
	set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
	set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
endif ()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../../3rdparty/ 3rdparty.out)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../../utils/ utils.out)

set(sherpa_rknn_include_dir ${CMAKE_SOURCE_DIR}/sherpa-rknn/include)
message(${CMAKE_BINARY_DIR})

add_subdirectory(sherpa-rknn)
add_subdirectory(demo)
add_dependencies(sherpa-rknn-demo sherpa-rknn)


find_package(spdlog REQUIRED)


set(CMAKE_INSTALL_PREFIX install)
set(BUILD_FOLDER_NAME ${CMAKE_SYSTEM_PROCESSOR}-${TARGET_SOC}-${CMAKE_BUILD_TYPE})
install(TARGETS sherpa-rknn-demo DESTINATION ./${BUILD_FOLDER_NAME})
install(TARGETS sherpa-rknn DESTINATION ./${BUILD_FOLDER_NAME}/lib)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/sherpa-rknn/include DESTINATION ./${BUILD_FOLDER_NAME}) # 这个路径是相对于${CMAKE_INSTALL_PREFIX}的
install(DIRECTORY ${CMAKE_SOURCE_DIR}/sherpa-rknn/cmake DESTINATION .)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../model/test.wav DESTINATION ./model)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../model/vocab.txt DESTINATION ./model)
file(GLOB RKNN_MODEL_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../model/*.rknn")
install(FILES ${RKNN_MODEL_FILES} DESTINATION model)

install(PROGRAMS ${LIBRKNNRT} DESTINATION ./${BUILD_FOLDER_NAME}/lib)
install(PROGRAMS ${LIBKALDI_NATIVE_FBANK} DESTINATION ./${BUILD_FOLDER_NAME}/lib)
