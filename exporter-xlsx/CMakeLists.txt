cmake_minimum_required(VERSION 3.13)

include_directories( . ${REDENGINE_INCLUDE_PATH})

file(GLOB plugin_EXPORTER_XLSX "*.cpp")

##-----------------------------------------------------------------------------
## EXPORTER-XLSX
add_library(exporter-xlsx SHARED ${plugin_EXPORTER_XLSX} )
if(WIN32)
    target_link_libraries(exporter-xlsx
            fmt::fmt
            ${LIBXLSXWRITER}
            ${REDENGINE_LIB}
            )
elseif(UNIX)
    target_link_libraries(exporter-xlsx
            fmt::fmt
            dl
            pthread
            ${LIBXLSXWRITER}
            ${REDENGINE_LIB}
            )
endif()

install( TARGETS exporter-xlsx
        CONFIGURATIONS Debug
        RUNTIME DESTINATION bin/plugins)

install( TARGETS exporter-xlsx
        CONFIGURATIONS Release
        RUNTIME DESTINATION bin/plugins)
