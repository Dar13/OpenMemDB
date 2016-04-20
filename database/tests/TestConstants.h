#ifndef TESTCONSTANTS_H
#define TESTCONSTANTS_H

// Use a namespace to declare constants
namespace TestConstants
{
  const int MaxTables = 10000;
  const int MaxInserts = 500000;
  const int MaxSelects = 10000;
  const int MaxMixed = 10000;

  const int InsertToTables = 1;
  const int SelectFromTables = 1;
}

#endif