/* TODO: File Header */

#include "sql/statements/builder.h"

#include <cstdio>

static SQLType getSQLType(std::string* type);

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
  // TODO: Add all columns in the given table to this query
}

void builderAddQualifiedSelectColumn(StatementBuilder* builder,
                                     SQLToken table, SQLToken source_column,
                                     SQLToken output_column)
{
  if(output_column == nullptr)
  {
    output_column = source_column;
  }

  /* TODO: Calculate source_column's index within the given table */
  ColumnReference source;

  if(builder->type != SQLStatement::SELECT)
  {
    printf("Invalid command!\n");
    return;
  }

  SelectQuery* query = reinterpret_cast<SelectQuery*>(builder->statement);

  query->source_columns.push_back(source);
  query->output_columns.push_back(*output_column);
}

void builderAddSelectPredicate(StatementBuilder* builder, SQLToken expr)
{
  // TODO: Create a tree of Expression objects as needed
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
  SQLType type = getSQLType(column_type);

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

void builderAddExpressions(StatementBuilder* builder,
                           SQLToken left, SQLToken right, SQLToken op)
{
}

SQLType getSQLType(std::string* type)
{
  if(type == nullptr)
  {
    printf("Null pointer in getSQLType, check your parser.\n");
    return SQLType::NONE;
  }

  if((*type) == "INTEGER")
  {
    return SQLType::INTEGER;
  }

  if((*type) == "SMALLINT")
  {
    return SQLType::SMALLINT;
  }

  if((*type) == "BIGINT")
  {
    return SQLType::BIGINT;
  }

  if((*type) == "BOOLEAN")
  {
    return SQLType::BOOLEAN;
  }

  if((*type) == "DATE")
  {
    return SQLType::DATE;
  }

  if((*type) == "TIME")
  {
    return SQLType::TIME;
  }

  if((*type) == "TIMESTAMP")
  {
    return SQLType::TIMESTAMP;
  }

  return SQLType::NONE;
}

