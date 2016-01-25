// C standard library includes
//
#include <cstdio>

// Project includes
#include "data/data_store.h"

//creates an empty table
ManipResult DataStore::createTable(CreateTableCommand table_info)
{
    if(table_info.columns.size() <= 0)
    {
        // TODO: Returns an error code, statement cannot execute
        return ManipResult(ResultStatus::ERROR_INVALID_TABLE,
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
        return ManipResult(ResultStatus::ERROR_INVALID_TABLE, ManipStatus::SUCCESS);
    }

    return ManipResult(ResultStatus::SUCCESS, ManipStatus::SUCCESS);
}

SchemaResult DataStore::getTableSchema(std::string table_name)
{
    SchemaTablePair* pair = getTablePair(table_name);
    if(pair == nullptr)
    {
        return SchemaResult(ResultStatus::ERROR_INVALID_TABLE, TableSchema());
    }
    else
    {
        TableSchema schema = *pair->schema;
        return SchemaResult(ResultStatus::SUCCESS, schema);
    }
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

	return UintResult(ResultStatus::ERROR_INVALID_COLUMN, 0);
    }

    // Table name doesn't exist in the table name map
    return UintResult(ResultStatus::ERROR_INVALID_TABLE, 0);
}

DataResult DataStore::updateData(Predicate* predicates,
                      std::string table_name, uint32_t column_idx,
                      TervelData value)
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
    // Get the table
    SchemaTablePair* table_pair = getTablePair(table_name);
    if(table_pair == nullptr)
    {
        return ManipResult(ResultStatus::ERROR_INVALID_TABLE, ManipStatus::FAILURE);
    }

    // Insert into the table
    Record* new_record = new (std::nothrow) Record(record.size());
    if(new_record == nullptr)
    {
        return ManipResult(ResultStatus::ERROR_MEM_ALLOC, ManipStatus::FAILURE);
    }

    for(auto data : record)
    {
        TervelData terv_data = { .value = 0 };

        // Ensure Tervel's bits are zeroed and then copy the rest of the data over
        data.data.tervel_status = 0;
        terv_data.value = data.value;

        printf("Inserting %ld\n", terv_data.value);
        size_t ret = new_record->push_back_w_ra(terv_data.value);
        printf("Pushback returned %lu\n", ret);
    }

    // TODO: Handle table constraints
    size_t ret = table_pair->table->push_back_w_ra(new_record);
    printf("Pushback returned %lu\n", ret);
    printf("Record address %p\n", new_record);

    return ManipResult(ResultStatus::SUCCESS, ManipStatus::SUCCESS);
}

ManipResult DataStore::updateRecord(Predicate* predicates,
                         std::string table_name,
                         RecordData record)
{
}

MultiRecordResult DataStore::getRecords(Predicate* predicates,
                                        std::string table_name)
{
    // Get the table pair
    SchemaTablePair* table_pair = getTablePair(table_name);
    if(table_pair == nullptr)
    {
	    return MultiRecordResult(ResultStatus::ERROR_INVALID_TABLE, MultiRecordData());
    }

    DataTable* table = table_pair->table;

    // Evaluate predicates and return all rows that satisfy the predicates'
    // conditions.
    //
    // If there are no predicates, do a full table copy.
    if(predicates == nullptr)
    {
        MultiRecordData data;

	    int64_t table_len = table->size(0);
	    if(table_len == 0)
	    {
	        // Finished, table is empty
		    return MultiRecordResult(ResultStatus::SUCCESS, MultiRecordData());
	    }

	    for(int64_t i = 0; i < table_len; i++)
	    {
	        // Get the current row pointer
	        Record* row = nullptr;
	        
	        // This essentially waits for an access to become available.
	        // TODO: Is this wait-free?
	        while(!table->at(i, row))
	        {}

            printf("Retrieved record address: %p\n", row);

	        RecordData record_copy = copyRecord(row);
	        data.push_back(record_copy);
	    }

	    return MultiRecordResult(ResultStatus::SUCCESS, data);
    }
    else
    {
        MultiRecordData data;

        // TODO: Actually evaluate the predicates
        if(predicates->type == PredicateType::NESTED)
        {
            // TODO: Handle multiple predicates
        }
        else
        {
            switch(predicates->type)
            {
                case PredicateType::VALUE:
                    {
                        ValuePredicate* val_pred = reinterpret_cast<ValuePredicate*>(predicates);
                        if(val_pred->column.table != table_name)
                        {
                            // TODO: Handle this
                        }
                        else
                        {
                            MultiRecordData data = searchTable(table_pair->table,
                                                               val_pred);
                            return MultiRecordResult(ResultStatus::SUCCESS, data);
                        }
                    }
                    break;
                case PredicateType::COLUMN:
                    break;
                default:
                    break;
            }
        }
    }

    return MultiRecordResult(ResultStatus::ERROR_INVALID_TABLE, MultiRecordData());
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

MultiRecordData DataStore::searchTable(DataTable* table,
                                  ValuePredicate* value_pred)
{
    MultiRecordData data;
    if(table == nullptr || value_pred == nullptr)
    {
        return MultiRecordData();
    }

    // This assumes table maps to the table name in the value predicate
    int64_t table_len = table->size(0);
    if(table_len == 0)
    {
        return MultiRecordData();
    }

    for(int64_t idx = 0; idx < table_len; idx++)
    {
        Record* row = nullptr;

        while(!table->at(idx, row)) {}

        RecordData record = copyRecord(row);

        if(record.size() <= value_pred->column.column_idx)
        {
            // TODO: Propagate error up? This should be caught earlier
            return MultiRecordData();
        }

        ExpressionValue row_value(record.at(value_pred->column.column_idx));

        if(evaluateOperation(value_pred->op, 
                             value_pred->expected_value,
                             row_value).IsTrue())
        {
            printf("Record matches predicate!\n");
            data.push_back(record);
        }
    }

    return data;
}

MultipleTableRecordData DataStore::searchTable(std::string table_first, std::string table_second,
        ColumnPredicate* col_pred)
{
    auto default_res = MultipleTableRecordData();
    auto result = MultipleTableRecordData();

    auto f_table_pair = getTablePair(table_first);
    auto s_table_pair = getTablePair(table_second);

    if(f_table_pair == nullptr || s_table_pair == nullptr || col_pred == nullptr)
    {
        return default_res;
    }

    DataTable* f_table = f_table_pair->table;
    DataTable* s_table = s_table_pair->table;

    int64_t first_len = f_table->size(0);
    int64_t second_len = s_table->size(0);
    if(first_len == 0 || second_len == 0)
    {
        return default_res;
    }

    for(int64_t first_idx = 0; first_idx < first_len; first_idx++)
    {
        Record* f_record = nullptr;

        while(!f_table->at(first_idx, f_record)) {}

        RecordData f_data = copyRecord(f_record);

        for(int64_t second_idx = 0; second_idx < second_len; second_idx++)
        {
            Record* s_record = nullptr;

            while(!s_table->at(second_idx, s_record)) {}

            RecordData s_data = copyRecord(s_record);

            TervelData f_value = f_data[col_pred->left_column.column_idx];
            TervelData s_value = s_data[col_pred->right_column.column_idx];
            if(f_value.data.value == s_value.data.value)
            {
                result[table_first].push_back(f_data);
                result[table_second].push_back(s_data);
            }
        }
    }

    return default_res;
}

RecordData DataStore::copyRecord(Record* record)
{
    if(record == nullptr) { return RecordData();}

    RecordData copy;

    // TODO: Is this dereference safe?
    int64_t record_len = record->size(0);

    for(int64_t i = 0; i < record_len; i++)
    {
        TervelData data = {.value = 0};

        int64_t tervel_data = 0;

	    // Pull the current value of the data from the record
	    while(!record->at(i, tervel_data))
	    {
            printf("Waiting for retrieval\n");
        }

        data.value = tervel_data;

        printf("Retrieved value: %ld\n", tervel_data);

	    // TODO: This is ugly af, rethink this naming scheme in Data/TervelData
	    copy.push_back(data);
    }

    // Return the copied data
    return copy;
}
