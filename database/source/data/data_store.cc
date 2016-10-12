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

    KeyHashFunctor hash_functor;

    TableSchema* schema = new (std::nothrow) TableSchema;
    if(schema == nullptr)
    {
        return ManipResult(ResultStatus::FAILURE, ManipStatus::ERR_NO_MEMORY);
    }

    // Copy the column information into the stored schema
    schema->columns = table_info.columns;

    DataTable* new_table = new (std::nothrow) DataTable();
    if(new_table == nullptr)
    {
        return ManipResult(ResultStatus::FAILURE, ManipStatus::ERR_NO_MEMORY);
    }

    SchemaTablePair* pair = new (std::nothrow) SchemaTablePair(new_table, schema);
    if(pair == nullptr)
    {
        return ManipResult(ResultStatus::FAILURE, ManipStatus::ERR_NO_MEMORY);
    }

    size_t key = hash_functor(table_info.table_name);

    // Attempt to insert into the hash table
    if(!table_name_mapping.insert( key, pair))
    {
        return ManipResult(ResultStatus::FAILURE, ManipStatus::ERR);
    }

    ManipResult result(ResultStatus::SUCCESS, ManipStatus::SUCCESS);
    result.rows_affected = 0;

    return result;
}

/**
 *  \brief Delete the table from the mapping and clean up the memory
 *  appropriately
 */
ManipResult DataStore::deleteTable(const std::string& table_name)
{
    KeyHashFunctor hasher;
    size_t key = hasher(table_name);
    // Find table in table_mapping
    SchemaTablePair* pair_ptr = nullptr;
    {
        // We need this extra scope to make sure the accessor destructs
        TableMap::ValueAccessor hash_accessor;
        if(table_name_mapping.at(key, hash_accessor))
        {
            if(hash_accessor.valid())
            {
                // TODO: Is this safe? I think so, but ...
                pair_ptr = (*hash_accessor.value());
            }
        }
    }

    if(pair_ptr == nullptr)
    {
        return ManipResult(ResultStatus::FAILURE,
                ManipStatus::ERR_TABLE_NOT_EXIST);
    }

    // Remove the table from the table name map
    // TODO: Have Tervel return a true return code rather than a boolean
    while(!table_name_mapping.remove(key))
    {
        /*
        return ManipResult(ResultStatus::FAILURE,
                ManipStatus::ERR_TABLE_NOT_EXIST);
        */
    }

    TableSchema *schema = pair_ptr->schema;

    // Delete the schema
    delete schema;

    // Delete the pair
    delete pair_ptr;

    // We're going to rely on the shared_ptr within SchemaTablePair
    // to delete the data table. The row-by-row delete will be done within the
    // DataTable destructor. Any call to getTablePair adds an owner to the shared
    // pointer, which increases its ref-count. Deleting this pair allows for the 
    // ref-count to hit zero, which triggers a delete.

    // TODO: Calculate the rows deleted due to the drop

    return ManipResult(ResultStatus::SUCCESS, ManipStatus::SUCCESS);
}

/**
 *  \brief Returns the schema associated with the given table name.
 */
SchemaResult DataStore::getTableSchema(const std::string& table_name)
{
    SchemaTablePair pair;
    if(!getTablePair(table_name, pair))
    {
        return SchemaResult(ResultStatus::FAILURE, TableSchema());
    }
    else
    {
        TableSchema schema = *pair.schema;
        return SchemaResult(ResultStatus::SUCCESS, schema);
    }
}

/**
 *	\brief Returns the column index of the given column in the table
 */
UintResult DataStore::getColumnIndex(const std::string& table_name, const std::string& column_name)
{
    SchemaTablePair table_pair;
    if(getTablePair(table_name, table_pair))
    {
        // This should be safe, we're only reading from the object
        size_t num_columns = table_pair.schema->columns.size();
        for(uint32_t itr = 0; itr < num_columns; itr++)
        {
            if(table_pair.schema->columns[itr].name == column_name)
            {
                return UintResult(ResultStatus::SUCCESS, itr);
            }
        }

        return UintResult(ResultStatus::FAILURE, 0);
    }

    // Table name doesn't exist in the table name map
    return UintResult(ResultStatus::FAILURE, 0);
}


//Checks if the inserted row is valid for those constraints
ConstraintResult DataStore::schemaChecker(SchemaTablePair& table_pair, RecordData *row)
{
    //table, schema
    std::shared_ptr<DataTable>& table = table_pair.table;
    TableSchema *schema = table_pair.schema;

    //number of columns, number of inputs from row, number of constraints in each column
    int64_t col_len = schema->columns.size();
    int64_t row_len = row->size();
    int64_t num_Constraints = 0;
    SQLConstraint constraint;

    if(row_len != col_len)
    {
        //mismatching columns not enough data
        return ConstraintResult(ResultStatus::FAILURE, ConstraintStatus::ERR_ROW_LEN);
    }

    //finds the column constraints and check each column in row to see if any apply
    for(int64_t i = 0; i < row_len; i++)
    {
        TervelData row_data = {.value = 0};
        row_data = row->at(i);
        num_Constraints = schema->columns.at(i).constraint.size();

        for(int64_t j = 0; j < num_Constraints; j++)
        {
            constraint = schema->columns.at(i).constraint.at(j);
            switch(constraint.type)
            {
                //do nothing
                case SQLConstraintType::NO_CONSTRAINT:
                    {
                        break;
                    }
                //data cannot be null
                case SQLConstraintType::NOT_NULL:
                    {
                        if(row_data.null == 1)
                        {
                            return ConstraintResult(ResultStatus::FAILURE, ConstraintStatus::ERR_NULL);
                        }

                        break;
                    }
                //increment unique counter that represents number of records inserted into table
                case SQLConstraintType::AUTO_INCREMENT:
                    {
                        TervelData insert = {.value = 0};
                        insert.type = INTEGER;
                        insert.data_value = table->record_counter.load()+1;
                        insert.tervel_status = 0;

                        row->at(i) = insert;
                        break;
                    }
                case SQLConstraintType::DEFAULT:
                    {
                        if(row_data.null == 1)
                        {
                            row->at(i) = constraint.value;
                        }
                        break;
                    }
            }
        }
    }

    return ConstraintResult(ResultStatus::SUCCESS, ConstraintStatus::SUCCESS);
}

/**
 *	\brief Inserts a record into the given data table, if it exists.
 */
ManipResult DataStore::insertRecord(const std::string& table_name, RecordData record)
{
    // Get the table
    SchemaTablePair table_pair;
    if(!getTablePair(table_name, table_pair))
    {
        return ManipResult(ResultStatus::FAILURE, ManipStatus::ERR_TABLE_NOT_EXIST);
    }

    if(schemaChecker(table_pair, &record).status != ResultStatus::SUCCESS)
    {
        return ManipResult(ResultStatus::FAILURE, ManipStatus::ERR_FAILED_CONSTRAINT);
    }

    // Insert into the table
    Record* new_record = new (std::nothrow) Record(record.size());
    if(new_record == nullptr)
    {
        return ManipResult(ResultStatus::FAILURE, ManipStatus::ERR_NO_MEMORY);
    }

    // These inserts should never fail, the tervel data structure isn't shared yet
    for(auto data : record)
    {
        TervelData terv_data = { .value = 0 };

        // Ensure Tervel's bits are zeroed and copy the rest of the data over
        terv_data.value = data.value;
        terv_data.tervel_status = 0;

        new_record->push_back_w_ra(terv_data.value);
    }

    // Push the record counter on the back of the record
    TervelData counter_data = {.value = 0};
    uint64_t counter = table_pair.table->record_counter.fetch_add(1);
    
    counter_data.data_value = counter;
    new_record->push_back_w_ra(counter_data.value);

    ValuePointer<Record>* record_ptr = new ValuePointer<Record>(new_record);

    table_pair.table->records.push_back_w_ra(record_ptr);

    return ManipResult(ResultStatus::SUCCESS, ManipStatus::SUCCESS);
}

/**
 *	\brief Update the appropriate record(s) with the given \refer RecordData object
 */
ManipResult DataStore::updateRecords(Predicate* predicates,
        const std::string& table_name,
        RecordData record)
{
    // Denotes whether there's been some contention in the update
    bool some_contention = false;

    if(predicates == nullptr)
    {
        // TODO: Allow a full update?
        printf("%s: Attempting a full table update which is currently not supported!\n", __FUNCTION__);
    }
    else
    {
        SchemaTablePair pair;
        if(!getTablePair(table_name, pair))
        {
            return ManipResult(ResultStatus::FAILURE, ManipStatus::ERR_TABLE_NOT_EXIST);
        }

        RecordReferences record_refs;
        switch(predicates->type)
        {
            case PredicateType::NESTED:
                record_refs = searchTablesForRefs(reinterpret_cast<NestedPredicate*>(predicates));
                break;
            case PredicateType::VALUE:
            {
                record_refs = searchTableForRefs(pair.table, reinterpret_cast<ValuePredicate*>(predicates));
            }
                break;
            default:
                // How the hell did you get here?
                // std::terminate()?
                break;
        }

        if(record_refs.size() == 0)
        {
            ManipResult result(ResultStatus::SUCCESS, ManipStatus::SUCCESS);
            // There just isn't anything to update
            result.rows_affected = 0;
            return result;
        }

        uint64_t num_affected = 0;
        // Attempt to swap the various references with the calculated record
        for(auto old_record : record_refs)
        {
            int64_t table_len = pair.table->records.size();
            for(int64_t idx = 0; idx < table_len; idx++)
            {
                VectorAccessor<Record> accessor;
                if(!accessor.init(pair.table->records, idx))
                {
                    // Failed to initialize the accessor.
                    // Not sure what to do here, I think it best would be to 
                    // continue on and try the rest of the table.
                    continue;
                }

                // TODO: Add a ID check?
                if(old_record.ptr != accessor.value)
                {
                    continue;
                }

                RecordCopy copy = copyRecord(accessor.value->ptr);

                // Perform the update
                Record* updated_record = updateRecord(copy, record); 
                if(updated_record == nullptr)
                {
                    // TODO: Error handling
                }

                // Now perform the CAS with the new record
                ValuePointer<Record>* new_record_ptr = new ValuePointer<Record>(updated_record);
                // TODO: Make sure the old record's pointer isn't modified in cas
                if(!pair.table->records.cas(idx, old_record.ptr, new_record_ptr))
                {
                    // CAS failed, contention on the element
                    some_contention = true;
                }
                else
                {
                    ++num_affected;
                }
            }
        }

        ManipResult result(ResultStatus::SUCCESS, ManipStatus::SUCCESS);
        if(some_contention)
        {
            result.result = ManipStatus::ERR_PARTIAL_CONTENTION;
        }

        result.rows_affected = num_affected;
        return result;
    }

    // General error, this should never be reached
    return ManipResult(ResultStatus::FAILURE, ManipStatus::ERR);
}

/**
 *  \brief Delete the appropriate records that satisfy the given predicate
 */
ManipResult DataStore::deleteRecords(Predicate* predicates, const std::string& table_name)
{
    bool some_contention = false;
    uint64_t num_affected = 0;

    if(predicates == nullptr)
    {
        // TODO: Allow a full delete?
        printf("%s: Attempting a full table delete which is currently not supported!\n", __FUNCTION__);
    }
    else
    {
        SchemaTablePair pair;
        if(!getTablePair(table_name, pair))
        {
            return ManipResult(ResultStatus::FAILURE, ManipStatus::ERR_TABLE_NOT_EXIST);
        }

        RecordReferences record_refs;
        switch(predicates->type)
        {
            case PredicateType::NESTED:
                record_refs = searchTablesForRefs(reinterpret_cast<NestedPredicate*>(predicates));
                break;
            case PredicateType::VALUE:
            {
                record_refs = searchTableForRefs(pair.table, reinterpret_cast<ValuePredicate*>(predicates));
            }
                break;
            default:
                // How the hell did you get here?
                // std::terminate()?
                break;
        }

        // Go through the collected record references and attempt to delete them
        for(auto old_record : record_refs)
        {
            int64_t table_len = pair.table->records.size();
            for(int64_t idx = 0; idx < table_len; idx++)
            {
                VectorAccessor<Record> accessor;
                if(!accessor.init(pair.table->records, idx))
                {
                    // Failed to initialize the accessor.
                    // Not sure what to do here, I think it best would be to 
                    // continue on and try the rest of the table.
                    continue;
                }

                // TODO: Add a ID check?
                if(old_record.ptr != accessor.value)
                {
                    continue;
                }

                ValuePointer<Record>* null_val_ptr = nullptr;
                if(!pair.table->records.cas(idx, old_record.ptr, null_val_ptr))
                {
                    // CAS failed, contention on the element
                    some_contention = true;
                }
                else
                {
                    ++num_affected;
                }

                if(!pair.table->records.eraseAt(idx, null_val_ptr))
                {
                    // Insert underneath me may have happened, but 
                    // that shouldn't happen since we only push_back or pop_back.
                    // This really shouldn't be reached
                    some_contention = true;
                }
            }
        }

        ManipResult result(ResultStatus::SUCCESS, ManipStatus::SUCCESS);
        if(some_contention)
        {
            result.result = ManipStatus::ERR_PARTIAL_CONTENTION;
        }

        result.rows_affected = num_affected;
        return result;
    }

    // General error, this should never be reached
    return ManipResult(ResultStatus::FAILURE, ManipStatus::ERR);
}

/**
 *	\brief Retrieves the appropriate record(s) based on the predicates given
 */
MultiRecordResult DataStore::getRecords(Predicate* predicates,
        const std::string& table_name)
{
    // Get the table pair
    SchemaTablePair table_pair;
    if(!getTablePair(table_name, table_pair))
    {
        return MultiRecordResult(ResultStatus::FAILURE, MultiRecordData());
    }

    auto table = table_pair.table;
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
            return MultiRecordResult(ResultStatus::SUCCESS, data);
        }

        for(int64_t i = 0; i < table_len; i++)
        {
            // Get the current row pointer
            Record* row_ptr = nullptr;

            VectorAccessor<Record> accessor;
            if(accessor.init(table->records, i))
            {
                row_ptr = accessor.value->ptr;

                RecordCopy record_copy = copyRecord(row_ptr);
                data.push_back(record_copy.data);
            }
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
                        MultiRecordCopies copies = searchTable(table_pair.table,
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
bool DataStore::getTablePair(std::string table_name, SchemaTablePair& pair)
{
    std::hash<std::string> hash_functor;
    TableMap::ValueAccessor hash_accessor;

    size_t key = hash_functor(table_name);
    if(table_name_mapping.at(key, hash_accessor))
    {
        if(hash_accessor.valid())
        {
            // TODO: Is this safe? I think so, but ...
            pair = *(*hash_accessor.value());
            return true;
        }
    }

    return false;
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
        Record* row_ptr = nullptr;

        VectorAccessor<Record> accessor;
        if(accessor.init(table->records, idx))
        {
            row_ptr = accessor.value->ptr;

            RecordCopy record = copyRecord(row_ptr);

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
                data.push_back(record);
            }
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
                SchemaTablePair table_pair;
                // Assume this succeeds
                getTablePair(val_pred->column.table, table_pair);
                auto value_result = searchTable(table_pair.table, val_pred);
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
                SchemaTablePair table_pair;
                // Assume this succeeds
                getTablePair(val_pred->column.table, table_pair);
                auto value_result = searchTable(table_pair.table, val_pred);
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
        Record* row_ptr = nullptr;

        VectorAccessor<Record> accessor;
        if(accessor.init(table->records, idx))
        {
            row_ptr = accessor.value->ptr;

            // TODO: Consider making this a more focused grab of data
            RecordCopy record = copyRecord(row_ptr);

            // Grabbing the record failed for whatever reason
            if(record.data.size() == 0) { continue; }

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
                data.push_back(RecordReference(record.id, accessor.value));
            }
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
                SchemaTablePair table_pair;
                getTablePair(val_pred->column.table, table_pair);
                left_result = searchTableForRefs(table_pair.table, val_pred);
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
                SchemaTablePair table_pair;

                getTablePair(val_pred->column.table, table_pair);
                right_result = searchTableForRefs(table_pair.table, val_pred);
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

    // TODO: Any gotchas here?
    VectorAccessor<Record> accessor;
    if(accessor.init(table, row_idx))
    {
        return copyRecord(record);
    }
    else
    {
        return RecordCopy();
    }

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

        // Clear out any Tervel status bits so as to make later comparisons trivial
        data.tervel_status = 0;
        copy.push_back(data);
    }

    RecordCopy record_copy(std::move(copy));

    // Return the copied data
    return record_copy;
}

/**
 *  \brief Creates an updated copy of a passed-in record
 */
Record* DataStore::updateRecord(const RecordCopy& old_record, RecordData& new_record)
{
    // Perform the update
    int64_t idx = 0;
    TervelData old_data = { .value = 0 };

    Record* updated_record = new (std::nothrow) Record;
    if(updated_record == nullptr)
    {
        return nullptr;
    }

    for(auto new_data : new_record)
    {
        // Grab the old data from the copy
        old_data = old_record.data.at(idx);

        // Reset the Tervel status bits for insertion into a new record (if needed)
        old_data.tervel_status = 0;

        // If the new data for this column is null,
        // then we need to keep the old data,
        // else bring in the new.
        if(new_data.null)
        {
            updated_record->push_back_w_ra(old_data.value);
        }
        else
        {
            updated_record->push_back_w_ra(new_data.value);
        }
        ++idx;
    }

    updated_record->push_back_w_ra(old_record.id);

    return updated_record;
}
