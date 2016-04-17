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
        <cluster hostcount=\"%s\" sitesperhost=\"%s\" kfactor=\"0\" />
        <httpd enabled=\"true\">
            <jsonapi enabled=\"true\" />
        </httpd>
    </deployment>\n" $size $size >> deployment.xml

# Voltdb Default Ports http=8080 admin=21211 client=21212 internal=3021 jmx=9090 zookeeper=7181 however these will be changed
# Host will use base port 7000 and go up by one for each port configuration 7000, 7001, 7002, ...
# Nodes will use host base port incremented by 1000 for each exisiting node, 8000, 9000, 10000, ... etc

# Host port necessary for nodes to connect to host
host=7010

#Temporary Node Port Arguments
http=7000
admin=7001
client=7002
internal=7010
jmx=7020
zookeeper=7030

offset=1000

#Create a deattached screen
screen -AdmS nodes -t 0 bash

# Create multiple SSH connections and connect them to host
for((j=1; j <= $size; j++))
do
    screen -S nodes -X screen -t $j
    screen -S nodes -p $j -X stuff "VOLTDB_OPTS="-Dvolt.rmi.agent.port=$jmx" ~/voltdb/bin/voltdb create -H localhost:$host --internal=$internal --http=$http --admin=$admin --client=$client --zookeeper=$zookeeper --deployment=deployment.xml \\r"

    # Update ports
    http=$((http+offset))
    admin=$((admin+offset))
    client=$((client+offset))
    internal=$((internal+offset))
    jmx=$((jmx+offset))
    zookeeper=$((zookeeper+offset))
done

echo "Cluster is done!\n"

