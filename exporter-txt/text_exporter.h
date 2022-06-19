//
// Created by jsalvador on 17/6/2022.
//

#ifndef REDENGINE_TEXT_EXPORTER_H
#define REDENGINE_TEXT_EXPORTER_H

#include <redatam/dataset/exporter.h>

const std::string EXPORTER_TEXT = "TXT";

class TRedTextExporter: public TRedExporter {
private:
    std::string exportRaw( TRedOutput* output );
public:
    TRedTextExporter();
    virtual ~TRedTextExporter();

    virtual bool isBinary() override {return false;}
    virtual Buffer exportToBuffer( TRedOutput* output, bool raw ) override;
};

#endif //REDENGINE_TEXT_EXPORTER_H
