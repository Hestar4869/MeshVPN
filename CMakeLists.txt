cmake_minimum_required(VERSION 3.24)
project(MeshVPN)
set(CMAKE_CXX_STANDARD 20)
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

# 设置项目输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build)

# 设置boost库搜索路径
set(BOOST_ROOT "E:\\boost_1_78_0")

# Boost库使用静态链接
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.78.0 REQUIRED COMPONENTS log)
include_directories(${Boost_INCLUDE_DIRS})

# 判断当前操作系统
if (WIN32)
    # Windows平台下使用Windows wdk和cl编译器
#    set(CMAKE_C_COMPILER "cl")
#    set(CMAKE_CXX_COMPILER "cl")

    # 设置最低 Windows 版本
    add_compile_definitions(_WIN32_WINNT=0x0600)

    # 包含wintun头文件
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/wintun)

    # 构建tun库
    add_library(tun OBJECT tun.cpp tun.h)
    target_link_libraries(tun PUBLIC Iphlpapi.lib ntdll.lib ws2_32.lib Boost::log)

    # 添加可执行文件
    add_executable(${PROJECT_NAME} main.cpp)

    # 根据不同架构选择不同的wintun.dll
    if (CMAKE_SIZEOF_VOID_P EQUAL 8) # 64-bit 架构
        set(WINTUN_DLL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/external/wintun/amd64/wintun.dll)
    elseif (CMAKE_SIZEOF_VOID_P EQUAL 4) # 32-bit 架构
        set(WINTUN_DLL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/external/wintun/x86/wintun.dll)
    elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "arm") # ARM 架构
        set(WINTUN_DLL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/external/wintun/arm/wintun.dll)
    elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64") # ARM64 架构
        set(WINTUN_DLL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/external/wintun/arm64/wintun.dll)
    else ()
        message(FATAL_ERROR "Unsupported architecture")
    endif ()
    # 将wintun.dll拷贝到构建目录
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${WINTUN_DLL_PATH}
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )

    # 链接
    target_link_libraries(${PROJECT_NAME}  PUBLIC tun )
#    target_compile_definitions(${PROJECT_NAME} PRIVATE -D_WIN32_WINNT=0x0600)
#    target_link_libraries(${PROJECT_NAME}  PUBLIC Boost::log )

    add_subdirectory(external/wintun)
elseif (UNIX)
    # Linux平台下使用gcc编译器
    set(CMAKE_C_COMPILER "gcc")
    set(CMAKE_CXX_COMPILER "g++")

    # 添加Linux平台相关的设置，例如编译选项等
    # ...

else()
    message(FATAL_ERROR "Unsupported platform")
endif()








