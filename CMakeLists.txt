cmake_minimum_required(VERSION 3.13)

project(redatam-plugins VERSION 1.0.0 DESCRIPTION "BETA1")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

string(TOLOWER ${CMAKE_BUILD_TYPE} LOWERCASE_BUILD_TYPE)
string(TOUPPER ${CMAKE_BUILD_TYPE} UPPERCASE_BUILD_TYPE)

#---------------------------------------------------
# path to the REDENGINE distribution
add_definitions(-DRED_ENGINE_EXPORTS=1)
set(REDENGINE_PATH "d:/DEV_REDATAM_2019/DISTRIBUTION")
set(REDENGINE_INCLUDE_PATH "${REDENGINE_PATH}/include")
set(REDENGINE_LIB_PATH "${REDENGINE_PATH}/${CMAKE_CXX_COMPILER_ID}/${LOWERCASE_BUILD_TYPE}/lib")

if(MSVC)
    include(c:/vcpkg/scripts/buildsystems/vcpkg.cmake)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4251 /wd4996 /wd4275")
elseif(APPLE)
    #    include(/home/jsalvador/vcpkg/scripts/buildsystems/vcpkg.cmake)
elseif(LINUX)
    #    include(/opt/vcpkg/scripts/buildsystems/vcpkg.cmake)
endif()

find_package(fmt CONFIG REQUIRED)

add_subdirectory(datasource-csv)
add_subdirectory(datasource-spss)
add_subdirectory(exporter-txt)

set( CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/../DISTRIBUTION/${CMAKE_CXX_COMPILER_ID}/${LOWERCASE_BUILD_TYPE} )
