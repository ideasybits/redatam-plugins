//
// Created by jsalvador on 26/9/2022.
//

#include "xlsx_exporter.h"

#include <fstream>
#include <redatam/utils.h>
#include <fmt/core.h>
#include <xlsxwriter.h>

DEFINE_PLUGIN_EXPORTER( EXPORTER_XLSX,
                        TRedXlsxExporter, "XLSX Redatam exporter (.xlsx files)" );

REDATAM_PLUNGIN_EXPORTER_FN_NAME() {
    REGISTER_EXPORTER( EXPORTER_XLSX );
}

//---------------------------------------------------------------------------------
class TRedMinipXlsxPrinter : public TRedOutputPrinter {
private:
    size_t rows, cols;
    TRedOutput* out;

    lxw_workbook  *workbook;
    lxw_worksheet *worksheet;
    lxw_format    *format;

    lxw_format* format_merge;
    lxw_format* format_number;

    //--options
    int decimals;
public:
    TRedMinipXlsxPrinter( TRedOutput* out, const std::string& fileName ) {
        this->out = out;

        workbook  = workbook_new( fileName.c_str() );
        worksheet = workbook_add_worksheet(workbook, NULL);

        format = workbook_add_format(workbook);

        {
            format_merge = workbook_add_format(workbook);
            format_set_align(format_merge, LXW_ALIGN_VERTICAL_CENTER);
        }

        format_set_bold(format);
        format_set_bg_color(format, 0xcedaf5);

        format_number = nullptr;

        //--options
        if( out->options().decimals.has_value() ) {
            this->decimals = out->options().decimals.value();
            std::string nformat = "#,##0." + std::string(this->decimals, '0');

            format_number = workbook_add_format(workbook);
            format_set_num_format(format_number, nformat.c_str());
        }

    }

    virtual void printValue( size_t row, size_t col, red::variant value ) override {

        lxw_format *celFormat = nullptr;
        //lxw_format *realCellFormat = workbook_add_format(workbook);

        //format_set_num_format( realCellFormat, "0.0000");
        //format_set_num_format(realCellFormat, "#,##0.00");

        if( out->dimension()==0 ) {

        }
        else if(out->dimension()==1 ) {
            if( row==0 ) {
                celFormat = this->format;
            }
        }
        else {
            if( row==0 || row==1 ) {
                celFormat = this->format;
            }
        }

        if( std::holds_alternative<int64_t>(value) ) {
            int64_t val = std::get<int64_t>(value);
            worksheet_write_number(worksheet, (lxw_col_t )row, (lxw_col_t )col, val, celFormat);
        }
        else if( std::holds_alternative<double>(value) ) {
            double val = std::get<double>(value);
            worksheet_write_number(worksheet, (lxw_col_t )row, (lxw_col_t )col, val, format_number);
        }
        else if( std::holds_alternative<std::string>(value) ) {
            std::string val = std::get<std::string>(value);
            worksheet_write_string(worksheet, (lxw_col_t )row, (lxw_col_t )col, val.c_str(), celFormat);
        }
    }

    virtual void setSize( size_t rows, size_t cols ) override {
        this->rows = rows;
        this->cols = cols;
    }

    virtual void end( ) override {
        // merge cells
        //worksheet_merge_range(worksheet,0,0,1,0,"",format1);
        workbook_close(workbook);
    }
};

//---------------------------------------------------------------------------------
TRedXlsxExporter::TRedXlsxExporter() : TRedExporter() {

}

TRedXlsxExporter::~TRedXlsxExporter() {

}

std::string TRedXlsxExporter::exportRaw( TRedOutput* output ) {
    return "";
}

TRedExporter::Buffer TRedXlsxExporter::exportToBuffer( TRedOutput* output, bool raw ) {

    std::string content;

    std::string tmpFilename = fmt::format("{}.xlsx", std::tmpnam(nullptr) );

    if( raw ) {
        content = exportRaw(output);
    }
    else {


        TRedMinipXlsxPrinter printer( output, tmpFilename);

        auto exporter = output->exporter(false);

        exporter->exportTo(&printer);

        // load the tmpfile
        content = red::readWholeBinFile(tmpFilename);
    }

    Buffer ret;

    std::copy( content.begin(), content.end(), std::back_inserter(ret) );

    return ret;
}

