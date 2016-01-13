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
  
    builder->type = SQLStatement::CREATE_TABLE;
    builder->statement = command;
  
    printf("Builder started for CREATE TABLE statement\n");
}

void builderStartDropTable(StatementBuilder* builder, Token table_name)
{
    DropTableCommand* command = new DropTableCommand;
    command->table_name = *table_name->text;
  
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
void builderAddSelectAllColumns(StatementBuilder* builder, Token table)
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
  
    if(builder->type != SQLStatement::SELECT && builder->statement == nullptr)
    {
        // Start the SELECT statement then
        builderStartSelectQuery(builder);
    }
  
    SelectQuery* query = reinterpret_cast<SelectQuery*>(builder->statement);
  
    // Push the previously calculated information into the query object
    query->source_columns.push_back(source);

    query->output_columns.push_back(*output_column->text);
}

void builderGeneratePredicates(StatementBuilder* builder)
{
    printf("Generating predicates for statement\n");
}

// Generic-ish helper functions ///////////////////////////////////////////////

void builderAddColumn(StatementBuilder* builder, Token column_name,
                      Token column_type, Token column_constraints)
{
    printf("Entered builderAddColumn\n");

    switch(builder->type)
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
        {
        }
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
        /*
        builder->expr = new (std::nothrow) Expression();
        builder->expr->flags = ExpressionFlags::OPERATION;
        builder->expr->op = getOperation(*operation->text);

	    Expression* left = new (std::nothrow) Expression();
	    if(left_term->is_column)
	    {
	        left->flags = ExpressionFlags::COLUMN;
	        left->table_name = new std::string(*left_term->table_name);
	        left->table_column = new std::string(*left_term->column_name);
	    }
	    else
	    {
	        left->flags = ExpressionFlags::VALUE;
	        left->value = left_term->value;
	    }

	    Expression* right = new (std::nothrow) Expression();
	    if(right_term->is_column)
	    {
	        right->flags = ExpressionFlags::COLUMN;
	        right->table_name = new (std::nothrow) std::string(*right_term->table_name);
	        right->table_column = new (std::nothrow) std::string(*right_term->column_name);
	    }
	    else
	    {
	        right->flags = ExpressionFlags::VALUE;
	        right->value = right_term->value;
	    }

	    builder->expr->left = left;
	    builder->expr->right = right;
        */
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

    switch(builder->type)
    {
        // TODO: Finish this
        default:
            // Throw error
            break;
    }

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
