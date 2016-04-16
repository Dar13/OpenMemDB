#Benchmark script that will use start, stop scripts and run main.java to get the execution times

# Steps
# run start up script at 1 node
# assumes that openmemdb input file is updated
# run java program at 1 node
# run stop script and redo using new node value

cap=2
for((i=0; i < cap; i++))
do
    var=$((2**i))
    ./start.sh $var
    sleep 10
    #make clean && make && java -classpath ".:/home/heyblackduck/workspace/voltdb/voltdb/*" main 2
    ./stop.sh
    sleep 5
done
