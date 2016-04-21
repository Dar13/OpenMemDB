#Runs java app

#Limits number of cores used
if [ "$(id -u)" != "0" ]; then
    echo "Sorry, you need to be root."
    exit 1
fi


set_num_cpus()
{
    arg=$1
    dec=1

    taskset -c -a -p 0-$((arg - dec)) $(pidof java) > /dev/null
    #taskset -c -a -p 0-$((arg - dec)) $(pidof java) > /dev/null
}

# Runs java App
set_num_cpus $1
java -classpath ".:/home/OpenMemDb/voltdb/voltdb/*" Run 1 $1
