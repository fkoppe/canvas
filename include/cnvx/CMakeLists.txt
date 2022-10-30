target_sources(
    canvas
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/canvas.h
)

add_subdirectory(logger)
add_subdirectory(pch)
add_subdirectory(renderer)
add_subdirectory(window)