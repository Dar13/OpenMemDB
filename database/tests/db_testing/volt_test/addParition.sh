#FOR PERFORMANCE TESTING PURPOSES

size=$(wc -l create.txt | awk '{print $1}')
for((i=0; i < size; i++))
do
    printf "%s\n" "PARTITION TABLE TestT$i" >> create.txt
done

