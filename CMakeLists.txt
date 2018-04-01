cmake_minimum_required(VERSION 2.8)

project(JXCore)

set(JERRY_LIBC OFF CACHE bool "" FORCE)
set(JERRY_LIBM OFF CACHE bool "" FORCE)
set(JERRY_CMDLINE OFF CACHE bool "" FORCE)
set(FEATURE_PROFILE es2015-subset CACHE bool "" FORCE)
set(FEATURE_ERROR_MESSAGES ON CACHE bool "" FORCE)

set(JXCORE_INCLUDE_I ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/jerryscript/jerry-core/include ${CMAKE_CURRENT_SOURCE_DIR}/jerryscript/jerry-ext/include ${CMAKE_CURRENT_SOURCE_DIR}/jerryscript/jerry-port/include)
set(JXCORE_LIBS_I jxcore jerry-core jerry-ext jerry-port-default m uv)

get_directory_property(HAS_PARENT PARENT_DIRECTORY)
if(HAS_PARENT)
  set(JXCORE_INCLUDE ${JXCORE_INCLUDE_I} PARENT_SCOPE)
  set(JXCORE_LIBS ${JXCORE_LIBS_I} PARENT_SCOPE)
endif()

include_directories(${JXCORE_INCLUDE_I})

add_subdirectory(jerryscript)

file(GLOB SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)
add_library(jxcore STATIC ${SRC})

add_dependencies(jxcore jerry-core jerry-ext jerry-port-default)

install(TARGETS jxcore DESTINATION lib)
install(FILES include/jxcore.h DESTINATION include)
install(DIRECTORY include/jxcore DESTINATION include/jxcore)
