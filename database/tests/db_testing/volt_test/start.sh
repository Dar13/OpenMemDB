# Start up script to initalize database

# How to run script
# ./"scriptname" "number of nodes"

# In order to start a cluster, for each node in the cluster they must be started as a seperate process with their own unique ports!!!
# This script creates multiple directories for each node in the cluster with the exact same deployment file. This is useful when nodes crash and their logs are saved into their own directory
# The script works as so
# 1.) Start up local host
# 2.) Start up each node and use localhost as their host using the host unique port
# 3.) The script should end with "Server initalization complete"

# TODO: Make size adjustable for benchmarking script [1, 2, 4, 8, ...] arguments, etc
size=$1

if test -f "deployment.xml"; then rm -f deployment.xml; fi

#Default deployment file
printf "<?xml version=\"1.0\"?>
    <deployment>
        <cluster hostcount=\"%s\" sitesperhost=\"8\" kfactor=\"0\" />
        <httpd enabled=\"true\">
            <jsonapi enabled=\"true\" />
        </httpd>
        </deployment>\n" $size >> deployment.xml

# Voltdb Default Ports http=8080 admin=21211 client=21212 internal=3021 jmx=9090 zookeeper=7181 however these will be changed
# Host will use base port 12000 and go up by one for each port configuration
# Nodes will use host base port incremented by 1 for each exisiting node and use offset of 6 since each node uses 6 ports

# Host port necessary for nodes to connect to host
host=12003

#Temporary Node Port Arguments
http=12000
admin=12001
client=12002
internal=12003
jmx=12004
zookeeper=12010
rep=12020

offset=6

#Create a deattached screen
screen -AdmS nodes -t 0 bash

screen -S nodes -X screen -t 1
screen -S nodes -p 1 -X stuff "VOLTDB_OPTS="-Dvolt.rmi.agent.port=$jmx" ~/voltdb/bin/voltdb create -H localhost:$host --internal=$internal --http=$http --admin=$admin --client=$client --replication=$rep --zookeeper=$zookeeper --deployment=deployment.xml \\r"

# Create multiple SSH connections and connect them to host
for((j=2; j <= $size; j++))
do
    # Update ports
    http=$((http+offset))
    admin=$((admin+offset))
    client=$((client+offset))
    internal=$((internal+offset))
    jmx=$((jmx+offset))
    zookeeper=$((zookeeper+offset))
    rep=$((rep+offset))

    screen -S nodes -X screen -t $j
    screen -S nodes -p $j -X stuff "VOLTDB_OPTS="-Dvolt.rmi.agent.port=$jmx" ~/voltdb/bin/voltdb create -H localhost:$host --internal=$internal --http=$http --admin=$admin --client=$client --replication=$rep --zookeeper=$zookeeper --deployment=deployment.xml \\r"
done

echo "Cluster is setup!"

