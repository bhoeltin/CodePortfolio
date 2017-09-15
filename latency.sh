#!/bin/sh

echo "| METHOD | PROCESSES | DIRECTORY | STATIC | CGI SCRIPTS |"
echo "|--------|-----------|-----------|--------|-------------|"

for num in 1 2 4
do
	DIRS=$(./thor.py -r 10 -p "$num"  http://student00.cse.nd.edu:9898 | tail -1 | cut -d : -f 2 | sed -e "s/ //g")
	STATS=$(./thor.py -r 10 -p "$num" http://student00.cse.nd.edu:9898/text/hackers.txt | tail -1 | cut -d : -f 2 | sed -e "s/ //g")
	CGIS=$(./thor.py -r 10 -p "$num" http://student00.cse.nd.edu:9898/scripts/cowsay.sh | tail -1 | cut -d : -f 2 | sed -e "s/ //g")
	
	printf "| SINGLE | %9d | %9s | %6s | %11s |\n" "$num" "$DIRS" "$STATS" "$CGIS"
done

