#Benchmark script that will use start, stop scripts and run main.java to get the execution times

# Steps
# run start up script at 1 node
# assumes that openmemdb input file is updated
# run java program at 1 node
# run stop script and redo using new node value
cap=1
declare -a flags=(st)
#ct st irt

#Compile Java Application
make clean && make

jar cvf catalog.jar main.class connector.class thread.class insert.class find.class
#Need to clean up before executing tests
rm -f execTime.txt create.txt select.txt drop.txt insert.txt catalog.jar

#Execute all OpenMemDB Flag Tests
for i in ${flags[@]}
do
    #Execute OpenMemDB tests
    cd ../..
    /home/OpenMemDb/OpenMemDB/database/tests/tests sqltf $i
    cd /home/OpenMemDb/OpenMemDB/database/tests/db_testing/volt_test/

    head -n 1 ../../sql_statements.omdbt >> execTime.txt

    #Parse SQL Statements
    ./voltdb.sh
    
    echo "Executing VoltDB Test for $i"
    for((i=0; i < cap; i++))
    do
        var=$((2**i))
        ./start.sh $var
        #sleep 120
        sleep 10
        printf "%s\n" "CREATE PROCEDURE find AS SELECT TestT0.B FROM TestT0 WHERE TestT0.B=?; CREATE PROCEDURE insert AS INSERT INTO TestT0 VALUES (?,?);" >> create.txt
        #/home/OpenMemDb/voltdb/bin/sqlcmd < "load classes /home/OpenMemDb/database/tests/db_testing/volt_test/catalog.jar"
        /home/OpenMemDb/voltdb/bin/sqlcmd --port=12002 < create.txt
        #sleep 30
        #sleep $((30*(i+1)))
        #java -classpath ".:/home/OpenMemDb/voltdb/voltdb/*" main $var
        echo "Stopping Cluster"
        ./stop.sh
        #sleep 120
        #sleep 30
        #sleep $((30*(i+1)))
    done
    echo "Done executing"
    rm -f create.txt insert.txt select.txt drop.txt
done

