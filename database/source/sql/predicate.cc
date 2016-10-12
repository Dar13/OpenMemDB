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

#include "sql/predicate.h"

#include "data/data_store.h"

/**
 * TODO: Document this
 */
Predicate* makePredicateFromExpression(Expression* expr, DataStore* data_store)
{
    if(expr == nullptr)
    {
        return nullptr;
    }

    Predicate* pred = nullptr;

    if(isExprFlagContained(expr->flags, ExpressionFlags::NESTED))
    {
        NestedPredicate* nested_pred = new (std::nothrow) NestedPredicate;
        if(nested_pred == nullptr)
        {
            printf("%s: Allocation failure!\n", __FUNCTION__);
            return nullptr;
        }

        Predicate* left = makePredicateFromExpression(expr->left, data_store);
        Predicate* right = makePredicateFromExpression(expr->right, data_store);

        // Something failed, return nullptr to signify failure
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

        nested_pred->op = expr->op;
        pred = nested_pred;
    }

    if(isExprFlagContained(expr->flags, ExpressionFlags::OPERATION))
    {
        Expression* left = expr->left;
        Expression* right = expr->right;

        if(isExprFlagContained(left->flags, ExpressionFlags::COLUMN) &&
           isExprFlagContained(right->flags, ExpressionFlags::COLUMN))
        {
            ColumnPredicate* col_pred = new (std::nothrow) ColumnPredicate;

            ColumnReference left_ref;
            ColumnReference right_ref;

            left_ref.table = left->table_name;
            right_ref.table = right->table_name;

            // TODO: Error handling
            auto res = data_store->getColumnIndex(left_ref.table, left->table_column);
            if(res.status == ResultStatus::SUCCESS)
            {
                left_ref.column_idx = res.result;
            }

            // TODO: Error handling
            res = data_store->getColumnIndex(right_ref.table, right->table_column);
            if(res.status == ResultStatus::SUCCESS)
            {
                right_ref.column_idx = res.result;
            }

            TableSchema left_schema = data_store->getTableSchema(left_ref.table).result;
            TableSchema right_schema = data_store->getTableSchema(right_ref.table).result;

            // Make sure the two types can actually be compared.
            // Note: All integer types can always be compared to each other, 
            // which is why the if-statement checks for if both types are 
            // integral. The if-statement should short-circuit if both are equal
            DataType left_type = left_schema.columns.at(left_ref.column_idx).type;
            DataType right_type = right_schema.columns.at(right_ref.column_idx).type;
            if((left_type == right_type) ||
               ((left_type >= SMALL_INT && left_type <= BIG_INT) &&
                (right_type >= SMALL_INT && right_type <= BIG_INT)))
            {
                col_pred->left_column = left_ref;
                col_pred->right_column = right_ref;

                pred = col_pred;
            }
            else
            {
                // The types can't be compared, signify failure and clean up
                delete col_pred;
                pred = nullptr;
            }
        }
        else
        {
            ValuePredicate* val_pred = new (std::nothrow) ValuePredicate;
            val_pred->op = expr->op;

            TableSchema table_schema;

            // TODO: Handle the value_type?
            if(isExprFlagContained(left->flags, ExpressionFlags::VALUE))
            {
                val_pred->expected_value = ExpressionValue(left->value);

                ColumnReference col;
                col.table = right->table_name;
                auto res = data_store->getColumnIndex(col.table, 
                                                      right->table_column);
                if(res.status == ResultStatus::SUCCESS)
                {
                    col.column_idx = res.result;
                    val_pred->column = col;

                    table_schema = data_store->getTableSchema(col.table).result;
                }
                else
                {
                    // TODO: Error handling
                    delete val_pred;
                    val_pred = nullptr;
                }
            }
            else
            {
                val_pred->expected_value = ExpressionValue(right->value);

                ColumnReference col;
                col.table = left->table_name;
                auto res = data_store->getColumnIndex(col.table, 
                                                      left->table_column);
                if(res.status == ResultStatus::SUCCESS)
                {
                    col.column_idx = res.result;
                    val_pred->column = col;

                    auto schema_result = data_store->getTableSchema(col.table);
                    if(schema_result.status == ResultStatus::SUCCESS)
                    {
                        table_schema = schema_result.result;
                    }
                    else
                    {
                        delete val_pred;
                        val_pred = nullptr;
                    }
                }
                else
                {
                    // TODO: Error handling
                    delete val_pred;
                    val_pred = nullptr;
                }
            }

            if(val_pred)
            {
                DataType col_type = table_schema.columns.at(val_pred->column.column_idx).type;
                DataType val_type;
                if(val_pred->expected_value.is_range)
                {
                    // TODO: Handle ranges properly
                }
                else
                {
                    val_type = (DataType)val_pred->expected_value.getValue().type;
                }

                if((val_type == col_type) ||
                   ((val_type >= SMALL_INT && val_type <= BIG_INT) &&
                    (col_type >= SMALL_INT && val_type <= BIG_INT)))
                {
                    pred = val_pred;
                }
                else
                {
                    delete val_pred;
                    pred = nullptr;
                }
            }
            else
            {
                pred = nullptr;
            }
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
            {
                UpdateCommand* update = reinterpret_cast<UpdateCommand*>(statement);
                update->predicate = makePredicateFromExpression(expr, data_store);
            }
            break;
        case SQLStatement::DELETE:
            {
                DeleteCommand* del = reinterpret_cast<DeleteCommand*>(statement);
                del->predicate = makePredicateFromExpression(expr, data_store);
            }
            break;
        default:
            // Should never reach this
            break;
    }
}

SQLBoolean evaluateOperation(ExpressionOperation op,
                             ExpressionValue lhs, ExpressionValue rhs)
{
    switch(op)
    {
        case ExpressionOperation::EQUALS:
            return (lhs == rhs);

        case ExpressionOperation::NOT_EQUALS:
            return (lhs != rhs);

        default:
            printf("Operation not implemented!\n");
            return SQL_FALSE;
    }
}

SQLBoolean operator==(ExpressionValue& lhs, ExpressionValue& rhs)
{
    if(lhs.is_range || rhs.is_range)
    {
        // TODO: Can you even compare ranges?
        return SQL_UNKNOWN;
    }

    // You can't compare against a NULL value
    if(lhs.getValue().null || rhs.getValue().null)
    {
        return SQL_UNKNOWN;
    }

    // TODO: Verify this works for floats
    // This should work for the integral types as the bit layouts should be 
    // the same for equivalent values.
    if(lhs.getValue().value == rhs.getValue().value)
    {
        return SQL_TRUE;
    }

    return SQL_FALSE;
}

SQLBoolean operator!=(ExpressionValue& lhs, ExpressionValue& rhs)
{
    return !(lhs == rhs);
}
