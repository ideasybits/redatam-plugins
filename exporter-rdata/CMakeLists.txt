cmake_minimum_required(VERSION 3.13)

include_directories( . ${REDENGINE_INCLUDE_PATH})

file(GLOB plugin_EXPORTER_RDATA "*.cpp" "librdata/*.c")

find_package(Iconv)

##-----------------------------------------------------------------------------
## EXPORTER-RDATA
add_library(exporter-rdata SHARED ${plugin_EXPORTER_RDATA} )
if(WIN32)
    target_link_libraries(exporter-rdata
            fmt::fmt
            Iconv::Iconv
            ${REDENGINE_LIB}
            )
elseif(UNIX)
    target_link_libraries(exporter-rdata
            fmt::fmt
            Iconv::Iconv
            dl
            pthread
            ${REDENGINE_LIB}
            )
endif()

install( TARGETS exporter-rdata
        CONFIGURATIONS Debug
        RUNTIME DESTINATION bin/plugins)

install( TARGETS exporter-rdata
        CONFIGURATIONS Release
        RUNTIME DESTINATION bin/plugins)
