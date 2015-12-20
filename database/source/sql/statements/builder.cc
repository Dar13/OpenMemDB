/* TODO: File Header */

#include "sql/statements/builder.h"

#include <cstdio>

static DataType getSQLType(std::string* type);

void builderStartCreateTable(StatementBuilder* builder, std::string* table_name)
{
    CreateTableCommand* command = new CreateTableCommand;
    command->table_name = *table_name;
  
    builder->type = SQLStatement::CREATE_TABLE;
    builder->statement = command;
  
    printf("Builder started for CREATE TABLE statement\n");
}

void builderStartDropTable(StatementBuilder* builder, std::string* table_name)
{
    DropTableCommand* command = new DropTableCommand;
    command->table_name = *table_name;
  
    builder->type = SQLStatement::DROP_TABLE;
    builder->statement = command;
  
    printf("Builder started/done for DROP TABLE statement\n");
}

void builderStartSelectQuery(StatementBuilder* builder)
{
    SelectQuery* query = new SelectQuery();
  
    builder->type = SQLStatement::SELECT;
    builder->statement = query;
  
    printf("Builder started for SELECT statement\n");
}

// SELECT helper functions ////////////////////////////////////////////////////
void builderAddSelectAllColumns(StatementBuilder* builder, SQLToken table)
{
    // Don't want to override a currently active statement
    if(builder->type == SQLStatement::INVALID &&
       builder->statement == nullptr)
    {
        builderStartSelectQuery(builder);
    }

    if(builder->type != SQLStatement::SELECT)
    {
        // Error!
        printf("Statement is already being built!\n");
        return;
    }

    // TODO: Add all columns in the given table to this query

    printf("AddSelectAllColumns called\n");
}

void builderAddQualifiedSelectColumn(StatementBuilder* builder,
                                     SQLToken table, SQLToken source_column,
                                     SQLToken output_column)
{
    // This means the output column should be named the same as the 
    // source column
    if(output_column == nullptr)
    {
        output_column = source_column;
    }
  
    ColumnReference source;
    source.table = *table;
    /* TODO: Calculate source_column's index within the given table */
    source.column_idx = 0;
  
    if(builder->type != SQLStatement::SELECT && builder->statement == nullptr)
    {
        // Start the SELECT statement then
        builderStartSelectQuery(builder);
    }
  
    SelectQuery* query = reinterpret_cast<SelectQuery*>(builder->statement);
  
    // Push the previously calculated information into the query object
    query->source_columns.push_back(source);

    query->output_columns.push_back(*output_column);
}

void builderGeneratePredicates(StatementBuilder* builder)
{
    printf("Generating predicates for statement\n");
}

// Generic-ish helper functions ///////////////////////////////////////////////

void builderAddColumnName(StatementBuilder* builder, std::string* column_name)
{
    printf("Entered buildAddColumnName\n");
  
    switch(builder->type)
    {
        case SQLStatement::CREATE_TABLE:
        {
            CreateTableCommand* cmd = (CreateTableCommand*)builder->statement;
            cmd->column_names.push_back(*column_name);
        }
        break;
  
        case SQLStatement::SELECT:
        {
            SelectQuery* query = (SelectQuery*)builder->statement;
            // TODO: Finish this...
        }
        break;
  
        case SQLStatement::INVALID:
        default:
            printf("There isn't a statement being built...\n");
            break;
    }
  
    printf("Exiting builderAddColumnName\n");
}

void builderAddColumnType(StatementBuilder* builder, std::string* column_type)
{
    printf("Entered builderAddColumnType\n");
    DataType type = getSQLType(column_type);
  
    switch(builder->type)
    {
        case SQLStatement::CREATE_TABLE:
        {
            CreateTableCommand* cmd = (CreateTableCommand*)builder->statement;
            cmd->column_types.push_back(type);
        }
        break;
  
        case SQLStatement::SELECT:
        {
            SelectQuery* query = (SelectQuery*)builder->statement;
            // TODO: Finish this...
        }
        break;
  
        case SQLStatement::INVALID:
        default:
            printf("There isn't a statement being built...\n");
            break;
    }
  
    printf("Exiting builderAddColumnType\n");
}

void builderStartNestedExpr(StatementBuilder* builder, SQLToken operation)
{
    if(builder == nullptr)
    {
	return; 
    }

    printf("Starting nested expression: [%s]\n", operation->c_str());
}

void builderAddValueExpr(StatementBuilder* builder,
	SQLToken operation, SQLToken left_term, SQLToken right_term)
{
    printf("Adding value expression\n");
}

void builderClean(StatementBuilder* builder)
{
    // DEBUG
    printf("builderClean called\n");

    delete builder->statement;

    builder->statement = nullptr;
    builder->type = SQLStatement::INVALID;
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

