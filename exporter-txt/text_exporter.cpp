//
// Created by jsalvador on 17/6/2022.
//

#include "text_exporter.h"

#include <fstream>
#include <redatam/utils.h>
#include <fmt/core.h>

#include "fort.hpp"

class TApiExporterTxtPlugin : public TRedExporterPluginApi {
public:
    TApiExporterTxtPlugin() = default;

    virtual std::shared_ptr<TRedExporter> createExporter() {
        return std::make_shared<TRedTextExporter>();
    }

    std::string type() const override {
        return std::string(EXPORTER_TEXT);
    }

    std::string description() const override {
        return std::string("TEXT Redatam exporter (.txt files)");
    }
};

REDATAM_PLUNGIN_EXPORTER_FN_NAME() {
//    REGISTER_EXPORTER( EXPORTER_TEXT );
    red::registerExporterType( new TApiExporterTxtPlugin() );
}

//---------------------------------------------------------------------------------
class TRedConsoleTablePrinter : public TRedOutputPrinter {
private:
    TRedOutput* out;
    size_t rows, cols;

    fort::utf8_table tb1;
public:
    TRedConsoleTablePrinter( TRedOutput* out ) {
        this->out = out;
    }

    virtual void printValue( size_t row, size_t col, red::variant value ) override {
        if( std::holds_alternative<int64_t>(value)) {
            tb1[row][col] = fmt::format("{}",std::get<int64_t>(value));
        }
        else if( std::holds_alternative<double>(value)) {
            tb1[row][col] = fmt::format("{}",std::get<double>(value));
        }
        else if( std::holds_alternative<std::string>(value)) {
            tb1[row][col] = std::get<std::string>(value);
        }
    }

    virtual void endTitle() override{
        tb1 << fort::endr;
        tb1 << fort::separator;
    }

    virtual void setSize( size_t rows, size_t cols ) override {
        this->rows = rows;
        this->cols = cols;
    }

    virtual void end( ) override {

        if(out->type()==eOutputType::AREALIST ) {
            return ;
        }

        auto vars = out->variables();

        auto varIndex = vars.size()-1;

        // valores distintos tabulados
        auto varValues = exporter->totValuesTabulated();

        // first row
        if(out->dimension()>1 ) {
            tb1[0][varIndex].set_cell_span(varValues + 1);
            tb1[0][varIndex].set_cell_text_align(fort::text_align::center);
            tb1.column(vars.size() + varValues - 1).set_cell_text_align(fort::text_align::right);
        }

        // second row: align VALUES to right
        auto index = this->out->variables().size()-1;
        for( auto j=vars.size()-1;j<cols;j++ ) {

            tb1.column(j).set_cell_text_align( fort::text_align::right);
            index++;
        }
    }

    std::string content() {
        return tb1.to_string();
    }
};
//---------------------------------------------------------------------------------
TRedTextExporter::TRedTextExporter() {

}

TRedTextExporter::~TRedTextExporter() {

}

std::string TRedTextExporter::exportRaw( TRedOutput* output ) {
//    csvfile ff;
//    auto ds = output->dataset();
//
//    int cols = ds->numFields();
//    for(int i=0;i<cols;i++ ) {
//        auto field = ds->fieldByIndex(i);
//        ff<<field->name();
//    }
//    ff<<endrow;
//
//    ds->first();
//    while( ds->next() ) {
//        int cols = ds->numFields();
//
//        for(int i=0;i<cols;i++ ) {
//            auto field = ds->fieldByIndex(i);
//
//            if(field->isNull() ) {
//                ff<<"";
//            }
//            else {
//                if (field->type() == eVarType::integer) {
//                    ff << field->asInteger();
//                } else if (field->type() == eVarType::real) {
//                    ff << field->asReal();
//                } else if (field->type() == eVarType::string) {
//                    ff << field->asString();
//                }
//            }
//        }
//
//        ff<<endrow;
//    }
//
//    return ff.str();
    return "";
}

TRedExporter::Buffer TRedTextExporter::exportToBuffer( TRedOutput* output, bool raw ) {
    TRedConsoleTablePrinter printer(output);

    auto exporter = output->exporter(false);
    exporter->exportTo(&printer);

    std::string content = printer.content();

    Buffer ret;

    std::copy( content.begin(), content.end(), std::back_inserter(ret));

    return ret;
}

