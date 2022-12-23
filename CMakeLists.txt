cmake_minimum_required(VERSION 3.13)

include(FetchContent)

project(redatam-plugins VERSION 1.0.0 DESCRIPTION "BETA1")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

string(TOLOWER ${CMAKE_BUILD_TYPE} LOWERCASE_BUILD_TYPE)
string(TOUPPER ${CMAKE_BUILD_TYPE} UPPERCASE_BUILD_TYPE)

find_package(Boost REQUIRED COMPONENTS filesystem iostreams)
find_package(chartdir CONFIG REQUIRED)
find_library(LIBXLSXWRITER  xlsxwriter)

#---------------------------------------------------
# path to the REDENGINE distribution
add_definitions(-DRED_ENGINE_EXPORTS=1)
set(REDENGINE_PATH "d:/DEV_REDATAM_2019/DISTRIBUTION")
set(REDENGINE_INCLUDE_PATH "${REDENGINE_PATH}/include")
set(REDENGINE_LIB_PATH "${REDENGINE_PATH}/windows-msvc/${LOWERCASE_BUILD_TYPE}/lib")
set(REDENGINE_LIB "${REDENGINE_LIB_PATH}/redengine-1.0.0-rc1.lib")

if(MSVC)
    include(c:/vcpkg/scripts/buildsystems/vcpkg.cmake)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4251 /wd4996 /wd4275")
    set(PLUGINS_PLATFORM "windows-msvc")
elseif(APPLE)
    #    include(/home/jsalvador/vcpkg/scripts/buildsystems/vcpkg.cmake)
    set(PLUGINS_PLATFORM "macos")
elseif(UNIX)
    #    include(/opt/vcpkg/scripts/buildsystems/vcpkg.cmake)
    set(PLUGINS_PLATFORM "linux")
endif()

find_package(fmt CONFIG REQUIRED)
find_library(LIBICONV   iconv)

add_subdirectory(datasource-csv)
add_subdirectory(datasource-spss)
add_subdirectory(exporter-txt)
add_subdirectory(exporter-graphs)
add_subdirectory(exporter-xlsx)
add_subdirectory(exporter-rdata)

set( CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/../DISTRIBUTION/${PLUGINS_PLATFORM}/${LOWERCASE_BUILD_TYPE} )

