/* TODO: File header */

#ifndef DATA_MANIP_H
#define DATA_MANIP_H

#include <vector>
#include <string>

// Boost include
#include <boost/variant.hpp>

template <typename T>
struct RangeValue
{
  T start;
  T end;
};

template <class S>
using ExpectedValue = boost::variant<S, RangeValue<S>>;

enum class PredicateOperation : int32_t
{
  NO_OP = 0,
  EQUALS,
  LESSER,
  LESSER_EQUALS,
  GREATER,
  GREATER_EQUALS,
  WITHIN
};

struct BasePredicate
{
  /* TODO: Table* table */
  uint32_t column_idx;

  enum class Operation : int32_t
  {
    NO_OP = 0,
    EQUALS,
    LESSER,
    LESSER_EQUALS,
    GREATER,
    GREATER_EQUALS,
    WITHIN
  };

  Operation eval_operation;

  /* TODO: bool evaluate(DataRecord& record);*/
};

template <class E>
struct Predicate : public BasePredicate
{
  ExpectedValue<E> value;
};

struct SelectOutputColumn
{
  std::string table_name;
  std::string output_name;
};

struct SelectQuery
{
  std::vector<std::string> source_tables;
  std::vector<SelectOutputColumn> source_columns;
  std::vector<BasePredicate*> predicates;
};

#endif
