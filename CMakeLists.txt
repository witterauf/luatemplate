cmake_minimum_required(VERSION 3.12)
project(LuaTemplate VERSION 1.0 LANGUAGES CXX)

set(CMAKE_DEBUG_POSTFIX d)
set(LUATEMPLATE_CMAKE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH ${LUATEMPLATE_CMAKE_PATH})

enable_testing()

###[ sources ]#####################################################################################

set(LUATEMPLATE_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

set(LUATEMPLATE_SOURCES
  ${LUATEMPLATE_INCLUDE_DIR}/luatemplate/Template.h
  ${LUATEMPLATE_INCLUDE_DIR}/luatemplate/Parser.h
  ${LUATEMPLATE_INCLUDE_DIR}/luatemplate/Environment.h
  ${LUATEMPLATE_INCLUDE_DIR}/luatemplate/Renderer.h
  ${LUATEMPLATE_INCLUDE_DIR}/luatemplate/Contracts.h
  ${LUATEMPLATE_INCLUDE_DIR}/luatemplate/LuaTemplate.h
)

add_custom_target(LuaTemplateSources
  SOURCES
    ${LUATEMPLATE_SOURCES}
)

###[ library ]#####################################################################################

add_library(LuaTemplate INTERFACE)
target_link_libraries(LuaTemplate
  INTERFACE
    $<$<CXX_COMPILER_ID:GNU>:stdc++fs>
    $<$<CXX_COMPILER_ID:Clang>:stdc++fs>
)
target_include_directories(LuaTemplate
  INTERFACE
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${LUATEMPLATE_INCLUDE_DIR}>
    $<BUILD_INTERFACE:${LUATEMPLATE_LUA_DIR}>
)
target_compile_features(LuaTemplate INTERFACE cxx_std_17)
add_library(LuaTemplate::LuaTemplate ALIAS LuaTemplate)

###[ installation ]################################################################################

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  LuaTemplateConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion
)
configure_file(${LUATEMPLATE_CMAKE_PATH}/LuaTemplateConfig.cmake.in LuaTemplateConfig.cmake @ONLY)

install(TARGETS LuaTemplate EXPORT LuaTemplate
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)
install(DIRECTORY "${LUATEMPLATE_INCLUDE_DIR}/luatemplate" DESTINATION include)
install(EXPORT LuaTemplate NAMESPACE LuaTemplate:: DESTINATION lib/cmake/luatemplate)
install(
  FILES
    ${CMAKE_CURRENT_BINARY_DIR}/LuaTemplateConfigVersion.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/LuaTemplateConfig.cmake
  DESTINATION
    lib/cmake/luatemplate
)
