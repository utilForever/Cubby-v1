set(TINYTHREAD_SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/fast_mutex.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/tinythread.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/tinythread.h"
	PARENT_SCOPE)

source_group("tinythread" FILES ${TINYTHREAD})