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


set(CMAKE_INSTALL_RPATH "$ORIGIN/lib")

find_package(spdlog REQUIRED)


# install(TARGETS ${PROJECT_NAME} DESTINATION .)
# install(TARGETS sherpa-rknn DESTINATION .)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../model/test.wav DESTINATION ./model)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../model/vocab.txt DESTINATION ./model)
file(GLOB RKNN_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../model/*.rknn")
install(FILES ${RKNN_FILES} DESTINATION model)