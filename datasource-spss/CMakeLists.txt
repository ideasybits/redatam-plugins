cmake_minimum_required(VERSION 3.13)

include_directories( . ${REDENGINE_INCLUDE_PATH})

file(GLOB plugin_DATASOURCE_SPSS
        "*.cpp"
        "*.c"
        )

##-----------------------------------------------------------------------------
## DATASET-SPSS
add_library(datasource-spss SHARED ${plugin_DATASOURCE_SPSS} )
if(WIN32)
    if(MINGW)
        set(READSTAT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/readstat/gnu/libreadstat.dll.a")
    elseif(MSVC)
        set(READSTAT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/readstat/msvc/ReadStat.lib")
    endif()

    message(STATUS "CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")
    message(STATUS "READSTAT_PATH: ${READSTAT_PATH}")

    target_link_libraries(datasource-spss
            fmt::fmt
            ${LIBICONV}
            ${REDENGINE_LIB}
            ${READSTAT_PATH}
            )
elseif(UNIX)
    target_link_libraries(datasource-spss
            fmt::fmt
            #            ${LIBICONV}
            dl
            pthread
            ${REDENGINE_LIB}
            )
endif()

install( TARGETS datasource-spss
        CONFIGURATIONS Debug
        RUNTIME DESTINATION bin/plugins)

install( TARGETS datasource-spss
        CONFIGURATIONS Release
        RUNTIME DESTINATION bin/plugins)
