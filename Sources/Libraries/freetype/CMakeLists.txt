set(FREETYPE_SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/freetypefont.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/freetypefont.cpp"
	PARENT_SCOPE)

source_group("freetype" FILES ${FREETYPE_SRCS})