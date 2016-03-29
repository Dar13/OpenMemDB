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

#include "sql/statements/builder.h"

#include "data/data_store.h"

#include <cstdio>

// Helper functions
static DataType getSQLType(std::string* type);

static Expression* createExpression(Token op, Token left, Token right);

void builderStartCreateTable(StatementBuilder* builder, Token table_name)
{
    CreateTableCommand* command = new CreateTableCommand;
    command->table_name = table_name->text;
  
    builder->started = true;
    builder->statement = command;
    builder->valid = true;
}

void builderStartDropTable(StatementBuilder* builder, Token table_name)
{
    DropTableCommand* command = new DropTableCommand;
    command->table_name = table_name->text;
  
    builder->started = true;
    builder->statement = command;
    builder->valid = true;
}

void builderAddColumnConstraint(StatementBuilder* builder, SQLConstraintType type,
        Token value) 
{
    assert(builder != nullptr);

    SQLConstraint constraint;
    constraint.type = type;
    
    if(value && value->is_value)
    {
        constraint.value = value->value;
    }

    builder->temp_constraints.push_back(constraint);
}

void builderStartSelectQuery(StatementBuilder* builder)
{
    SelectQuery* query = new (std::nothrow) SelectQuery();
    if(query == nullptr)
    {
        // TODO: Error handling
    }
  
    builder->started = true;
    builder->statement = query;
    builder->valid = true;
}

void builderStartInsertCommand(StatementBuilder* builder)
{
    InsertCommand* cmd = new (std::nothrow) InsertCommand();
    // TODO: Error handling

    builder->started = true;
    builder->statement = cmd;
    builder->valid = true;
}

void builderStartUpdateCommand(StatementBuilder* builder)
{
    UpdateCommand* cmd = new (std::nothrow) UpdateCommand();

    builder->started = true;
    builder->statement = cmd;
    builder->valid = true;
}

void builderStartDeleteCommand(StatementBuilder* builder)
{
    DeleteCommand* cmd = new (std::nothrow) DeleteCommand();

    builder->started = true;
    builder->statement = cmd;
    builder->valid = true;
}

// SELECT helper functions ////////////////////////////////////////////////////
void builderAddSelectAllColumns(StatementBuilder* builder, Token table)
{
    // Don't want to override a currently active statement
    if(!builder->started)
    {
        builderStartSelectQuery(builder);
    }

    if(builder->statement == nullptr)
    {
        // TODO: Error handling
    }

    // TODO: Add all columns in the given table to this query
    switch(builder->statement->type)
    {
        case SQLStatement::SELECT:
            break;
        default:
            // TODO: Error handling?
            break;
    }
}

void builderAddQualifiedSelectColumn(StatementBuilder* builder,
                                     Token table, Token source_column,
                                     Token output_column)
{
    // This means the output column should be named the same as the 
    // source column
    if(output_column == nullptr)
    {
        output_column = source_column;
    }
  
    if(!builder->started)
    {
        // Start the SELECT statement then
        builderStartSelectQuery(builder);
    }
    else
    {
        // TODO: Error handling
    }

    ColumnReference source;
    source.table = table->text;

    auto res = builder->data_store->getColumnIndex(source.table, source_column->text);
    if(res.status == ResultStatus::SUCCESS)
    {
        source.column_idx = res.result;
    }
    else
    {
        // TODO: Error handling
        printf("%s: Error encountered finding column index! Error code = %d\n",
                __FUNCTION__, res.status);
    }
  
    SelectQuery* query = reinterpret_cast<SelectQuery*>(builder->statement);
  
    // Push the previously calculated information into the query object
    query->source_columns.push_back(source);

    query->output_columns.push_back(output_column->text);
}

void builderFinishSelectQuery(StatementBuilder* builder)
{
    if(!builder->started || builder->statement->type != SQLStatement::SELECT)
    {
        return;
    }

    SelectQuery* query = reinterpret_cast<SelectQuery*>(builder->statement);

    // Push all unique tables into the tables vector in the query.
    for(auto col_ref : query->source_columns)
    {
        if(std::find(query->tables.begin(), query->tables.end(), col_ref.table) == query->tables.end())
        {
            query->tables.push_back(col_ref.table);
        }
    }
}

// Insert command helper functions ////////////////////////////////////////////

void builderAddDataItem(StatementBuilder* builder, Token data)
{
    if(data->is_value && builder->started)
    {
        InsertCommand* cmd = reinterpret_cast<InsertCommand*>(builder->statement);

        cmd->data.push_back(data->value);
    }
}

void builderFinishInsertCommand(StatementBuilder* builder)
{
}

// Update command helper functions ////////////////////////////////////////////

void builderAddUpdateExpr(StatementBuilder* builder, Token operation,
        Token left, Token right)
{
    if(!builder->started)
    {
        return;
    }

    if(!operation->is_operation || 
        operation->operation != static_cast<uint16_t>(ExpressionOperation::EQUALS))
    {
        printf("Invalid expression for UPDATE statements!\n");
        return;
    }

    UpdateCommand* update = reinterpret_cast<UpdateCommand*>(builder->statement);
    ColumnUpdate update_info;

    SchemaResult schema_res = builder->data_store->getTableSchema(update->table);
    if(schema_res.status != ResultStatus::SUCCESS)
    {
        // TODO: Table doesn't exist
        printf("%s: Unable to retrieve table schema! Handle this TODO!\n", __FUNCTION__);
    }

    update->table_schema = schema_res.result;

    auto column_idx_result = builder->data_store->getColumnIndex(update->table, left->text);
    if(column_idx_result.status == ResultStatus::SUCCESS)
    {
        update_info.column_idx = column_idx_result.result;
    }
    else
    {
        // Column doesn't exist
        // TODO: Error handling
    }

    if(right->is_value)
    {
        update_info.new_data = right->value;
    }
    else
    {
        // Not a value
        // TODO: Error handling
    }

    update->columns.push_back(update_info);
}

void builderFinishUpdateCommand(StatementBuilder* builder)
{
    if(builder->started && builder->valid)
    {
        UpdateCommand* update_cmd = reinterpret_cast<UpdateCommand*>(builder->statement);

        if(update_cmd->table_schema.columns.size() == 0)
        {
            // Attempt to get the table schema one more time
            SchemaResult schema_check = builder->data_store->getTableSchema(update_cmd->table);
            if(schema_check.status != ResultStatus::SUCCESS)
            {
                update_cmd->runnable = false;
                // No point going any further
                return;
            }
            else
            {
                update_cmd->table_schema = schema_check.result;
            }
        }

        TervelData null_data = { .value = 0 };
        null_data.data.null = 1;

        update_cmd->full_record.resize(update_cmd->table_schema.columns.size(), null_data);
        for(auto updated_col : update_cmd->columns)
        {
            update_cmd->full_record[updated_col.column_idx] = updated_col.new_data;
        }

        update_cmd->runnable = true;
    }
}

// Generic-ish helper functions ///////////////////////////////////////////////

void builderAddColumn(StatementBuilder* builder, Token column_name,
                      Token column_type, Token column_constraints)
{
    //printf("Entered builderAddColumn\n");

    if(!builder->started)
    {
        printf("Builder not started, but attempting to add column?!\n");
    }

    switch(builder->statement->type)
    {
        case SQLStatement::CREATE_TABLE:
        {
            CreateTableCommand* cmd = (CreateTableCommand*)builder->statement;
            SQLColumn column;
            column.name = column_name->text;
            column.type = getSQLType(&column_type->text);

            std::swap(column.constraint, builder->temp_constraints);
            builder->temp_constraints.clear();

            cmd->columns.push_back(column);
        }
        break;
        case SQLStatement::SELECT:
        { }
        break;

        case SQLStatement::INVALID:
        default:
        {}
        break;
    }
}

void builderAddTableName(StatementBuilder* builder, Token table_name)
{
    if(!builder->started)
    {
        printf("%s: Builder hasn't been started!\n", __FUNCTION__);
        return;
    }

    switch(builder->statement->type)
    {
        case SQLStatement::INSERT_INTO:
            {
                InsertCommand* insert = reinterpret_cast<InsertCommand*>(builder->statement);
                insert->table = table_name->text;
            }
            break;
        case SQLStatement::UPDATE:
            {
                UpdateCommand* update = reinterpret_cast<UpdateCommand*>(builder->statement);
                update->table = table_name->text;

                // Grab the table schema
                SchemaResult schema_check = builder->data_store->getTableSchema(update->table);
                if(schema_check.status == ResultStatus::SUCCESS)
                {
                    update->table_schema = schema_check.result;
                }
                else
                {
                    // TODO: Handle error here?
                }
            }
            break;
        case SQLStatement::DELETE:
            {
                DeleteCommand* del = reinterpret_cast<DeleteCommand*>(builder->statement);
                del->table = table_name->text;
            }
            break;
        default:
            printf("%s: SQL statement being built isn't supported!\n", __FUNCTION__);
            break;
    }
}

void builderStartNestedExpr(StatementBuilder* builder, Token operation)
{
    if(builder == nullptr)
    {
        printf("This shouldn't happen\n");
        // TODO: Assert?
	    return; 
    }

    printf("Starting nested expression: [%s]\n", operation->text.c_str());

    if(isExprFlagContained(builder->expr->flags, ExpressionFlags::NESTED))
    {
        builder->expr->op = static_cast<ExpressionOperation>(operation->operation);
        printf("Nested operation = %u\n", (uint32_t)builder->expr->op);
        builder->expr->flags = ExpressionFlags::NESTED;
    }

    printExpressionTree(builder->expr);
}

void builderAddValueExpr(StatementBuilder* builder,
	Token operation, Token left_term, Token right_term)
{
    printf("Adding value expression: %s %s %s\n",
            left_term->text.c_str(),
            operation->text.c_str(),
            right_term->text.c_str());

    if(builder->expr == nullptr)
    {
        builder->expr = createExpression(operation, left_term, right_term);
        if(builder->expr == nullptr)
        {
            printf("Fatal error, unable to generate value expression\n");
            return;
        }
    }
    else
    {
        printf("Non-null builder->expr\n");
        // Create nested expression and set the previous to the left child
        Expression* right = createExpression(operation, left_term, right_term);
        if(right == nullptr)
        {
            printf("Fatal error, unable to generate value expression\n");
            return;
        }

        Expression* parent = new (std::nothrow) Expression();
        if(parent == nullptr)
        {
            printf("Fatal error, unable to generate value expression\n");
            return;
        }

        parent->flags = ExpressionFlags::NESTED | ExpressionFlags::EMPTY;
        parent->left = builder->expr;
        parent->right = right;

        builder->expr = parent;
    }

    printExpressionTree(builder->expr);
}

Expression* createExpression(Token op, Token left, Token right)
{
    Expression* parent = new (std::nothrow) Expression(op);
    if(parent)
    {
        Expression* left_expr = new (std::nothrow) Expression(left);
        if(left_expr)
        {
            parent->left = left_expr;
        }
        else
        {
            delete parent;
            return nullptr;
        }

        Expression* right_expr = new (std::nothrow) Expression(right);
        if(right_expr)
        {
            parent->right = right_expr;
        }
        else
        {
            delete parent;
            return nullptr;
        }
    }
    else
    {
        return nullptr;
    }

    return parent;
}

void builderClean(StatementBuilder* builder)
{
    // DEBUG
    printf("builderClean called\n");

    if(builder->started)
    {
        delete builder->statement;
    }

    builder->statement = nullptr;

    builder->started = false;
}

DataType getSQLType(std::string* type)
{
    if(type == nullptr)
    {
        printf("Null pointer in getSQLType, check your parser.\n");
        return DataType::NONE;
    }
  
    if((*type) == "INTEGER")
    {
        return DataType::INTEGER;
    }
  
    if((*type) == "SMALLINT")
    {
        return DataType::SMALL_INT;
    }
  
    if((*type) == "BIGINT")
    {
        return DataType::BIG_INT;
    }
  
    if((*type) == "BOOLEAN")
    {
        return DataType::BOOLEAN;
    }
  
    if((*type) == "DATE")
    {
        return DataType::DATE;
    }
  
    if((*type) == "TIME")
    {
        return DataType::TIME;
    }
  
    return DataType::NONE;
}
