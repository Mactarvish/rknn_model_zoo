if (${CMAKE_BUILD_TYPE} AND ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(d "d")
else()
    set(d "")
endif()

if (NOT (TARGET_SOC STREQUAL "rk3588"))
    message(FATAL_ERROR "sherpa_rknn : Unsupported soc")
endif()

if(CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
    set(BUILD_FOLDER_NAME "${CMAKE_SYSTEM_PROCESSOR}-${TARGET_SOC}-${CMAKE_BUILD_TYPE}")
else ()
    message(FATAL_ERROR "sherpa_rknn : Unsupported platform")
endif()

# 设置包含目录
set(SHERPA_RKNN_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/../${BUILD_FOLDER_NAME}/include")

# 设置库目录
set(SHERPA_RKNN_LIBRARY_DIR "${CMAKE_CURRENT_LIST_DIR}/../${BUILD_FOLDER_NAME}/lib")

# 设置库
set(SHERPA_RKNN_LIBRARIES "${SHERPA_RKNN_LIBRARY_DIR}/libsherpa-rknn${d}.so")

# 导出包含目录
set(SHERPA_RKNN_INCLUDE_DIRS ${SHERPA_RKNN_INCLUDE_DIR})

# 导出库
set(SHERPA_RKNN_LIBS ${SHERPA_RKNN_LIBRARIES})

# 为用户提供包含和库的变量
#include_directories(${SHERPA_RKNN_INCLUDE_DIRS})
#link_libraries(${SHERPA_RKNN_LIBS})

add_library(sherpa_rknn SHARED IMPORTED)
set_target_properties(sherpa_rknn PROPERTIES
        IMPORTED_LOCATION             ${SHERPA_RKNN_LIBS}
        INTERFACE_INCLUDE_DIRECTORIES ${SHERPA_RKNN_INCLUDE_DIRS}
)
