#!/bin/bash

filename="../../sql_statements.omdbt"
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

    if [ "$flag" == "create" ] && [ "$line" != "$flag" ]; then
        create+=("$line")
    fi

        if [ "$flag" == "insert" ] && [ "$line" != "$flag" ]; then
        insert+=("$line")
    fi

    if [ "$flag" == "select" ] && [ "$line" != "$flag" ]; then
        select+=("$line")
    fi

    if [ "$flag" == "drop" ] && [ "$line" != "$flag" ]; then
        drop+=("$line")
    fi
    
done < $filename
printf "%s\n" "${create[@]}" > temp.txt
head -n -1 temp.txt > output.txt
rm -f temp.txt
