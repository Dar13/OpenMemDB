/* TODO: File header */

#ifndef SQL_TYPES_COMMON_H
#define SQL_TYPES_COMMON_H

#include "boolean.h"
#include "datetime.h"
#include "exact_numeric.h"

enum SQLTypes
{
 //SQL_NONE = 0, redeclaration error when both common.h files are included
  SQL_BOOLEAN = 1,
  SQL_SMALLINT,
  SQL_INTEGER,
  SQL_BIGINT,
  SQL_DATE,
  SQL_TIME,
  SQL_TIMESTAMP
};

#endif
