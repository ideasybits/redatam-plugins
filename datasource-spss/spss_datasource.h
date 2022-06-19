//
// Created by jsalvador on 9/3/2022.
//

#ifndef REDENGINE_SPSS_DATASOURCE_H
#define REDENGINE_SPSS_DATASOURCE_H

#include <redatam/commons.h>
#include <redatam/datasource/datasource.h>

#include "readstat.h"

#include <functional>

const std::string DATASOURCE_SPSS = "SPSS";
const std::string DATASOURCE_STATA = "STATA";

using TRedDatasetStataReadFn=std::function<readstat_error_t(readstat_parser_t*,const char *,void *)>;

class TReadstatDatasourceField : public TRedDatasourceField {

public:

    std::string _fieldName;
    eVarState _state;
    red::variant _value;

    explicit TReadstatDatasourceField( TRedDatasource* owner );
    virtual ~TReadstatDatasourceField( );

    virtual bool isNull() const override;
    virtual int64_t asInteger( ) override;
    virtual double asReal( ) override;
    virtual std::string asString( ) override;

    virtual  bool isNotApp() const override;
    virtual  bool isMissing() const override;

    virtual std::string name() override;
};

class TReadstatDatasource : public TRedDatasource {

public:
    bool _eof;

    std::string _data_file_name;
    int64_t _current_pos;
    int64_t _rec_count;

    std::vector<TReadstatDatasourceField*> _fields;
    std::map<std::string, TReadstatDatasourceField*, red::CaseInsensitiveComparator> _fields_cache;

    TRedPushDatasource* push;

    TRedDatasetStataReadFn read_data;

public:

    TReadstatDatasource();
    virtual ~TReadstatDatasource();

//    virtual std::string typeName( ) const override;
    virtual void initialize( _init_params& params ) override;

    //-- multiple source
    //virtual bool isMultiple() const override { return false; };

    // Abrir/Cerrar el origen de Datos
    virtual bool open( const std::string& fileName ) override;
    virtual bool openData( const std::string& fileName ) override;
    virtual bool close( ) override;
    virtual bool isOpen( ) override;

    // Número de registros almacenados
    virtual int64_t recCount( ) override;
    virtual int64_t  recPos()  override;

    // Funciones que permiten navegar por los datos
    virtual void first( ) override;
    virtual void next( TRedPushDatasource* push=nullptr ) override;

    // TRUE si se está en el final del archivo, FALSE en otro caso
    virtual bool eof( ) override;

    virtual TRedDatasourceField* fieldByName( const std::string& fname ) override;

    //-- nuevas
    virtual void parseMetadata( const std::string& text ) override;
    virtual TRedCreateEntityMetadata* parseEntityMetadata( const std::string& entName, const std::string& text ) override;
    virtual TRedCreateVariableMetadata* parseVariableMetadata( const TRedVariable* var, const std::string& text ) override;

    virtual void release() override;

    virtual std::tuple<std::string,std::string,std::string> metadataSchema( ) override;

    virtual bool isPush() { return true; };
};

class TSpssDatasource : public TReadstatDatasource {
public:
    TSpssDatasource() {
        this->read_data = readstat_parse_sav;
    };

    virtual ~TSpssDatasource() = default;

    virtual std::string typeName( ) const override {
        return DATASOURCE_SPSS;
    }
};

class TStataDatasource : public TReadstatDatasource {
public:
    TStataDatasource() {
        this->read_data = readstat_parse_dta;
    };

    virtual ~TStataDatasource() = default;

    virtual std::string typeName( ) const override {
        return DATASOURCE_STATA;
    }
};

#endif //REDENGINE_SPSS_DATASOURCE_H
