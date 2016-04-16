# This script stops multiple processes from each terminal in screen session and terminates screen

#Delete deattached screens, maybe safe?? each screen shouldn't have anything running
screen -ls | grep Detached | cut -d. -f1 | awk '{print $1}' | xargs kill
