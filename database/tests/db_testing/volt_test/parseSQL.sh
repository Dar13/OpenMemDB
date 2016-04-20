#!/bin/bash

filename="../../sql_statements.omdbt"
#declare -a create
declare -a insert
declare -a select
declare -a drop
declare -a mixed
flag=""

echo "Reading SQL statements"
#Reads file and determines how each line will be stored based on flags
while read line
do
    #Set up flag to determine how the read line will be used
    case "$line" in 
        #"create" ) flag="create" ;;
        "insert" ) flag="insert" ;;
        "select" ) flag="select" ;;
        "drop" ) flag="drop" ;;
        "mixed" ) flag="mixed" ;;
    esac

    #if [ "$flag" == "create" ] && [ "$line" != "$flag" ] && [ "$line" != "end" ]; then
    #    create+=("$line")
    #fi

    if [ "$flag" == "insert" ] && [ "$line" != "$flag" ] && [ "$line" != "end" ]; then
        insert+=("$line")
    fi

    if [ "$flag" == "select" ] && [ "$line" != "$flag" ] && [ "$line" != "end" ]; then
        select+=("$line")
    fi

    if [ "$flag" == "drop" ] && [ "$line" != "$flag" ] && [ "$line" != "end" ]; then
        drop+=("$line")
    fi

    if [ "$flag" == "mixed" ] && [ "$line" != "$flag" ] && [ "$line" != "end" ]; then
        mixed+=("$line")
    fi
done < $filename
echo "Done reading SQL statements"

#len=${#create[@]}
#if [[ $len > 0 ]]
#then
#    printf "%s\n" "${create[@]}" > create.txt
#fi
len=${#insert[@]}
if [[ $len > 0 ]]
then
    printf "%s\n" "${insert[@]}" > insert.txt
fi
len=${#select[@]}
if [[ $len > 0 ]]
then
    printf "%s\n" "${select[@]}" > select.txt
fi
len=${#drop[@]}
if [[ $len > 0 ]]
then
    printf "%s\n" "${drop[@]}" > drop.txt
fi
len=${#mixed[@]}
if [[ $len > 0 ]]
then
    printf "%s\n" "${mixed[@]}" > mixed.txt
fi

