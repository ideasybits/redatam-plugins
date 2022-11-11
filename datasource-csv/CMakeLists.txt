cmake_minimum_required(VERSION 3.13)

include_directories( . ${REDENGINE_INCLUDE_PATH})

file(GLOB plugin_DATASOURCE_CSV
        "*.cpp"
        )

##-----------------------------------------------------------------------------
## DATASET-CSV
add_library(datasource-csv SHARED ${plugin_DATASOURCE_CSV} )
if(WIN32)
    target_link_libraries(datasource-csv
            ${REDENGINE_LIB}
            fmt::fmt
            )
elseif(UNIX)
    target_link_libraries(datasource-csv
            fmt::fmt
            dl
            pthread
            ${REDENGINE_LIB}
            )
endif()

install( TARGETS datasource-csv
        CONFIGURATIONS Debug
        RUNTIME DESTINATION bin/plugins)

install( TARGETS datasource-csv
        CONFIGURATIONS Release
        RUNTIME DESTINATION bin/plugins)
