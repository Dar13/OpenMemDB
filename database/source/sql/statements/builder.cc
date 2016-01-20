/* TODO: File Header */

#include "sql/statements/builder.h"

#include <cstdio>

// Helper functions
static DataType getSQLType(std::string* type);

static Expression* createExpression(Token op, Token left, Token right);

void builderStartCreateTable(StatementBuilder* builder, Token table_name)
{
    CreateTableCommand* command = new CreateTableCommand;
    command->table_name = *table_name->text;
  
    builder->started = true;
    builder->statement = command;
    builder->valid = true;
  
    printf("Builder started for CREATE TABLE statement\n");
}

void builderStartDropTable(StatementBuilder* builder, Token table_name)
{
    DropTableCommand* command = new DropTableCommand;
    command->table_name = *table_name->text;
  
    builder->started = true;
    builder->statement = command;
    builder->valid = true;
  
    printf("Builder started/done for DROP TABLE statement\n");
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
  
    printf("Builder started for SELECT statement\n");
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
        case SQLStatement::UPDATE:
            break;
        case SQLStatement::INSERT_INTO:
            break;
        case SQLStatement::DELETE:
            break;
        default:
            break;
    }

    printf("AddSelectAllColumns called for table %s\n", table->text->c_str());
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
  
    ColumnReference source;
    source.table = *table->text;
    /* TODO: Calculate source_column's index within the given table */
    source.column_idx = 0;
  
    if(!builder->started)
    {
        // Start the SELECT statement then
        builderStartSelectQuery(builder);
    }
    else
    {
        // TODO: Error handling
    }
  
    SelectQuery* query = reinterpret_cast<SelectQuery*>(builder->statement);
  
    // Push the previously calculated information into the query object
    query->source_columns.push_back(source);

    query->output_columns.push_back(*output_column->text);
}

// Generic-ish helper functions ///////////////////////////////////////////////

void builderAddColumn(StatementBuilder* builder, Token column_name,
                      Token column_type, Token column_constraints)
{
    printf("Entered builderAddColumn\n");

    if(!builder->started)
    {
        // TODO: What do?
    }

    switch(builder->statement->type)
    {
        case SQLStatement::CREATE_TABLE:
        {
            CreateTableCommand* cmd = (CreateTableCommand*)builder->statement;
            SQLColumn column;
            column.name = *(column_name->text);
            column.type = getSQLType(column_type->text);
            // TODO: Column constraints
            (void)column_constraints;

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

    printf("Exit builderAddColumn\n");
}

void builderStartNestedExpr(StatementBuilder* builder, Token operation)
{
    if(builder == nullptr)
    {
        printf("This shouldn't happen\n");
        // TODO: Assert?
	    return; 
    }

    printf("Starting nested expression: [%s]\n", operation->text->c_str());

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
            left_term->text->c_str(),
            operation->text->c_str(),
            right_term->text->c_str());

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
