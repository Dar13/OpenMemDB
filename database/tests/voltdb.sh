filename="sql_statements.omdbt"
declare -a create
declare -a insert
declare -a select
declare -a drop
flag=""
test=""

#Reads file and determines how each line will be stored based on flags
test=$(head -n 1 $filename)
echo $test
while read line
do
    #Set up flag to determine how the read line will be used
    case "$line" in 
        "create" ) flag="create" ;;
        "insert" ) flag="insert" ;;
        "select" ) flag="select" ;;
        "drop" ) flag="drop" ;;
        *) continue
    esac

    #Copies data into array depending on flag and makes sure that flag is not copied as well
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

#print array
printf "%s\n" "${create[@]}" "${insert[@]}" "${select[@]}" "${drop[@]}" >> output.sql

#TODO: start voltdb

if [ $test == "create_test" ]; then
    startTime=$(date +%s%N)/1000 
    while 

