#!/bin/bash

#Remove DATE from Openmemdb test
filename="create.txt"

while read line
do
    perl -pi -e 's/DATE/TIMESTAMP/g' create.txt
done < $filename

