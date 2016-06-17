file(GLOB GLM_SRCS *.hpp)
set(GLM_SRCS ${GLM_SRCS} PARENT_SCOPE)

file(GLOB GLM_DETAIL_SRCS detail/*.hpp detail/*.cpp detail/*.inl)
set(GLM_DETAIL_SRCS ${GLM_DETAIL_SRCS} PARENT_SCOPE)

file(GLOB GLM_GTC_SRCS gtc/*.hpp gtc/*.cpp gtc/*.inl)
set(GLM_GTC_SRCS ${GLM_GTC_SRCS} PARENT_SCOPE)

file(GLOB GLM_GTX_SRCS gtx/*.hpp gtx/*.cpp gtx/*.inl)
set(GLM_GTX_SRCS ${GLM_GTX_SRCS} PARENT_SCOPE)

source_group("glm" FILES ${GLM_SRCS})
source_group("glm\\detail" FILES ${GLM_DETAIL_SRCS})
source_group("glm\\gtc" FILES ${GLM_GTC_SRCS})
source_group("glm\\gtx" FILES ${GLM_GTX_SRCS})