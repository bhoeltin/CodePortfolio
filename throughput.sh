#!/bin/sh                                                                                                                                                           

echo "| METHOD | PROCESSES |     1KB   |   1MB  |     1GB     |"
echo "|--------|-----------|-----------|--------|-------------|"

for num in 1 2 4
do
        DIRS=$(./thor.py -r 10 -p "$num"  http://student02.cse.nd.edu:9890/test.txt | tail -1 | cut -d : -f 2 | sed -e "s/ //g")
        STATS=$(./thor.py -r 10 -p "$num" http://student02.cse.nd.edu:9890/officer.txt | tail -1 | cut -d : -f 2 | sed -e "s/ //g")
        CGIS=$(./thor.py -r 10 -p "$num" http://student02.cse.nd.edu:9890/sample.txt | tail -1 | cut -d : -f 2 | sed -e "s/ //g")

        printf "| SINGLE | %9d | %9s | %6s | %11s |\n" "$num" "$DIRS" "$STATS" "$CGIS"
done

