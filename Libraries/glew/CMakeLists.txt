set(GLEW_SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/glew.c"
	PARENT_SCOPE)

set(GLEW_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/GL/glew.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/GL/glxew.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/GL/wglew.h"
	PARENT_SCOPE)
	
source_group("glew\\src" FILES ${GLEW_SRCS})
source_group("glew\\include\\GL" FILES ${GLEW_HEADERS})