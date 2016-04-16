# A script that stops the server by using voltadmin shutdown -H localhost:"adminport"
# Admin port should be a default value

admin=7001
# ~/workspace/voltdb/bin/voltadmin shutdown -H localhost:$admin
rm -r ~/workspace/OpenMemDB/database/tests/db_testing/volt_test/voltdbroot
rm -r ~/workspace/OpenMemDB/database/tests/db_testing/volt_test/log

screen -d nodes

#Clean up detached screens
screen -ls | grep Detached | cut -d. -f1 | awk '{print $1}' | xargs kill
