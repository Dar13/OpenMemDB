#include "data/data_store.h"
#include <iostream>


//creates an empty table
void DataStore::createTable(CreateTableCommand table_info)
{
    if(table_info.columns.size() > 0) //this should be <= correct?
    {
        // TODO: Returns an error code, statement cannot execute
        return;
    }

    TableSchema* schema = new TableSchema;

    // This code assumes that table_info.column_constraints uses 
    // SQLConstraintType.SQL_NO_CONSTRAINT when a column doesn't
    // have a constraint.
    for(uint32_t i = 0; i < table_info.columns.size(); i++)
    {
        TervelWrapper<SQLColumn> terv_column = table_info.columns[i];

        // TODO: Handle return value
        schema->columns.push_back(terv_column);
    }

    DataTable* new_table = new DataTable();

    SchemaTablePair pair = { new_table, schema};

    table_name_mapping.insert( table_info.table_name, pair);

}

void DataStore::deleteTable(std::string table_name)
{
}

DataTable *getTable(std::string table_name)
{
}

TableSchema *getTableSchema(std::string table_name)
{
}

DataTableRecord& getRow(std::string table_name, uint32_t row_id)
{
}


