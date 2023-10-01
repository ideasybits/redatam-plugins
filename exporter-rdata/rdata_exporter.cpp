//
// Created by jsalvador on 26/9/2022.
//

#include "rdata_exporter.h"

#include <fstream>
#include <redatam/utils.h>
#include <fmt/core.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <stdio.h>

#include "librdata/rdata.h"

//---------------------------------------------------------------------------------
class TRedRDATAExporterFactory : public TRedExporterFactory {
public:
    TRedRDATAExporterFactory() = default;
    virtual ~TRedRDATAExporterFactory()=default;

    virtual std::shared_ptr<TRedExporter> createExporter() override {
        return std::make_shared<TRedRDATAExporter>();
    }

    virtual std::string type() const override {
        return EXPORTER_RDATA;//TRedXlsxExporter::EXPORTER_NAME;
    }

    virtual std::string description() const override {
        return std::string("RDATA Redatam exporter (.rdata files)");
    }
};

void register_exporter_rdata() {
    red::registerExporterType( std::make_shared<TRedRDATAExporterFactory>() );
}

REDATAM_PLUNGIN_EXPORTER_FN_NAME() {
    register_exporter_rdata();
}

//---------------------------------------------------------------------------------

static ssize_t write_data(const void *bytes, size_t len, void *ctx) {
    int fd = *(int *)ctx;
    return write(fd, bytes, len);
}

void test_kk() {
    int row_count = 3;
    int fd = open("c:/z_temp/somewhere.rdata", O_CREAT | O_WRONLY | O_BINARY);
    rdata_writer_t *writer = rdata_writer_init(&write_data, RDATA_WORKSPACE);

    rdata_column_t *col1 = rdata_add_column(writer, "column1", RDATA_TYPE_REAL);
    rdata_column_t *col2 = rdata_add_column(writer, "column2", RDATA_TYPE_STRING);

    rdata_begin_file(writer, &fd);
    rdata_begin_table(writer, "my_table");

    rdata_begin_column(writer, col1, row_count);
    rdata_append_real_value(writer, 0.0);
    rdata_append_real_value(writer, 100.0);
    rdata_append_real_value(writer, NAN);
    rdata_end_column(writer, col1);

    rdata_begin_column(writer, col2, row_count);
    rdata_append_string_value(writer, "hello");
    rdata_append_string_value(writer, "goodbye");
    rdata_append_string_value(writer, NULL);
    rdata_end_column(writer, col2);

    rdata_end_table(writer, row_count, "My data set");
    rdata_end_file(writer);

    close(fd);
}

//---------------------------------------------------------------------------------
class TRedMinipXlsxPrinter : public TRedOutputPrinter {
private:
    size_t rows, cols;
    TRedOutput* out;

    rdata_writer_t *writer;
    int fd;
    std::vector<rdata_column_t*> columns;

    std::vector<std::vector<red::variant>> values;

            //--options
    int decimals;
public:
    TRedMinipXlsxPrinter( TRedOutput* out, const std::string& fileName ) {
        this->out = out;

        fd = open( fileName.c_str(), _O_CREAT | _O_WRONLY | O_BINARY );
        writer = rdata_writer_init(&write_data, RDATA_WORKSPACE);
    }

    virtual void printValue( size_t row, size_t col, red::variant value ) override {

        auto& vals = values.at(col);

        vals.push_back(value);
    }

    virtual void setSize( size_t rows, size_t cols ) override {
        this->rows = rows;
        this->cols = cols;

        for( int i=0;i<cols; i++ ) {
            values.push_back( {} );
        }
    }

    virtual void end( ) override {

        //for( auto col:columns ) {
        //    rdata_begin_column( writer, col, this->rows );
        //}

        //for( auto col:columns ) {
        //    rdata_end_column( writer, col );
        //}

        std::vector<rdata_column_t*> columns;
        int index = 0;
        for(auto vals:values ) {
            auto col_name = fmt::format("column{}", index++);

//            auto data = values.at(index++);
//            auto myval = data.at(0);

//            rdata_column_t *col = nullptr;
//
//            if( std::holds_alternative<int64_t>(myval) ) {
//                col = rdata_add_column(writer, col_name.c_str(), RDATA_TYPE_INT32);
//            }
//            else if( std::holds_alternative<double>(myval) ) {
//                col = rdata_add_column(writer, col_name.c_str(), RDATA_TYPE_REAL);
//            }
//            else if( std::holds_alternative<std::string>(myval) ) {
//                col = rdata_add_column(writer, col_name.c_str(), RDATA_TYPE_STRING);
//            }

            rdata_column_t *col = rdata_add_column(writer, col_name.c_str(), RDATA_TYPE_STRING);

            columns.push_back(col);
        }

        rdata_begin_file(writer, &fd);
        rdata_begin_table(writer, "my_table");

        index=0;
        for( auto col:columns ) {
            rdata_begin_column(writer, col, this->rows);

            auto data = values.at(index++);

            for( int i=0;i<this->rows; i++ ) {
                auto myval = data.at(i);

                rdata_append_string_value(writer, red::variant_to_string(myval).c_str() );

//                if( std::holds_alternative<int64_t>(myval) ) {
//                    rdata_append_int32_value(writer, std::get<int64_t>(myval) );
//                }
//                else if( std::holds_alternative<double>(myval) ) {
//                    rdata_append_real_value(writer, std::get<double>(myval));
//                }
//                else if( std::holds_alternative<std::string>(myval) ) {
//                    rdata_append_string_value(writer, std::get<std::string>(myval).c_str());
//                }
            }

            rdata_end_column(writer, col);
        }

        rdata_end_table(writer, this->rows, "My data set");
        rdata_end_file(writer);

        close(fd);

        test_kk();
    }
};

//---------------------------------------------------------------------------------
TRedRDATAExporter::TRedRDATAExporter() : TRedExporter() {

}

TRedRDATAExporter::~TRedRDATAExporter() {

}

std::string TRedRDATAExporter::exportRaw( TRedOutput* output ) {
    return "";
}

TRedExporter::Buffer TRedRDATAExporter::exportToBuffer( TRedOutput* output, bool raw ) {

//    auto params = output->options().save_params;
//
//    if(params.find("width")!=params.end() ) {
//        width = std::stoi(params.at("width"));
//    }

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

