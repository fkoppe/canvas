cmake_minimum_required(VERSION 3.22 FATAL_ERROR)
project(cnvx LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(canvas)

target_link_libraries(
    canvas
    PRIVATE
    spore
    glfw
)

target_compile_definitions(
    canvas
    PUBLIC
    $<$<CONFIG:Debug>:"___CNVX_DEBUG">
)
target_compile_definitions(
    canvas
    PUBLIC
    $<$<CONFIG:RelWithDebInfo>:
    "___CNVX_DEBUG"
    "___CNVX_RELWITHDEBINFO"
    >
)
target_compile_definitions(
    canvas
    PUBLIC
    $<$<CONFIG:Release>:
    "___CNVX_RELEASE"
    >
)
target_compile_definitions(
    canvas
    PUBLIC
    $<$<CONFIG:MinSizeRel>:
    "___CNVX_RELEASE"
    "___CNVX_MINSIZEREL"
    >
)

find_package(Vulkan REQUIRED)
if(Vulkan_FOUND)
	message(STATUS "VulkanSDK found")
    target_link_libraries(
        canvas
        PRIVATE
        Vulkan::Vulkan
    )
    target_include_directories(
        canvas
        PRIVATE
        ${VULKAN_INCLUDE_DIRS}
    )
else()
message(WARNING "VulkanSDK not found")
endif()

add_subdirectory(include)
add_subdirectory(library)
add_subdirectory(source)