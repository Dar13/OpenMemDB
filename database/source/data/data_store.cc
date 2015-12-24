#include "data/data_store.h"
#include <iostream>


//creates an empty table
ManipResult DataStore::createTable(CreateTableCommand table_info)
{
    if(table_info.columns.size() <= 0)
    {
        // TODO: Returns an error code, statement cannot execute
        return ManipResult(ResultStatus::INVALID_TABLE,
                           ManipStatus::SUCCESS);
    }

    TableSchema* schema = new (std::nothrow) TableSchema;
    if(schema == nullptr)
    {
        // TODO: Error handling
    }

    // Copy the column information into the stored schema
    schema->columns = table_info.columns;

    DataTable* new_table = new (std::nothrow) DataTable();
    if(new_table == nullptr)
    {
        // TODO: Error handling
    }

    SchemaTablePair* pair = new (std::nothrow) SchemaTablePair(new_table, schema);
    if(pair == nullptr)
    {
        // TODO: Error handling
    }

    table_name_mapping.insert( table_info.table_name, pair);

    return ManipResult(ResultStatus::SUCCESS, ManipStatus::SUCCESS);
}

ManipResult DataStore::deleteTable(std::string table_name)
{
    // Remove the table from the table name map
    // TODO: Perform proper memory clean-up after removal from mapping
    // TODO: What does access_counter != 0 mean?
    if(!table_name_mapping.remove(table_name))
    {
        // TODO: Update with appropriate ManipStatus value
        return ManipResult(ResultStatus::INVALID_TABLE, ManipStatus::SUCCESS);
    }

    return ManipResult(ResultStatus::SUCCESS, ManipStatus::SUCCESS);
}

UintResult DataStore::getColumnIndex(std::string table_name, std::string column_name)
{
    SchemaTablePair* table_pair = getTablePair(table_name);
    if(table_pair)
    {
        // This should be safe, we're only reading from the object
        size_t num_columns = table_pair->schema->columns.size();
        for(uint32_t itr = 0; itr < num_columns; itr++)
        {
            if(table_pair->schema->columns[itr].name == column_name)
            {
                return UintResult(ResultStatus::SUCCESS, itr);
            }
        }

        // TODO: Error code for not finding the column in the table?
    }

    // Table name doesn't exist in the table name map
    return UintResult(ResultStatus::INVALID_TABLE, 0);
}

DataResult DataStore::updateData(Predicate* predicates,
                      std::string table_name, uint32_t column_idx,
                      Data value)
{
    // TODO: This function doesn't really make sense to have...
}

DataResult DataStore::getData(Predicate* predicates,
                   std::string table_name, uint32_t column_idx)
{
    // TODO: This function doesn't really make sense to have...
}

ManipResult DataStore::insertRecord(std::string table_name, RecordData record)
{
}

ManipResult DataStore::updateRecord(Predicate* predicates,
                         std::string table_name,
                         RecordData record)
{
}

MultiRecordResult DataStore::getRecords(Predicate* predicates,
                                        std::string table_name)
{
    // Evaluate predicates and return all rows that satisfy the predicates'
    // conditions.
    //
    // If there are no predicates, do a full table copy.
    if(predicates == nullptr)
    {
        MultiRecordData data;

    }
    else
    {
        // TODO: Actually evaluate the predicates
    }
}

SchemaTablePair* DataStore::getTablePair(std::string table_name)
{
    TableMap::ValueAccessor hash_accessor;
    if(table_name_mapping.at(table_name, hash_accessor))
    {
        if(hash_accessor.valid())
        {
            // TODO: Is this safe? I think so, but ...
            SchemaTablePair* pair = (*hash_accessor.value());
            return pair;
        }
    }

    return nullptr;
}
