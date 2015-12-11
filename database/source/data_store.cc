#include "data/data_store.h"
#include <iostream>

class data_store
{
/*
	TableMap lookup_table = new TableMap();
	//creates an empty table
	void createTable(char *tablename, TableSchema schema)
	{
		DataTable table = new DataTable();
		SchemaTablePair table_schema= {table, schema);
		TableHashFunctor hashvalue = HashFunction(tablename, table_schema);
		lookup_table.add(tablename, table_schema, hashvalue);
	}

	void createRow(DataVariant row, TableSchema schema)
	{
		DataTableRecord newrow = new DataTableRecord();
		if(checkSchema(row, schema))
			newrow.add(TervDataVariant(row)); //typecast row
		else
			std::cout << "Error, row does not match table schema";
	}

	boolean checkSchema(DataVariant row, TableSchema schema)
	{
		if(row.size = schema.size)
		{
			for(autoptr : row)
			{
				if(!(row.type = schema.type))
					return false;
			}
		}
		return true;
	}

	DataTable *getTable(char *tablename)
	{
		DataTable *table = lookup_table.get(tablename);
		return table; //if table is not found then it should be NULL or false
	}


	DataTableRecord *getRow(char *tablename, int rowID)
	{
		DataTable *table = getTable(tablename);
		if(table != NULL)
		{
			return table.get(rowID);
		}
	}

	DataVariant *getData(char *tablename, int rowID, int columnID)
	{
		DataTableRecord *row = getRow(tablename, rowID);
		if(row != NULL)
		{
			return row.get(columnID);
		}
	}
	TableSchema *getSchema(char *tablename)
	{
		SchemaTablePair *table = lookup_table.get(tablename);
		if(table != NULL)
		{
			return table.get(second); //returns schema
		}
	}
*/

	int main()
	{
		std::cout << "Hello world";
	}
};

