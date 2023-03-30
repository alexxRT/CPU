add_library(token STATIC ${CMAKE_CURRENT_SOURCE_DIR}/cpu_tokens.cpp)

target_include_directories(token PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
