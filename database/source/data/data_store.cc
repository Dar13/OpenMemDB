#include "data/data_store.h"
#include <iostream>


//creates an empty table
void DataStore::createTable(CreateTableCommand table_info)
{
    if(table_info.columns.size() > 0) //this should be <= correct?
    {
        // TODO: Returns an error code, statement cannot execute
        return;
    }

    TableSchema* schema = new TableSchema;

    // This code assumes that table_info.column_constraints uses 
    // SQLConstraintType.SQL_NO_CONSTRAINT when a column doesn't
    // have a constraint.
    for(uint32_t i = 0; i < table_info.columns.size(); i++)
    {
        TervelWrapper<SQLColumn> terv_column = table_info.columns[i];

        // TODO: Handle return value
        schema->columns.push_back(terv_column);
    }

    DataTable* new_table = new DataTable();

    SchemaTablePair pair = { new_table, schema};

    table_name_mapping.insert( table_info.table_name, pair);

}

DataStore::ERROR DataStore::deleteTable(std::string table_name)
{
	//returns false is the key cannot be found or access_counter is non-zero
	if(!table_name_mapping.remove(table_name))
		return MISSING_TABLE;
	//TODO: may need to return a success code
}

DataTable *getTable(std::string table_name)
{
	//Tervel hashmap seems to use get_position for the key value
	//at seems to be the function to use but the valueaccessor parameter 
	//wants the address of the value

	return &table_name_mapping.get(table_name); //returns pair
}

TableSchema *getTableSchema(std::string table_name)
{
	//assuming getTable works
	return &(getTable(table_name).second; //returns the schema from the pair
	//TODO: need error checking
}

DataTableRecord& getRow(std::string table_name, uint32_t row_id)
{
	return &(getTable(table_name).first.at(row_id));
}

//returns a column based off the column_id offset using table schema
//this returns an vector that is built from rows, this needs to be deleted
//afterward
DataTableColumn& getColumn(std::string table_name, uint32_t column_id)
{
	DataTable *table = getTable(table_name);
	int table_size = table.size();
	DataTableColumn column = new DataTableColumn(); 
	for(uint32_t i = 0; i < table_size; i++)
	{
		//finds the row and inserts the data from column_id into column vector
		column.push_back(getRow(table_name, i).at(column_id));
	}
	return &column; //this needs to be deleted and/or returns
			//in such a way that there is no memory leak 
}

void insertRow(std::string table_name, DataTableRecord row)
{
	DataTable *table = getTable(table_name);
	table.push_back(row);
}


