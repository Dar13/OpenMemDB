#Benchmark script that will use start, stop scripts and run main.java to get the execution times

# Steps
# run start up script at 1 node
# assumes that openmemdb input file is updated
# run java program at 1 node
# run stop script and redo using new node value
cap=6
declare -a flags=(irt)
#ct st irt


#Compile Java Application
make clean && make

#Clean up directory and recompile
#rm -f catalog.jar
#jar cvf catalog.jar main.class connector.class thread.class insert.class find.class

#Need to clean up before executing tests
rm -f execTime.txt create.txt select.txt drop.txt insert.txt

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
    
    #Replace DATE with TIMESTAMP
    #./parse.sh
    
    #Add partition tables to create.txt
    #./addPartition.sh

    #----------------Starts Cluster and Executes Java App and Stops Cluster-------------------------
    # stops are necessary since cluster start/stop takes time to execute
    echo "Executing VoltDB Test for $i"
    for((j=cap; j >= 0; j--))
    do
        var=$((2**j))
        ./start.sh 1
        sleep 10

        #Insert procedures
        #printf "%s\n" "CREATE PROCEDURE find AS SELECT TestT0.B FROM TestT0 WHERE TestT0.B=?; CREATE PROCEDURE insert AS INSERT INTO TestT0 VALUES (?,?);" >> create.txt
        #/home/OpenMemDb/voltdb/bin/sqlcmd --port=12002 load classes /home/OpenMemDb/OpenMemDB/database/tests/db_testing/volt_test/catalog.jar

        #Load Database With Schema
        #/home/OpenMemDb/voltdb/bin/sqlcmd --port=12002 < create.txt

        java -classpath ".:/home/OpenMemDb/voltdb/voltdb/*" main 1 $var
        echo "Stopping Cluster"
        ./stop.sh
        sleep 10
    done
    echo "Done executing"
    rm -f create.txt insert.txt select.txt drop.txt
done

