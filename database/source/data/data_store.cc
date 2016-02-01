/* Copyright (c) 2016 Neil Moore, Jason Stavrinaky, Micheal McGee, Robert Medina
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this 
 * software and associated documentation files (the "Software"), to deal in the Software 
 * without restriction, including without limitation the rights to use, copy, modify, 
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and to 
 * permit persons to whom the Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies 
 * or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE 
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE 
 * USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

// C standard library includes
#include <cstdio>

// STL includes
#include <stack>

// Project includes
#include "data/data_store.h"

//TODO: Test schema checker for more constraints
//Checks if the column type matches the data type and constraint of the schema for that column
//Should return error codes depending on the conflict (probably ManipResult)
//think about this: parsing contraints, error codes, type of contraints, efficency of SQLColumn, memory
void DataStore::schemaChecker(TableSchema schema, std::vector<SQLColumn> column)
{
	//check data type
	//check constraint
}

// TODO: Doxygenify this documentation
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

/**
 * TODO
 * Must find table in table mapping
 * Find table and delete all rows
 * Must delete schema pair and then delete table from table_mapping
 */
ManipResult DataStore::deleteTable(std::string table_name)
{
    //find table in table_mapping
    SchemaTablePair* table_pair = getTablePair(table_name);
    if(table_pair == nullptr)
    {
        return ManipResult(ResultStatus::ERROR_INVALID_TABLE, ManipStatus::FAILURE);
    }

    DataTable *table = table_pair->table;
    TableSchema *schema = table_pair->schema;
    //delete table from pair
    //delete schema from pair
    //delete pair (schema and table)
    //delete table from table_mapping

    /*
    // Remove the table from the table name map
    // TODO: Perform proper memory clean-up after removal from mapping
    // TODO: What does access_counter != 0 mean?
    if(!table_name_mapping.remove(table_name))
    {
        // TODO: Update with appropriate ManipStatus value
        return ManipResult(ResultStatus::ERROR_INVALID_TABLE, ManipStatus::SUCCESS);
    }

    return ManipResult(ResultStatus::SUCCESS, ManipStatus::SUCCESS);
    */
}

/**
 *	\brief Returns the schema associated with the given table name.
 */
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

/**
 *	\brief Returns the column index of the given column in the table
 */
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

/**
 *	\brief Inserts a record into the given data table, if it exists.
 */
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

    // Push the record counter on the back of the record
    TervelData counter_data = {.value = 0};
    uint64_t counter = table_pair->table->record_counter.fetch_add(1);

    counter_data.data.value = counter;
    new_record->push_back_w_ra(counter_data.value);

    // TODO: Handle table constraints
    size_t ret = table_pair->table->records.push_back_w_ra(new_record);
    printf("Pushback returned %lu\n", ret);
    printf("Record address %p\n", new_record);

    return ManipResult(ResultStatus::SUCCESS, ManipStatus::SUCCESS);
}

/**
 *	\brief Update the appropriate record(s) with the given \refer RecordData object
 */
ManipResult DataStore::updateRecord(Predicate* predicates,
                         std::string table_name,
                         RecordData record)
{
}

/**
 *	\brief Retrieves the appropriate record(s) based on the predicates given
 */
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
    RecordVector& records = table->records;

    // Evaluate predicates and return all rows that satisfy the predicates'
    // conditions.
    //
    // If there are no predicates, do a full table copy.
    if(predicates == nullptr)
    {
        MultiRecordData data;

	    int64_t table_len = records.size(0);
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
	        while(!records.at(i, row))
	        {}

            printf("Retrieved record address: %p\n", row);

	        RecordCopy record_copy = copyRecord(row);
	        data.push_back(record_copy.data);
	    }

	    return MultiRecordResult(ResultStatus::SUCCESS, data);
    }
    else
    {
        switch(predicates->type)
        {
            case PredicateType::NESTED:
                {
                    NestedPredicate* nested_pred = reinterpret_cast<NestedPredicate*>(predicates);
                    // Keeping this able to handle multiple tables in case we decide
                    // to implement multiple-table queries
                    MultiTableRecordCopies copies = searchTables(nested_pred);

                    MultiTableRecordData data;
					for(auto itr : copies)
					{
						for(auto rec_itr : itr.second)
						{
							data[itr.first].push_back(rec_itr.data);
						}
					}

                    return MultiRecordResult(ResultStatus::SUCCESS, data.at(table_name));
                }
                break;
            case PredicateType::VALUE:
                {
                    ValuePredicate* val_pred = reinterpret_cast<ValuePredicate*>(predicates);
                    if(val_pred->column.table != table_name)
                    {
						// There's no data to return, give back an empty set
						return MultiRecordResult(ResultStatus::SUCCESS, MultiRecordData());
                    }
                    else
                    {
                        MultiRecordCopies copies = searchTable(table_pair->table,
                                                           val_pred);

						// Have to convert back into a multi record data
                        MultiRecordData data;
                        for(auto itr : copies)
                        {
                            data.push_back(itr.data);
                        }

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

    return MultiRecordResult(ResultStatus::ERROR_INVALID_TABLE, MultiRecordData());
}

/**
 *	\brief Gets the \refer SchemaTablePair object associated to the table name given
 */
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

/**
 *	\brief Searches the given table for records that satisfies the given predicate
 */
MultiRecordCopies DataStore::searchTable(DataTable* table,
                                  ValuePredicate* value_pred)
{
    MultiRecordCopies data;
    if(table == nullptr || value_pred == nullptr)
    {
        return MultiRecordCopies();
    }

    // This assumes table maps to the table name in the value predicate
    int64_t table_len = table->records.size(0);
    if(table_len == 0)
    {
        return MultiRecordCopies();
    }

    for(int64_t idx = 0; idx < table_len; idx++)
    {
        Record* row = nullptr;

        while(!table->records.at(idx, row)) {}

        RecordCopy record = copyRecord(row);

        if(record.data.size() <= value_pred->column.column_idx)
        {
            // TODO: Propagate error up? This should be caught earlier
            return MultiRecordCopies();
        }

        ExpressionValue row_value(record.data.at(value_pred->column.column_idx));

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

/**
 * TODO
 */
/*
MultiTableRecordData DataStore::searchTable(std::string table_first, std::string table_second,
        ColumnPredicate* col_pred)
{
    auto default_res = MultiTableRecordData();
    auto result = MultiTableRecordData();

    auto f_table_pair = getTablePair(table_first);
    auto s_table_pair = getTablePair(table_second);

    if(f_table_pair == nullptr || s_table_pair == nullptr || col_pred == nullptr)
    {
        return default_res;
    }

    RecordVector& f_table = f_table_pair->table->records;
    RecordVector& s_table = s_table_pair->table->records;

    int64_t first_len = f_table.size(0);
    int64_t second_len = s_table.size(0);
    if(first_len == 0 || second_len == 0)
    {
        return default_res;
    }

    for(int64_t first_idx = 0; first_idx < first_len; first_idx++)
    {
        RecordData f_data = copyRecord(f_table, first_idx);

        for(int64_t second_idx = 0; second_idx < second_len; second_idx++)
        {
            RecordData s_data = copyRecord(s_table, second_idx);

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
*/

/**
 *	\brief Searches the system tables for records that satisfy the
 *	given predicate tree.
 */
MultiTableRecordCopies DataStore::searchTables(NestedPredicate* pred)
{
	auto result = MultiTableRecordCopies();
    auto left_result = MultiTableRecordCopies();
    auto right_result = MultiTableRecordCopies();

	// This assumes the nested predicate given has two children
	Predicate* left = pred->left_child;
	switch(left->type)
	{
		case PredicateType::NESTED:
			{
				auto nest_pred = reinterpret_cast<NestedPredicate*>(left);
				left_result = searchTables(nest_pred);
			}
			break;
		case PredicateType::COLUMN:
			{
                /*
				ColumnPredicate* col_pred = reinterpret_cast<ColumnPredicate*>(left);
				auto column_result = searchTable(col_pred->left_column.table,
						col_pred->right_column.table, col_pred);
                        */
                printf("Column to column predicates are not supported!\n");
				return MultiTableRecordCopies();
			}
			break;
		case PredicateType::VALUE:
			{
				ValuePredicate* val_pred = reinterpret_cast<ValuePredicate*>(left);
				auto table_pair = getTablePair(val_pred->column.table);
				auto value_result = searchTable(table_pair->table, val_pred);
                left_result[val_pred->column.table] = value_result;
			}
			break;
	}

	Predicate* right = pred->right_child;
	switch(right->type)
	{
		case PredicateType::NESTED:
			{
				auto nest_pred = reinterpret_cast<NestedPredicate*>(right);
				right_result = searchTables(nest_pred);
			}
			break;
		case PredicateType::COLUMN:
			{
                /*
				ColumnPredicate* col_pred = reinterpret_cast<ColumnPredicate*>(right);
				auto column_result = searchTable(col_pred->left_column.table,
						col_pred->right_column.table, col_pred);
                        */
                printf("Column to column predicates are not supported!\n");
				return MultiTableRecordCopies();
			}
			break;
		case PredicateType::VALUE:
			{
				ValuePredicate* val_pred = reinterpret_cast<ValuePredicate*>(right);
				auto table_pair = getTablePair(val_pred->column.table);
				auto value_result = searchTable(table_pair->table, val_pred);
                right_result[val_pred->column.table] = value_result;
			}
			break;
	}

    // Join the two results based on the nested predicate operation
    auto comp_copy = [] (RecordCopy a, RecordCopy b) -> bool {
        return a.id < b.id;
    };

    for(auto l_itr : left_result)
    {
        for(auto r_itr : right_result)
        {
            if(r_itr.first != l_itr.first)
            {
                continue;
            }

			size_t l_size = l_itr.second.size();
			size_t r_size = r_itr.second.size();

			size_t num_join_records = 0;
			switch(pred->op)
			{
				case ExpressionOperation::AND:
					num_join_records = (l_size > r_size) ? l_size : r_size;
					break;
				case ExpressionOperation::OR:
				default:
					num_join_records = l_size + r_size;
					break;
			}

            auto join = MultiRecordCopies(num_join_records);

            // The ranges have to be sorted
            std::sort(l_itr.second.begin(), l_itr.second.end(), comp_copy);
            std::sort(r_itr.second.begin(), r_itr.second.end(), comp_copy);

			MultiRecordCopies::iterator join_end_itr;

            // Perform the appropriate set operation
            switch(pred->op)
            {
                case ExpressionOperation::AND:
                    join_end_itr = std::set_intersection(l_itr.second.begin(), l_itr.second.end(),
                            r_itr.second.begin(), r_itr.second.end(),
                            join.begin(), comp_copy);
                    break;
                case ExpressionOperation::OR:
                    join_end_itr = std::set_union(l_itr.second.begin(), l_itr.second.end(),
                            r_itr.second.begin(), r_itr.second.end(),
                            join.begin(), comp_copy);
                    break;
                default:
                    printf("Invalid operation used to join sets!\n");
                    break;
            }

			// Resize the vector to chop off unused elements
			join.resize(join_end_itr - join.begin());
			result[l_itr.first] = join;
        }
    }


	return result;
}

/**
 *	\brief Copy a specific row from a given table
 */
RecordCopy DataStore::copyRecord(RecordVector& table, int64_t row_idx)
{
	Record* record = nullptr;

	// TODO: Handle non-trivial failure case (e.g. row_idx > table->size())
	while(!table.at(row_idx, record)) {}

	return copyRecord(record);
}

/**
 *	\brief Copies the record into a local copy for manipulation and computation
 */
RecordCopy DataStore::copyRecord(Record* record)
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
	    while(!record->at(i, tervel_data)) {}

        data.value = tervel_data;

        printf("Retrieved value: %ld\n", tervel_data);

	    // TODO: This is ugly af, rethink this naming scheme in Data/TervelData
		// Clear out any Tervel status bits so as to make later comparisons trivial
		data.data.tervel_status = 0;
	    copy.push_back(data);
    }

    RecordCopy record_copy(std::move(copy));

    // Return the copied data
    return record_copy;
}
