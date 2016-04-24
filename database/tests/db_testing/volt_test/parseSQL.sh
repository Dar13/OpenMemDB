#!/bin/bash

filename="../../sql_statements.omdbt"
outputFile="input.txt"
#declare -a create
declare -a input
flag=""

echo "Reading SQL statements"
#Reads file and determines how each line will be stored based on flags
while read line
do
    #Set up flag to determine how the read line will be used
    case "$line" in 
        "create" ) flag="create" ;;
        "insert" ) flag="insert" ;;
        "select" ) flag="select" ;;
        "drop" ) flag="drop" ;;
        "mixed" ) flag="mixed" ;;
    esac

    # parse flags so input only contains insert and select
    if [ "$flag" == "insert" ] || [ "$flag" == "select" ]  || [ "$flag" == "mixed" ] && [ "$line" != "$flag" ] && [ "$line" != "end" ]; then
        echo $line >> $outputFile
    fi
done < $filename
echo "Done reading SQL statements"
