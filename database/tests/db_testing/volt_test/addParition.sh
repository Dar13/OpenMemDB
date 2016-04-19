#FOR PERFORMANCE TESTING PURPOSES

size=$(wc -l create.txt | awk '{print $1}')
for((i=0; i < size; i++))
do
    name="TestT$i"
    printf "%s\n" "PARTITION TABLE $name ON B;" >> create.txt
    printf "%s\n" "CREATE PROCEDURE insert PARTITION ON TABLE $name COLUMN B AS INSERT INTO TestT0 VALUES (?,?);" >> create.txt
    printf "%s\n" "CREATE PROCEDURE find PARTITION ON TABLE $name COLUMN B AS SELECT TestT0.B FROM TestT0 WHERE TestT0.B=?;" >> create.txt
done

