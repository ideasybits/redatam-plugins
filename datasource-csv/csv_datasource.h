#ifndef REDENGINE_CSV_DATASOURCE_H
#define REDENGINE_CSV_DATASOURCE_H

#include <redatam/commons.h>
#include <redatam/datasource/datasource.h>

#include <fstream>
#include <csv.hpp>

const std::string DATASOURCE_CSV = "CSV";

class TCsvVariableMetadata : public TRedCreateVariableMetadata {
public:
    TCsvVariableMetadata() = default;
};

class TCsvDatasourceField : public TRedDatasourceField {

public:

    std::string _fieldName;
    std::string _value;
    eVarState _state;

    explicit TCsvDatasourceField( TRedDatasource* owner );
    virtual ~TCsvDatasourceField( );

    virtual bool isNull() const override;
    virtual int64_t asInteger( ) override;
    virtual double asReal( ) override;
    virtual std::string asString( ) override;

    virtual  bool isNotApp() const override;
    virtual  bool isMissing() const override;

    virtual std::string name() override;
};

class TCsvDatasource : public TRedDatasource {

protected:

    csv::CSVFormat format;
    csv::CSVRow row;

    std::shared_ptr<csv::CSVReader> reader;

    bool _eof;

    std::string _data_file_name;
    int64_t _current_pos;

    std::vector<TCsvDatasourceField*> _fields;
    std::map<std::string, TCsvDatasourceField*, red::CaseInsensitiveComparator> _fields_cache;

public:

    TCsvDatasource();
    virtual ~TCsvDatasource();

    virtual std::string typeName( ) const override;
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
};

#endif //REDENGINE_CSV_DATASOURCE_H
