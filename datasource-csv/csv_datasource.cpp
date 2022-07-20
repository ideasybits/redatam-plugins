#include "csv_datasource.h"

#include <string>
#include <functional>
#include <filesystem>

#include <redatam/utils.h>
#include <redatam/exceptions.h>

#include <fmt/core.h>
#include <nlohmann/json.hpp>
#include <boost/algorithm/string.hpp>

using json_type=nlohmann::ordered_json;

REGISTER_PLUGIN_DATASOURCE( DATASOURCE_CSV,
                            TCsvDatasource,
                            "CSV Redatam datasource" );

REDATAM_PLUNGIN_EXPORTER_FN_NAME() {
    REGISTER_DATASOURCE( DATASOURCE_CSV );
}
//-----------------------------------------------------------------------------
TCsvDatasourceField::TCsvDatasourceField( TRedDatasource* owner )
    : TRedDatasourceField(owner) {

}

TCsvDatasourceField::~TCsvDatasourceField( ) {

}

bool TCsvDatasourceField::isNull() const{
    return _value.empty();
}

int64_t TCsvDatasourceField::asInteger( ) {
    try {
        return std::stoi(_value);
    }
    catch( std::exception& ) {
        return -1;
    }
}

double TCsvDatasourceField::asReal( ) {
    try {
        return std::stod(_value);
    }
    catch( std::exception& ) {
        return -1;
    }
}

std::string TCsvDatasourceField::asString( ) {
    return _value;
}

bool TCsvDatasourceField::isNotApp() const {
    std::string tmp = _value;

    boost::trim(tmp);

    return tmp.empty();
}

bool TCsvDatasourceField::isMissing() const {
    return _state==eVarState::missing;
}

std::string TCsvDatasourceField::name() {
    return _fieldName;
}

//-----------------------------------------------------------------------------
TCsvDatasource::TCsvDatasource() {
    _current_pos = 0;
    _eof = false;
}

TCsvDatasource::~TCsvDatasource() {
    // free fields
    for( auto it:_fields ) {
        delete it;
    }

    _fields.clear();
}

void TCsvDatasource::parseMetadata( const std::string& text ) {
    json_type json = json_type::parse(text);

    if( json.contains("header") ) {
        bool csvHasHeader = json["header"];

        if( !csvHasHeader ) {
            format.no_header();
        }
    }

    if( json.contains("separator") ) {
        std::string csvSeparator = json["separator"];

        char sep = csvSeparator[0];
        format.delimiter( sep );
    }
}

TRedCreateEntityMetadata* TCsvDatasource::parseEntityMetadata( const std::string& entName, const std::string& text ) {

    TRedCreateEntityMetadata* ret = new TRedCreateEntityMetadata();

    // parse common data
    TRedDatasource::parseCommonEntityMetadata(entName, text, ret );

    return ret;
}

TRedCreateVariableMetadata* TCsvDatasource::parseVariableMetadata( const TRedVariable* var, const std::string& text ) {

    TCsvVariableMetadata* ret = new TCsvVariableMetadata();

    // parse common data: lookup, field, record, start, length
    TRedDatasource::parseCommonVariableMetadata( var, text, ret );

    if( !ret->_lookup ) {
        if (_fields_cache.find(ret->_field) == _fields_cache.end()) {
            // error, field no existe
            auto msg = fmt::format("{}: field '{}' not found for variable '{}'", typeName(), ret->_field, var->fullName() );
            throw red::exception(msg);
        }
    }

    return ret;
}

std::string TCsvDatasource::typeName( ) const {
    return DATASOURCE_CSV;
}

void TCsvDatasource::initialize( _init_params& params ) {

}

bool TCsvDatasource::open( const std::string& fileName ) {

    _eof = false;

    _data_file_name = fileName;

    csv::CSVReader ed( fileName, format );

    auto cols = ed.get_col_names();

    for( auto name:cols ) {

        TCsvDatasourceField* field = new TCsvDatasourceField( this );

        field->_fieldName = name;

        _fields.push_back( field );
        _fields_cache[field->_fieldName] = field;
    }

    return true;
}

bool TCsvDatasource::openData( const std::string& fileName ) {

    if( std::filesystem::exists(fileName)==false ) {
        auto msg = fmt::format( "Datasource file no found {}", fileName.c_str() );
        throw red::exception( msg );
    }

    _eof = false;

    reader = std::make_shared<csv::CSVReader>( fileName, format );

    auto colNames = reader->get_col_names();

    _data_file_name = fileName;

    return true;
}

bool TCsvDatasource::close( ) {
    reader.reset();

    return true;
}

bool TCsvDatasource::isOpen( ) {
    return reader!= nullptr;
}

int64_t TCsvDatasource::recCount( ) {
    return red::fileSize( _data_file_name );
}

int64_t TCsvDatasource::recPos( ) {
    return _current_pos;
}
void TCsvDatasource::first( ) {
    _eof = false;
}

void TCsvDatasource::next( TRedPushDatasource* /*push*/ ) {
    _eof = reader->read_row(row)==false;

    if( _eof ) {
        return ;
    }

    for (csv::CSVField& field: row) {
        auto str = field.get_sv();

        if( field.is_str() ) {
            _current_pos += 2;
        }

        _current_pos += str.length();
    }

    size_t index = 0;
    for (auto field:_fields) {
        field->_state = eVarState::valid;

        field->_value = row[index].get();

        if( field->_value.empty() ) {
            field->_state = eVarState::notapp;
        }

        index++;
    }
}

bool TCsvDatasource::eof( ) {
    return _eof;
}

TRedDatasourceField* TCsvDatasource::fieldByName( const std::string& fname ) {
    auto it = _fields_cache.find(fname);

    if( it!=_fields_cache.end() ) {
        return it->second;
    }

    return nullptr;
}

void TCsvDatasource::release() {
    delete this;
}

std::tuple<std::string,std::string,std::string> TCsvDatasource::metadataSchema( ) {
    return {"","",""};
}