/*
 *
 */

#include "sql/predicate.h"

#include "data/data_store.h"

/**
 *
 */
Predicate* makePredicateFromExpression(Expression* expr, DataStore* data_store)
{
    if(expr == nullptr)
    {
        return nullptr;
    }

    Predicate* pred = nullptr;

    ExpressionFlags type = expr->flags & (ExpressionFlags::NESTED | 
                                          ExpressionFlags::COLUMN |
                                          ExpressionFlags::VALUE);

    if(type == ExpressionFlags::NESTED)
    {
        NestedPredicate* nested_pred = new (std::nothrow) NestedPredicate;
        if(nested_pred == nullptr)
        {
            // TODO: Error handling
        }

        Predicate* left = makePredicateFromExpression(expr->left, data_store);
        Predicate* right = makePredicateFromExpression(expr->right, data_store);

        if(left == nullptr || right ==  nullptr)
        {
            delete left;
            delete right;
            delete pred;
            pred = nullptr;
        }
        else
        {
            nested_pred->left_child = left;
            nested_pred->right_child = right;
        }

        pred = nested_pred;
    }

    if(type == ExpressionFlags::OPERATION)
    {
        Expression* left = expr->left;
        Expression* right = expr->right;

        if(isExprFlagContained(left->flags, ExpressionFlags::COLUMN) &&
           isExprFlagContained(right->flags, ExpressionFlags::COLUMN))
        {
            ColumnPredicate* col_pred = new (std::nothrow) ColumnPredicate;

            ColumnReference left_ref;
            ColumnReference right_ref;

            left_ref.table = *left->table_name;
            right_ref.table = *right->table_name;

            auto res = data_store->getColumnIndex(left_ref.table, *left->table_column);
            if(res.status == ResultStatus::SUCCESS)
            {
                left_ref.column_idx = res.result;
            }

            res = data_store->getColumnIndex(right_ref.table, *right->table_column);
            if(res.status == ResultStatus::SUCCESS)
            {
                right_ref.column_idx = res.result;
            }

            col_pred->left_column = left_ref;
            col_pred->right_column = right_ref;

            pred = col_pred;
        }
        else
        {
            ValuePredicate* val_pred = new (std::nothrow) ValuePredicate;

            // TODO: Handle the value_type?
            if(isExprFlagContained(left->flags, ExpressionFlags::VALUE))
            {
                val_pred->expected_value = ExpressionValue(left->value);

                ColumnReference col;
                col.table = *right->table_name;
                auto res = data_store->getColumnIndex(col.table, 
                                                      *right->table_column);
                if(res.status == ResultStatus::SUCCESS)
                {
                    col.column_idx = res.result;
                }
                else
                {
                    // TODO: Error handling
                    delete pred;
                    pred = nullptr;
                }

                val_pred->column = col;
            }
            else
            {
                val_pred->expected_value = ExpressionValue(right->value);

                ColumnReference col;
                col.table = *left->table_name;
                auto res = data_store->getColumnIndex(col.table, 
                                                      *left->table_column);
                if(res.status == ResultStatus::SUCCESS)
                {
                    col.column_idx = res.result;
                }
                else
                {
                    // TODO: Error handling
                    delete pred;
                    pred = nullptr;
                }

                val_pred->column = col;
            }

            pred = val_pred;
        }
    }

    return pred;
}

void setupStatement(ParsedStatement* statement, Expression* expr, DataStore* data_store)
{
    if(statement == nullptr || expr == nullptr)
    {
        // TODO: Make an error propagate back up
        return;
    }

    switch(statement->type)
    {
        case SQLStatement::SELECT:
            {
                SelectQuery* query = reinterpret_cast<SelectQuery*>(statement);
                query->predicate = makePredicateFromExpression(expr, data_store);
            }
            break;
        case SQLStatement::UPDATE:
            // TODO
            break;
        case SQLStatement::INSERT_INTO:
            // TODO
            break;
        case SQLStatement::DELETE:
            // TODO
            break;
        default:
            break;
    }
}
