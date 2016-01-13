/*
 *
 */

#include <map>
#include <iostream>
#include <functional>

#include "sql/statements/expression.h"

static thread_local std::map<std::string, ExpressionOperation> operation_map = {
    {"=",ExpressionOperation::EQUALS},
    {"<",ExpressionOperation::LESSER},
    {"<=",ExpressionOperation::LESSER_EQUALS},
    {">",ExpressionOperation::GREATER},
    {">=",ExpressionOperation::GREATER_EQUALS},
    {"IN",ExpressionOperation::IN},
    {"BETWEEN",ExpressionOperation::BETWEEN},
    {"AND",ExpressionOperation::AND},
    {"OR",ExpressionOperation::OR}
};

Expression::Expression() 
    : left(nullptr), right(nullptr), 
      op(ExpressionOperation::NO_OP), flags(ExpressionFlags::EMPTY),
      table_name(nullptr), table_column(nullptr)
{}

Expression::Expression(Token tok)
    : left(nullptr), right(nullptr), op(ExpressionOperation::NO_OP),
      flags(ExpressionFlags::EMPTY),
      table_name(nullptr), table_column(nullptr)
{
    if(tok)
    {
        if(tok->is_column)
        {
            flags = ExpressionFlags::COLUMN;
            table_name = new std::string(*tok->table_name);
            table_column = new std::string(*tok->column_name);
        }
        else
        {
            if(tok->is_value)
            {
                flags = ExpressionFlags::VALUE;
                value = tok->value;
            }
            else
            {
                flags = ExpressionFlags::OPERATION;
                op = (ExpressionOperation)tok->operation;
            }
        }
    }
}

Expression::~Expression()
{
    delete left;
    delete right;
}

ExpressionFlags operator |(ExpressionFlags a, ExpressionFlags b)
{
    return static_cast<ExpressionFlags>(static_cast<uint32_t>(a) |  static_cast<uint32_t>(b));
}

ExpressionFlags operator &(ExpressionFlags a, ExpressionFlags b)
{
    return static_cast<ExpressionFlags>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
}

bool isExprFlagContained(ExpressionFlags a, ExpressionFlags b)
{
    return (static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
}

ExpressionOperation getOperation(std::string op)
{
    if(operation_map.find(op) != operation_map.end())
    {
	    return operation_map[op];
    }

    return ExpressionOperation::NO_OP;
}

std::string getOperationString(ExpressionOperation op)
{
    for(auto itr = operation_map.begin();
	    itr != operation_map.end();
	    itr++)
    {
	    if(itr->second == op)
	    {
	        return itr->first;
	    }
    }

    return "";
}

static void print_expr(Expression* expr)
{
    if(expr)
    {
        switch(expr->flags)
        {
            case ExpressionFlags::COLUMN:
                std::cout << *expr->table_name << "." << *expr->table_column;
                break;
            case ExpressionFlags::VALUE:
                switch(expr->value.data.type)
                {
                    case DataType::SMALL_INT:
                    case DataType::INTEGER:
                    case DataType::BIG_INT:
                        std::cout << expr->value.data.value;
                        break;
                    case DataType::FLOAT:
                    {
                        FloatData float_data = { .value = expr->value.data.value};
                        std::cout << float_data.data;
                    }
                        break;
                    default:
                        std::cout << "UNRECOG";
                        break;
                }
                break;
            case ExpressionFlags::OPERATION:
            case ExpressionFlags::NESTED:
                std::cout << getOperationString(expr->op).c_str();
                break;
            default:
                std::cout << "Unknown @ " << expr;
                break;
        }
        std::cout << " ";
    }
}

// TODO: Document this
void printExpressionTree(Expression* expr, int indent)
{
    printf("Expression evaluates to following expression: ");
    if(expr == nullptr)
    {
        return;
    }

    std::function<void(Expression*)> print = [&print] (Expression* expr)
    {
        if(!expr) { return; }

        print(expr->left);

        print_expr(expr);

        print(expr->right);
    };

    print(expr);

    printf("\n");
}
