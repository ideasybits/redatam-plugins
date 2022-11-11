cmake_minimum_required(VERSION 3.13)

include_directories( . ${REDENGINE_INCLUDE_PATH})

file(GLOB plugin_EXPORTER_TEXT
        "*.cpp"
        "*.c"
        )

##-----------------------------------------------------------------------------
## EXPORTER-TEXT
add_library(exporter-text SHARED ${plugin_EXPORTER_TEXT} )
if(WIN32)
    target_link_libraries(exporter-text
            fmt::fmt
            ${REDENGINE_LIB}
            )
elseif(UNIX)
    target_link_libraries(exporter-text
            fmt::fmt
            dl
            pthread
            ${REDENGINE_LIB}
            )
endif()

install( TARGETS exporter-text
        CONFIGURATIONS Debug
        RUNTIME DESTINATION bin/plugins)

install( TARGETS exporter-text
        CONFIGURATIONS Release
        RUNTIME DESTINATION bin/plugins)
