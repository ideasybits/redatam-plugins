//
// Created by jsalvador on 16/7/2022.
//

#include "exporter_gnuplot_png.h"
#include "exporter_jpg.h"

DEFINE_PLUGIN_EXPORTER( EXPORTER_PNG,
        TRedGnuplotPngExporter, "GNUPLOT PNG Redatam exporter (.png files)" );

DEFINE_PLUGIN_EXPORTER( EXPORTER_JPG,
                        TRedJpgExporter, "JPG Redatam exporter (.jpg files)" );

REDATAM_PLUNGIN_EXPORTER_FN_NAME() {
    REGISTER_EXPORTER( EXPORTER_PNG );
    REGISTER_EXPORTER( EXPORTER_JPG );
}
