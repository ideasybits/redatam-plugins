//
// Created by jsalvador on 9/3/2022.
//

#include "spss_datasource.h"

#include <redatam/utils.h>
#include <redatam/exceptions.h>

#include <fmt/core.h>
#include <nlohmann/json.hpp>

using json_type=nlohmann::ordered_json;

REGISTER_PLUGIN_DATASOURCE( DATASOURCE_SPSS,
                            TSpssDatasource,
                            "SPSS Redatam datasource" );

REGISTER_PLUGIN_DATASOURCE( DATASOURCE_STATA,
                            TStataDatasource,
                            "STATA Redatam datasource" );

REDATAM_PLUNGIN_EXPORTER_FN_NAME() {
    REGISTER_DATASOURCE( DATASOURCE_SPSS );
    REGISTER_DATASOURCE( DATASOURCE_STATA );
}

//-----------------------------------------------------------------------------
int handle_metadata(readstat_metadata_t *metadata, void *ctx) {
    auto my_count = readstat_get_row_count(metadata);

    TReadstatDatasource* ds = static_cast<TReadstatDatasource*>(ctx);

    ds->_rec_count = my_count;

    return READSTAT_HANDLER_OK;
}

int handle_variableDummy(int index, readstat_variable_t *variable,const char *val_labels, void *ctx) {
    return READSTAT_HANDLER_OK;
}

int handle_variable(int index, readstat_variable_t *variable,const char *val_labels, void *ctx) {

    TReadstatDatasource* ds = static_cast<TReadstatDatasource*>(ctx);

    TReadstatDatasourceField* field = new TReadstatDatasourceField(ds);

    auto name = readstat_variable_get_name(variable);

    field->_fieldName = name;

    ds->_fields.push_back( field );
    ds->_fields_cache[field->_fieldName] = field;

    return READSTAT_HANDLER_OK;
}

int handle_value(int obs_index, readstat_variable_t *variable, readstat_value_t value, void *ctx) {

    TReadstatDatasource* ds = static_cast<TReadstatDatasource*>(ctx);

    int var_index = readstat_variable_get_index(variable);

    ds->_current_pos = obs_index;
    ds->_eof = obs_index >= ds->_rec_count;

    readstat_type_t type = readstat_value_type(value);
    auto name = readstat_variable_get_name(variable);

    auto field = ds->_fields_cache[name];

    if (!readstat_value_is_system_missing(value)) {
        if (type == READSTAT_TYPE_STRING) {
            field->_value = readstat_string_value(value);
        } else if (type == READSTAT_TYPE_INT8) {
            field->_value = (int64_t )readstat_int8_value(value);
        } else if (type == READSTAT_TYPE_INT16) {
            field->_value = (int64_t )readstat_int16_value(value);
        } else if (type == READSTAT_TYPE_INT32) {
            field->_value = (int64_t )readstat_int32_value(value);
        } else if (type == READSTAT_TYPE_FLOAT) {
            field->_value = (double )readstat_float_value(value);
        } else if (type == READSTAT_TYPE_DOUBLE) {
            field->_value = (double )readstat_double_value(value);
        }
    }
    else {
        field->_state = eVarState::missing;
    }

    if (var_index == ds->_fields.size() - 1) {
        // push the data
        bool next = ds->push->processData();

        if(!next) {
            return READSTAT_HANDLER_ABORT;
        }
    }

    return READSTAT_HANDLER_OK;
}
//-----------------------------------------------------------------------------
TReadstatDatasourceField::TReadstatDatasourceField( TRedDatasource* owner )
        : TRedDatasourceField(owner) {

}

TReadstatDatasourceField::~TReadstatDatasourceField( ) {

}

bool TReadstatDatasourceField::isNull() const{
    //return _value.empty();
    return false;
}

int64_t TReadstatDatasourceField::asInteger( ) {
    return red::variant_to_int64(_value);
}

double TReadstatDatasourceField::asReal( ) {
    return red::variant_to_double(_value);
}

std::string TReadstatDatasourceField::asString( ) {
    return red::variant_to_string(_value);
}

bool TReadstatDatasourceField::isNotApp() const {
    return _state==eVarState::notapp;
}

bool TReadstatDatasourceField::isMissing() const {
    return _state==eVarState::missing;
}

std::string TReadstatDatasourceField::name() {
    return _fieldName;
}

//-----------------------------------------------------------------------------
TReadstatDatasource::TReadstatDatasource() {
    _current_pos = 0;
    _eof = false;

    _rec_count = 0;

    push = nullptr;

    read_data = readstat_parse_sav;
}

TReadstatDatasource::~TReadstatDatasource() {
    // free fields
    for( auto it:_fields ) {
        delete it;
    }

    _fields.clear();
}

void TReadstatDatasource::parseMetadata( const std::string& text ) {
    json_type json = json_type::parse(text);
}

TRedCreateEntityMetadata* TReadstatDatasource::parseEntityMetadata( const std::string& entName, const std::string& text ) {

    TRedCreateEntityMetadata* ret = new TRedCreateEntityMetadata();

    // parse common data
    TRedDatasource::parseCommonEntityMetadata(entName, text, ret );

    return ret;
}

TRedCreateVariableMetadata* TReadstatDatasource::parseVariableMetadata( const TRedVariable* var, const std::string& text ) {

    TRedCreateVariableMetadata* ret = new TRedCreateVariableMetadata();

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

void TReadstatDatasource::initialize( _init_params& params ) {

}

bool TReadstatDatasource::open( const std::string& fileName ) {

    _eof = false;

    readstat_error_t error = READSTAT_OK;

    //--parse metadata
    readstat_parser_t *parser = readstat_parser_init();
    readstat_set_metadata_handler(parser, &handle_metadata);
    readstat_set_variable_handler(parser, &handle_variable);

    error = read_data(parser, fileName.c_str(), this);

    readstat_parser_free(parser);

    if (error != READSTAT_OK) {
        auto msg = fmt::format("Error processing SPSS file {}: {}\n", this->_data_file_name, readstat_error_message(error));
        throw red::exception(msg);
    }

    return true;
}

bool TReadstatDatasource::openData( const std::string& fileName ) {

    if( std::filesystem::exists(fileName)==false ) {
        auto msg = fmt::format( "Datasource file no found {}", fileName.c_str() );
        throw red::exception( msg );
    }

    _eof = false;

    _data_file_name = fileName;

    return true;
}

bool TReadstatDatasource::close( ) {
    return true;
}

bool TReadstatDatasource::isOpen( ) {
    return false;
}

int64_t TReadstatDatasource::recCount( ) {
    return _rec_count;
}

int64_t TReadstatDatasource::recPos( ) {
    return _current_pos;
}

void TReadstatDatasource::first( ) {
    _eof = false;
}

void TReadstatDatasource::next( TRedPushDatasource* push ) {

    // this is a PUSH datasource, so we need to call stat->processData();
    this->push = push;

    readstat_error_t error = READSTAT_OK;

    //--parse metadata
    readstat_parser_t *parser = readstat_parser_init();
    readstat_set_metadata_handler(parser, &handle_metadata);
    readstat_set_variable_handler(parser, &handle_variableDummy);

    readstat_set_value_handler(parser, &handle_value);

    error = read_data(parser, this->_data_file_name.c_str(), this);

    readstat_parser_free(parser);

    if (error != READSTAT_OK) {
        auto push_ex = push->exception();

        if( push_ex.has_value() ) {
            throw push_ex.value();
        }
        else {
            auto msg = fmt::format("Error processing {} file {}: {}\n", this->typeName(), this->_data_file_name,
                                   readstat_error_message(error));
            throw red::exception(msg);
        }
    }

}

bool TReadstatDatasource::eof( ) {
    return _eof;
}

TRedDatasourceField* TReadstatDatasource::fieldByName( const std::string& fname ) {
    auto it = _fields_cache.find(fname);

    if( it!=_fields_cache.end() ) {
        return it->second;
    }

    return nullptr;
}

void TReadstatDatasource::release() {
    delete this;
}

std::tuple<std::string,std::string,std::string> TReadstatDatasource::metadataSchema( ) {
    return {"","",""};
}

