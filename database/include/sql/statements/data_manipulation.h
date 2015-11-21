/* TODO: File header */

#ifndef DATA_MANIP_H
#define DATA_MANIP_H

#include <vector>
#include <string>

// Boost include
#include <boost/variant.hpp>

// Forward declaration
struct Expression;

template <typename T>
struct RangeValue
{
  T start;
  T end;
};

template <class S>
using ExpectedValue = boost::variant<S, RangeValue<S>>;

enum class ExpressionOperation : int32_t
{
  NO_OP = 0,
  EQUALS,
  LESSER,
  LESSER_EQUALS,
  GREATER,
  GREATER_EQUALS,
  IN,
  BETWEEN
};

struct BasePredicate
{
  /* TODO: Table* table */
  uint32_t column_idx;

  ExpressionOperation operation;

  /* TODO: bool evaluate(DataRecord& record);*/
};

template <class E>
struct Predicate : public BasePredicate
{
  ExpectedValue<E> value;
};

struct ColumnReference
{
  /* TODO: Table* table; */
  uint32_t column_idx;
};

struct SelectQuery
{
  /* TODO (if needed): std::vector<Table*> source_tables;*/
  std::vector<ColumnReference> source_columns;
  std::vector<std::string> output_columns;
  std::vector<BasePredicate*> predicates;
  Expression* expr;
};

// TODO: UPDATE, INSERT INTO & DELETE statements

#endif
