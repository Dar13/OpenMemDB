if [[ $# -eq 0 ]]; then
    echo "Need an argument"
    exit 0
fi

testFlag=$1

#Compile Java Application
echo "Removing old class files and compiling Java"
make clean && make

#Clean up directory and recompile
echo "Removing old catalog jar file"
rm -f catalog.jar
echo "Compiling stored procedure into catalog.jar"
jar cvf catalog.jar *.class

printf "%s\n" "New Test Run: " >> execTime.txt

echo "removed old text files"
rm -f select.txt drop.txt insert.txt mixed.txt

echo "Running OpenMemDB tests"
cd ../..
/home/OpenMemDb/OpenMemDB/database/tests/tests sqltf $testFlag -c
cd /home/OpenMemDb/OpenMemDB/database/tests/db_testing/volt_test/

echo "Spliting sql instructions into seperate files"
./parseSQL.sh

