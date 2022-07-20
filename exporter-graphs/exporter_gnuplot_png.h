//
// Created by jsalvador on 13/7/2022.
//

#ifndef REDENGINE_EXPORTER_GNUPLOT_PNG_H
#define REDENGINE_EXPORTER_GNUPLOT_PNG_H

#include <redatam/dataset/exporter.h>

const std::string EXPORTER_PNG = "PNG";

class TRedGnuplotPngExporter: public TRedExporter {
//private:
//    std::string exportRaw( TRedOutput* output );
public:
    TRedGnuplotPngExporter();
    virtual ~TRedGnuplotPngExporter();

    virtual bool isBinary() override {return true;}
    virtual Buffer exportToBuffer( TRedOutput* output, bool raw ) override;
};

#endif //REDENGINE_EXPORTER_GNUPLOT_PNG_H
