#Sally
#Assignment 2
#Question 5

#!/bin/bash

#initialize variables
SUFFIX=$1
N=$2

find . -type f -name '*.'$SUFFIX -printf '%p %s\n' | sort -t " " -k 2 -n | head -$N | awk '{x += $2 ; printf "%s\t%d\n", $1, $2} END { print "Total size:\t" x}' 

