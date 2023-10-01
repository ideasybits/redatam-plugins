//
// Created by jsalvador on 26/9/2022.
//

#ifndef REDATAM_PLUGINS_XLSX_EXPORTER_H
#define REDATAM_PLUGINS_XLSX_EXPORTER_H

#include <redatam/dataset/exporter.h>

const std::string EXPORTER_RDATA = "RDATA";

class TRedRDATAExporter: public TRedExporter {
private:
    std::string exportRaw( TRedOutput* output );
public:
    TRedRDATAExporter();
    virtual ~TRedRDATAExporter();

    virtual bool isBinary() override {return true;}
    virtual Buffer exportToBuffer( TRedOutput* output, bool raw ) override;
};

#endif //REDATAM_PLUGINS_XLSX_EXPORTER_H
