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

