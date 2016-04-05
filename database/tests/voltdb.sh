#!/bin/bash

filename="sql_statements.omdbt"
declare -a create
declare -a insert
declare -a select
declare -a drop
flag=""

#Reads file and determines how each line will be stored based on flags
while read line
do
    #Set up flag to determine how the read line will be used
    case "$line" in 
        "create" ) flag="create" ;;
        "insert" ) flag="insert" ;;
        "select" ) flag="select" ;;
        "drop" ) flag="drop" ;;
    esac

    if [ "$flag" == "create" ]; then
        create+=("$line")
    fi

    #do this for every other flag
    
done < $filename

echo $flag \ ${create[@]}
