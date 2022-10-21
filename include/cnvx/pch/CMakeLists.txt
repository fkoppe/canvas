target_sources(
    canvas
	PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/cnvxmodule.h
)

target_precompile_headers(
    canvas
	PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/cnvxmodule.h
)