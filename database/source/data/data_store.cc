#include "data/data_store.h"
#include <iostream>


//creates an empty table
DataStore::Error DataStore::createTable(CreateTableCommand table_info)
{
    if(table_info.columns.size() <= 0)
    {
        // TODO: Returns an error code, statement cannot execute
        return INVALID_SCHEMA;
    }

    TableSchema* schema = new TableSchema;

    // This code assumes that table_info.column_constraints uses 
    // SQLConstraintType.SQL_NO_CONSTRAINT when a column doesn't
    // have a constraint.
    for(uint32_t i = 0; i < table_info.columns.size(); i++)
    {
        // TODO: Handle return value
        schema->columns.push_back(table_info.columns[i]);
    }

    DataTable* new_table = new DataTable();

    SchemaTablePair* pair = new SchemaTablePair(new_table, schema);

    table_name_mapping.insert( table_info.table_name, pair);

    return SUCCESS;
}

DataStore::Error DataStore::deleteTable(std::string table_name)
{
    // TODO: What does access_counter != 0 mean?
    // Remove the 
	if(!table_name_mapping.remove(table_name))
    {
		return INVALID_TABLE;
    }

	//TODO: may need to return a success code
    return SUCCESS;
}

DataTable *DataStore::getTable(std::string table_name)
{
	//Tervel hashmap seems to use get_position for the key value
	//at seems to be the function to use but the valueaccessor parameter 
	//wants the address of the value

    TableMap::ValueAccessor value;
    if(table_name_mapping.at(table_name,value))
    {
        if(value.valid())
        {
            SchemaTablePair* pair = *value.value();
            return pair->table;
        }

        return nullptr;
    }

    return nullptr;
}

TableSchema *DataStore::getTableSchema(std::string table_name)
{
    SchemaTablePair* table_pair;
    TableMap::ValueAccessor value;
    if(table_name_mapping.at( table_name, value))
    {
        if(value.valid())
        {
            table_pair = *value.value();
            return table_pair->schema;
        }

        return nullptr;
    }

    return nullptr;
}

DataStore::Error DataStore::getRow(std::string table_name, uint32_t row_id, Record** record)
{
    DataTable* table = getTable(table_name);

    // TODO: Need to do std::atomic<>.load() on table

    Record* req_record;
    if(table != nullptr && table->at(row_id, req_record))
    {
        // TODO: Replace with std::atomic<>.load() on the record
        *record = req_record;
        return SUCCESS;
    }

    return INVALID_TABLE;
}

