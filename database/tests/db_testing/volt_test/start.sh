# Start up script to initalize database

# In order to start a cluster, for each node in the cluster they must be started as a seperate process with their own unique ports!!!
# This script creates multiple directories for each node in the cluster with the exact same deployment file. This is useful when nodes crash and their logs are saved into their own directory
# The script works as so
# 1.) Start up local host
# 2.) Start up each node and use localhost as their host using the host unique port
# 3.) The script should end with "Server initalization complete"

# Create multiple node directories with deployment.xml file included

# TODO: Make size adjustable for benchmarking script [1, 2, 4, 8, ...] arguments, etc
size=2

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

# TODO: Command needed to be executed during ssh connection to server, each connection need their own voltdb database
# Create host voltdb process
#VOLTDB_OPTS="-Dvolt.rmi.agent.port=$jmx" ~/voltdb/bin/voltdb create -H localhost:$host --internal=$internal --http=$http --admin=$admin --client=$client --zookeeper=$zookeeper --deployment=deployment.xml

#Create a deattached screen
screen -d -m -S nodes

# Create multiple screen windows and run multiple voltdb instances for cluster
for ((j=1; j <= $size; j++))
do
    #Create multiple terminals and run commands
    screen -S nodes -X screen $j
    screen -t $j 2 bash -c 'ls ; $j' #VOLTDB_OPTS="-Dvolt.rmi.agent.port=$jmx" ~/voltdb/bin/voltdb create -H localhost:$host --internal=$internal --http=$http --admin=$admin --client=$client --zookeeper=$zookeeper --deployment=deployment.xml


    # Update ports
    http=$((http+offset))
    admin=$((admin+offset))
    client=$((client+offset))
    internal=$((internal+offset))
    jmx=$((jmx+offset))
    zookeeper=$((zookeeper+offset))

done

# Host is running and each node should have connected to host

