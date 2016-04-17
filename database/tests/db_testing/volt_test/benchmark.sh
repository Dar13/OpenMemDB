#Benchmark script that will use start, stop scripts and run main.java to get the execution times

# Steps
# run start up script at 1 node
# assumes that openmemdb input file is updated
# run java program at 1 node
# run stop script and redo using new node value
cap=1
declare -a flags=(ct st irt)

#Compile Java Application
make clean && make

#Execute all OpenMemDB Flag Tests
for i in ${flags[@]}
do
    #Write header for type of test
    head -n 1 ../../sql_statements.omdbt > execTime.txt

    #Execute OpenMemDB tests
    /home/OpenMemDb/OpenMemDB/database/tests/tests sqltf $i

    #Parse SQL Statements
    ./voltdb.sh
    
    echo "Executing $i"
    for((i=0; i < cap; i++))
    do
        var=$((2**i))
        ./start.sh $var
        sleep $((30*var))
        java -classpath ".:/home/OpenMemDb/voltdb/voltdb/*" main $var
        ./stop.sh
        sleep $((30*var))
    done

    rm -f create.txt insert.txt select.txt drop.txt
done


