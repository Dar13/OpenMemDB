/* TODO: File header */

// Standard library includes
#include <string>

// Project includes
#include "sql/statements/data_manipulation.h"

static std::string getExpressionOperation(ExpressionOperation op);

bool Predicate::evaluate() { return false; }

bool NestedPredicate::evaluate()
{
    switch(op)
    {
        case ExpressionOperation::AND:
            return (left_child->evaluate() && right_child->evaluate());
            break;
        case ExpressionOperation::OR:
            return (left_child->evaluate() || right_child->evaluate());
            break;
        case ExpressionOperation::NOT_EQUALS:
            return (left_child->evaluate() != right_child->evaluate());
            break;
        default:
            printf("Unable to perform operation on given nested predicate.");
            printf("Operation: %s\n", getExpressionOperation(op).c_str());
            return false;
            break;
    }

    // This should never be reached
    return false;
}

bool ValuePredicate::evaluate()
{
    switch(op)
    {
    case ExpressionOperation::EQUALS:

    break;
    case ExpressionOperation::NOT_EQUALS:

    break;
    case ExpressionOperation::LESSER:
    break;
    case ExpressionOperation::LESSER_EQUALS:
    break;
    case ExpressionOperation::GREATER:
    break;
    case ExpressionOperation::GREATER_EQUALS:
    break;
    case ExpressionOperation::IN:
    break;
    case ExpressionOperation::BETWEEN:
    break;
    default:
        printf("Unable to perform operation on given nested predicate.");
        printf("Operation: %s\n", getExpressionOperation(op).c_str());
    break;
    }

    return false;
}

bool ColumnPredicate::evaluate()
{
    // TODO: Make this actually do work.
    switch(op)
    {
    case ExpressionOperation::EQUALS:
    break;
    case ExpressionOperation::NOT_EQUALS:
    break;
    case ExpressionOperation::LESSER:
    break;
    case ExpressionOperation::LESSER_EQUALS:
    break;
    case ExpressionOperation::GREATER:
    break;
    case ExpressionOperation::GREATER_EQUALS:
    break;
    case ExpressionOperation::IN:
    break;
    case ExpressionOperation::BETWEEN:
    break;
    default:
        printf("Unable to perform operation on given nested predicate.");
        printf("Operation: %s\n", getExpressionOperation(op).c_str());
    break;
    }

    return false;
}

std::string getExpressionOperation(ExpressionOperation op)
{
    switch(op)
    {
	case ExpressionOperation::AND:
	    return "AND";
	case ExpressionOperation::OR:
	    return "OR";
	case ExpressionOperation::EQUALS:
	    return "=";
	case ExpressionOperation::NOT_EQUALS:
	    return "!=";
	case ExpressionOperation::LESSER:
	    return "<";
	case ExpressionOperation::LESSER_EQUALS:
	    return "<=";
	case ExpressionOperation::GREATER:
	    return ">";
	case ExpressionOperation::GREATER_EQUALS:
	    return ">=";
	case ExpressionOperation::IN:
	    return "IN";
	case ExpressionOperation::BETWEEN:
	    return "BETWEEN";
	default:
	    return "INVALID";
    }
}
