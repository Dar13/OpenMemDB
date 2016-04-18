# A script that stops the server by using voltadmin shutdown -H localhost:"adminport"
# Admin port should be a default value

size=$1
admin=21211
~/voltdb/bin/voltadmin shutdown -H localhost:$admin

#offset=1000
#for((i=0; i < size; i++))
#do
#    ~/voltdb/bin/voltadmin shutdown -H localhost:$((admin+i*offset))
#done
rm -rf ~/OpenMemDB/database/tests/db_testing/volt_test/voltdbroot
rm -rf ~/OpenMemDB/database/tests/db_testing/volt_test/log

#screen -d nodes

#Clean up detached screens
#screen -ls | grep Detached | cut -d. -f1 | awk '{print $1}' | xargs kill
