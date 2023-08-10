file(GLOB_RECURSE SRC_UI ${CMAKE_SOURCE_DIR} "ui/*.c")

idf_component_register(SRCS "httpGetClient.c" "screen.c" ${SRC_UI}
			INCLUDE_DIRS "." "ui")


