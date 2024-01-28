# CMakeLists.txt in external/wintun_


# 设置构建类型为 Debug
set(CMAKE_VERBOSE_MAKEFILE ON)

# 添加示例程序
add_executable(wintun_example example.c)
target_link_libraries(wintun_example Iphlpapi.lib ntdll.lib ws2_32.lib)

# 设置输出目录
set_target_properties(wintun_example PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/wintun)

# 根据不同架构选择不同的wintun.dll
if (CMAKE_SIZEOF_VOID_P EQUAL 8) # 64-bit 架构
    set(WINTUN_DLL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/amd64/wintun.dll)
elseif (CMAKE_SIZEOF_VOID_P EQUAL 4) # 32-bit 架构
    set(WINTUN_DLL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/x86/wintun.dll)
elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "arm") # ARM 架构
    set(WINTUN_DLL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/arm/wintun.dll)
elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64") # ARM64 架构
    set(WINTUN_DLL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/arm64/wintun.dll)
else ()
    message(FATAL_ERROR "Unsupported architecture")
endif ()

# 将wintun.dll拷贝到构建目录
add_custom_command(TARGET wintun_example POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${WINTUN_DLL_PATH}
        $<TARGET_FILE_DIR:wintun_example>
        )

