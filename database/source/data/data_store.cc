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

/**
 *  \brief Creates and initializes a table with the information given in
 *  the command
 */
ManipResult DataStore::createTable(CreateTableCommand table_info)
{
    if(table_info.columns.size() <= 0)
    {
        return ManipResult(ResultStatus::FAILURE,
                ManipStatus::ERR_TABLE_CMD_INVALID);
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
 *  \brief Delete the table from the mapping and clean up the memory
 *  appropriately
 */
ManipResult DataStore::deleteTable(std::string table_name)
{
    //find table in table_mapping
    SchemaTablePair* table_pair = getTablePair(table_name);
    if(table_pair == nullptr)
    {
        return ManipResult(ResultStatus::FAILURE,
                ManipStatus::ERR_TABLE_NOT_EXIST);
    }

    // Remove the table from the table name map
    // TODO: Have Tervel return a true return code rather than a boolean
    if(!table_name_mapping.remove(table_name))
    {
        return ManipResult(ResultStatus::FAILURE,
                ManipStatus::ERR_TABLE_NOT_EXIST);
    }

    TableSchema *schema = table_pair->schema;

    // Delete the schema
    delete schema;

    // Delete the pair
    delete table_pair;

    // We're going to rely on the shared_ptr within SchemaTablePair
    // to delete the data table. The row-by-row delete will be done within the
    // DataTable destructor.

    return ManipResult(ResultStatus::SUCCESS, ManipStatus::SUCCESS);
}

/**
 *  \brief Returns the schema associated with the given table name.
 */
SchemaResult DataStore::getTableSchema(std::string table_name)
{
    SchemaTablePair* pair = getTablePair(table_name);
    if(pair == nullptr)
    {
        return SchemaResult(ResultStatus::FAILURE, TableSchema());
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

        return UintResult(ResultStatus::FAILURE, 0);
    }

    // Table name doesn't exist in the table name map
    return UintResult(ResultStatus::FAILURE, 0);
}

//TODO: Clean up some testing code, maybe some bounds checking beforehand
//for these loops, error checking too checks if the row data matches with the table schema
ConstraintResult DataStore::schemaChecker(SchemaTablePair *table_pair, Record *insert_row)
{
    //table, schema and copy of row being inserted
    std::shared_ptr<DataTable> table = table_pair->table;
    TableSchema *schema = table_pair->schema;
    RecordCopy copy_row = copyRecord(insert_row); //may need record copy
    RecordData row = copy_row.data;

    //number of columns, number of inputs from row, number of constraints in each column
    int64_t col_len = schema->columns.size();
    int64_t row_len = row.size();
    int64_t num_Constraints = 0;
    SQLConstraint constraint;

    //TESTING MUST DELETE: Hardcode schema constraint to test
    for(int64_t itr = 0; itr < col_len; itr++)
    {
        //hardcode constraint
        SQLConstraintType state = SQLConstraintType::SQL_DEFAULT;
        constraint.type = state;
        constraint.value.value = 10;

        //add constraints to the schema
        schema->columns.at(itr).constraint.push_back(constraint);
    }

    if(row_len-1 != col_len)
    {
        //mismatching columns not enough data
        return ConstraintResult(ResultStatus::SUCCESS, ConstraintStatus::ERR_ROW_LEN);
    }

    //copies a value from record, checks the schema column constraint at that index and
    //checks if the value is coherent with the column constraint
    //row-1 because of the record counter stored there
    for(int64_t i = 0; i < row_len-1; i++)
    {
        TervelData data = {.value = 0};
        data.value = row.at(i).value;
        data.data.tervel_status = 0; //TODO: probably not needed
        num_Constraints = schema->columns.at(i).constraint.size();

        for(int64_t j = 0; j < num_Constraints; j++)
        {
            constraint = schema->columns.at(i).constraint.at(j);
            switch(constraint.type)
            {
                case SQLConstraintType::SQL_NO_CONSTRAINT:
                    {
                        break;
                    }

                case SQLConstraintType::SQL_NOT_NULL:
                    {
                        if(data.data.null == 1)
                        {
                            return ConstraintResult(ResultStatus::FAILURE, ConstraintStatus::ERR_NULL);
                        }

                        break;
                    }

                case SQLConstraintType::SQL_AUTO_INCREMENT:
                    {
                        //grab last value from end of table
                        int64_t table_data;
                        Record *table_row;
                        int64_t table_size = table.get()->records.size(0);
                        while(!table->records.at(table_size, table_row)) {}
                        while(!table_row->at(i, table_data)) {}

                        //increment the table data by 1 and overwrite the data.value to reflect this
                        data.value = table_data + 1;
                        break;
                    }
                case SQLConstraintType::SQL_DEFAULT:
                    {
                        /*
                        if(data.value != constraint.value.value)
                        {
                            return ConstraintResult(ResultStatus::FAILURE, ConstraintStatus::ERR_DEFAULT);
                        }
                        */

                        //overwrites row value to be default value
                        data.value = constraint.value.value;
                        break;
                    }
/*
 * TODO: Not thread safe because of long table scans
                case SQLConstraintType::SQL_PRIMARY_KEY:
                    {
                        int counter = 0; //TODO: Should be a flag

                        //check if PRIMARY KEY is the only constraint in schema
                        //then it will follow into same behavior as unique
                        for(int64_t m = 0; m < col_len; m++)
                        {
                            if(schema->columns.at(m).constraint.at(j).type == SQLConstraintType::SQL_PRIMARY_KEY)
                                counter++;
                            if(counter > 1)
                            {
                                return ConstraintResult(ResultStatus::FAILURE, ConstraintStatus::ERR_NOT_PKEY);
                                //printf("error two primary keys \n");
                                //break;
                            }
                        }

                        //continues on to unique behavior
                    }


                    //TODO: A snapshot of the table may be necessary since data can be inserted by other
                    //threads while this unique check is performing a table scan...
                    //TODO: Or create a master-slave thread where threads insert into db without a table scan
                    //and this master will clean up any mistakes... having each thread check constraints on a
                    //snapshot ... does not make much sense
                case SQLConstraintType::SQL_UNIQUE:
                    {
                        //tables should be record here? tables can change while insert is being evaluated
                        int64_t size = table->records.size(0);
                        Record *table_row;
                        int64_t table_data;

                        //perform a table scan and check if any column data matches the row data
                        for(int64_t k = 0; k < size; k++)
                        {
                            while(!table->records.at(k, table_row)) {}
                            while(!table_row->at(i, table_data))
                                if(table_data == data.value)
                                {
                                    return ConstraintResult(ResultStatus::FAILURE, ConstraintStatus::ERR_NOT_UNIQUE);
                                }
                        }
                        break;
                    }
*/
            }
        }
    }

    return ConstraintResult(ResultStatus::SUCCESS, ConstraintStatus::SUCCESS);
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
        return ManipResult(ResultStatus::FAILURE, ManipStatus::ERR_TABLE_NOT_EXIST);
    }

    // Insert into the table
    Record* new_record = new (std::nothrow) Record(record.size());
    if(new_record == nullptr)
    {
        return ManipResult(ResultStatus::FAILURE, ManipStatus::ERR_NO_MEMORY);
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
    if(schemaChecker(table_pair, new_record).status == ResultStatus::SUCCESS)
    {
        size_t ret = table_pair->table->records.push_back_w_ra(new_record);
        printf("Pushback returned %lu\n", ret);
        printf("Record address %p\n", new_record);
    }
    else
    {
        return ManipResult(ResultStatus::FAILURE, ManipStatus::ERR_FAILED_CONSTRAINT);
    }

    return ManipResult(ResultStatus::SUCCESS, ManipStatus::SUCCESS);
}

/**
 *	\brief Update the appropriate record(s) with the given \refer RecordData object
 */
ManipResult DataStore::updateRecords(Predicate* predicates,
        std::string table_name,
        RecordData record)
{
}

/**
 *  \brief Delete the appropriate records that satisfy the given predicate
 */
ManipResult DataStore::deleteRecords(Predicate* predicates, std::string table_name)
{
    if(predicates == nullptr)
    {
        // TODO: Allow a full delete?
    }
    else
    {
        RecordReferences record_refs;
        switch(predicates->type)
        {
            case PredicateType::NESTED:
                record_refs = searchTablesForRefs(predicates);
                break;
            case PredicateType::VALUE:
                break;
            default:
                // How the hell did you get here?
                // std::terminate()?
                break;
        }
    }
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
        return MultiRecordResult(ResultStatus::FAILURE, MultiRecordData());
    }

    auto table = table_pair->table;
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
            while(!records.at(i, row)) {}

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

    return MultiRecordResult(ResultStatus::FAILURE, MultiRecordData());
}

/**
 *  \brief Gets the \refer SchemaTablePair object associated to the table name given
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
MultiRecordCopies DataStore::searchTable(std::shared_ptr<DataTable>& table,
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
 * TODO This only needs to be implemented if we support column-column compares
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

    // This compares two records and orders them based on their ID
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
 *	\brief Searches the given table for records that satisfies the given predicate 
 *	and records their ID
 */
RecordReferences DataStore::searchTableForRefs(std::shared_ptr<DataTable>& table,
        ValuePredicate* value_pred)
{
    RecordReferences data;
    if(table == nullptr || value_pred == nullptr)
    {
        return RecordReferences();
    }

    // This assumes table maps to the table name in the value predicate
    int64_t table_len = table->records.size(0);
    if(table_len == 0)
    {
        return RecordReferences();
    }

    for(int64_t idx = 0; idx < table_len; idx++)
    {
        Record* row = nullptr;

        while(!table->records.at(idx, row)) {}

        // TODO: Consider making this a more focused grab of data
        RecordCopy record = copyRecord(row);

        if(record.data.size() <= value_pred->column.column_idx)
        {
            // TODO: Propagate error up? This should be caught earlier
            return RecordReferences();
        }

        ExpressionValue row_value(record.data.at(value_pred->column.column_idx));

        if(evaluateOperation(value_pred->op, 
                    value_pred->expected_value,
                    row_value).IsTrue())
        {
            printf("Record matches predicate!\n");
            data.push_back(RecordReference(record.id, row));
        }
    }

    return data;
}

RecordReferences DataStore::searchTablesForRefs(NestedPredicate* pred)
{
    auto result = RecordReferences();
    auto left_result = RecordReferences();
    auto right_result = RecordReferences();

    // This assumes the nested predicate given has two children
    Predicate* left = pred->left_child; 
    switch(left->type)
    {
        case PredicateType::NESTED:
            {
                auto nest_pred = reinterpret_cast<NestedPredicate*>(left);
                left_result = searchTablesForRefs(nest_pred);
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
                return RecordReferences();
            }
            break;
        case PredicateType::VALUE:
            {
                ValuePredicate* val_pred = reinterpret_cast<ValuePredicate*>(left);
                auto table_pair = getTablePair(val_pred->column.table);
                left_result = searchTableForRefs(table_pair->table, val_pred);
            }
            break;
    }

    Predicate* right = pred->right_child;
    switch(right->type)
    {
        case PredicateType::NESTED:
            {
                auto nest_pred = reinterpret_cast<NestedPredicate*>(right);
                right_result = searchTablesForRefs(nest_pred);
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
                return RecordReferences();
            }
            break;
        case PredicateType::VALUE:
            {
                ValuePredicate* val_pred = reinterpret_cast<ValuePredicate*>(right);
                auto table_pair = getTablePair(val_pred->column.table);
                right_result = searchTableForRefs(table_pair->table, val_pred);
            }
            break;
    }

    // Join the two results based on the nested predicate operation

    // This compares two records and orders them based on their ID
    auto comp_copy = [] (RecordReference a, RecordReference b) -> bool {
        return a.id < b.id;
    };

    size_t l_size = left_result.size();
    size_t r_size = right_result.size();

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

    auto join = RecordReferences(num_join_records);

    // The ranges have to be sorted
    std::sort(left_result.begin(), left_result.end(), comp_copy);
    std::sort(right_result.begin(), right_result.end(), comp_copy);

    RecordReferences::iterator join_end_itr;

    // Perform the appropriate set operation
    switch(pred->op)
    {
        case ExpressionOperation::AND:
            join_end_itr = std::set_intersection(left_result.begin(), left_result.end(),
                    right_result.begin(), right_result.end(),
                    join.begin(), comp_copy);
            break;
        case ExpressionOperation::OR:
            join_end_itr = std::set_union(left_result.begin(), left_result.end(),
                    right_result.begin(), right_result.end(),
                    join.begin(), comp_copy);
            break;
        default:
            printf("Invalid operation used to join sets!\n");
            break;
    }

    // Resize the vector to chop off unused elements
    join.resize(join_end_itr - join.begin());
    result = join;

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
