//
// Created by jsalvador on 16/7/2022.
//

#ifndef REDENGINE_EXPORTER_JPG_H
#define REDENGINE_EXPORTER_JPG_H

#include <redatam/dataset/exporter.h>

const std::string EXPORTER_JPG = "JPG";

class TRedJpgExporter: public TRedExporter {
private:
    TRedOutput* output;

    int width;
    int height;
    std::string file_name;

    void bar1D();
    void pie1D();
public:
    TRedJpgExporter();
    virtual ~TRedJpgExporter();

    virtual bool isBinary() override {return true;}
    virtual Buffer exportToBuffer( TRedOutput* output, bool raw ) override;
};

#endif //REDENGINE_EXPORTER_JPG_H
