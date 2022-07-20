cmake_minimum_required(VERSION 3.13)

include_directories( . ${REDENGINE_INCLUDE_PATH})

file(GLOB plugin_EXPORTER_GRAPH
        "*.cpp"
        )

##-----------------------------------------------------------------------------
## EXPORTER-TEXT
add_library(exporter-graphs SHARED ${plugin_EXPORTER_GRAPH} )
if(WIN32)
    target_link_libraries(exporter-graphs
            fmt::fmt
            ${REDENGINE_LIB_PATH}/redengine.lib
			Boost::boost Boost::iostreams Boost::filesystem
            chartdir
            )
elseif(UNIX)
    target_link_libraries(exporter-graphs
            fmt::fmt
            dl
            pthread
            ${REDENGINE_LIB_PATH}/redengine.lib
			chartdir
            )
endif()

install( TARGETS exporter-graphs
        CONFIGURATIONS Debug Release
        RUNTIME DESTINATION bin/plugins)
