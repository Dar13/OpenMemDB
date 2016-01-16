/*
 *
 */

#include "sql/predicate.h"

static Predicate* makePredicateFromExpressionSubtree(Expression* expr);

/**
 *
 */
Predicate* getPredicateFromExpression(Expression* expr)
{
    if(expr == nullptr)
    {
        return nullptr;
    }

    Predicate* root = makePredicateFromExpressionSubtree(expr);

    return root;
}

Predicate* makePredicateFromExpressionSubtree(Expression* expr)
{
    if(expr == nullptr)
    {
        return nullptr;
    }

    Predicate* pred = nullptr;

    ExpressionFlags type = expr->flags & (ExpressionFlags::NESTED | 
                                          ExpressionFlags::COLUMN |
                                          ExpressionFlags::VALUE);

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

            // TODO: Make this work
            /*
            auto res = data_store->getColumnIdx(left_ref.table, *left->column_name);
            if(res.status == ResultStatus::SUCCESS)
            {
                left_ref.column_idx = res.result;
            }

            res = data_store->getColumnIdx(right_ref.table, *right->column_name);
            if(res.status == ResultStatus::SUCCESS)
            {
                right_ref.column_idx = res.result;
            }
            */

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
                // TODO: Make this possible
                /*
                auto res = data_store->getColumnIndex(col.table_name, 
                                                         *right->table_column);
                if(res.status == SUCCESS)
                {
                    col.column_idx = res.result;
                }
                else
                {
                    // TODO: Error handling
                }
                */

                val_pred->column = col;
            }
            else
            {
                val_pred->expected_value = ExpressionValue(right->value);

                ColumnReference col;
                col.table = *left->table_name;
                // TODO: Make this possible
                /*
                auto res = data_store->getColumnIndex(col.table_name, 
                                                      *left->table_column);
                if(res.status == SUCCESS)
                {
                    col.column_idx = res.result;
                }
                else
                {
                    // TODO: Error handling
                }
                */

                val_pred->column = col;
            }

            pred = val_pred;
        }
    }

    return pred;
}
