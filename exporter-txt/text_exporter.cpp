//
// Created by jsalvador on 17/6/2022.
//

#include "text_exporter.h"

#include <fstream>
#include <redatam/utils.h>
#include <fmt/core.h>

#include "fort.hpp"

//DEFINE_PLUGIN_EXPORTER( EXPORTER_TEXT,
//                        TRedTextExporter, "TEXT Redatam exporter (.txt files)" );
//
//REDATAM_PLUNGIN_EXPORTER_FN_NAME() {
//    REGISTER_EXPORTER( EXPORTER_TEXT );
//}

//---------------------------------------------------------------------------------
class TRedConsoleTablePrinter : public TRedOutputPrinter {
private:
    TRedOutput* out;
    size_t rows, cols;
    int decimals;

    fort::utf8_table tb1;
public:
    TRedConsoleTablePrinter( TRedOutput* out ) {
        this->out = out;

        decimals = 4;
        if( out->options().decimals.has_value() ) {
            this->decimals = out->options().decimals.value();
        }
    }

    virtual void printValue( size_t row, size_t col, red::variant value ) override {
        if( std::holds_alternative<int64_t>(value)) {
            int64_t val = std::get<int64_t>(value);
            tb1[row][col] = fmt::format("{}",val);
        }
        else if( std::holds_alternative<double>(value)) {
            double val = std::get<double>(value);
            tb1[row][col] = fmt::format("{:.{}f}",val, decimals );
        }
        else if( std::holds_alternative<std::string>(value)) {
            std::string val = std::get<std::string>(value);
            tb1[row][col] = val;
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

        if( out->type()==eOutputType::AREALIST || out->type()==eOutputType::TABLIST ) {
            return ;
        }

        if(out->dimension()==0 ) {
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
    auto dataset = output->dataset();

    dataset->first();

    size_t fieldCount = dataset->numFields();

    fort::utf8_table table;

    table << fort::header;

    for( int i=0; i<fieldCount; i++ ) {
        auto field = dataset->fieldByIndex(i);

        table << fmt::format( "{}({})", field->name(), (int )field->type() );
    }

    table << fort::endr;

    while (dataset->next()) {
        for( int i=0; i<fieldCount; i++ ) {
            auto field = dataset->fieldByIndex(i);

            if( field->isNull() ) {
                table << "-";
            }
            else {
                table << fmt::format( "{}", field->asString() );
            }
        }

        table << fort::endr;
    }

    fmt::print("\n");

    for( int i=0; i<fieldCount; i+=3 ) {
        table.column(i).set_cell_text_align(fort::text_align::right);
        table.column(i+2).set_cell_text_align(fort::text_align::center);
    }

    table.column(fieldCount-1).set_cell_text_align(fort::text_align::right);

    return table.to_string();
}

TRedExporter::Buffer TRedTextExporter::exportToBuffer( TRedOutput* output, bool raw ) {

    std::string content;

    if( raw ) {
        content = exportRaw(output);
    }
    else {

        TRedConsoleTablePrinter printer(output);

        auto exporter = output->exporter(false);
        exporter->exportTo(&printer);

        content = printer.content();
    }

    Buffer ret;

    std::copy( content.begin(), content.end(), std::back_inserter(ret));

    return ret;
}

