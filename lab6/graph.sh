#!/bin/bash

FILE1='./p3_output.csv'
FILE2='./p4_output.csv'
PRECS='0.1 0.05 0.01 0.005 0.001 0.0005 0.0001 0.00005 0.00001 0.000005 0.000001 0.0000005 0.0000001'
GNUPLOT="set datafile sep ','; set logscale x; plot '$FILE1' u 1:4 w lines, '$FILE2' u 1:4 w lines"

# Clear out files if they exist
rm -f $FILE1 $FILE2

# Call p3, p4 with increasing values for precision
for i in $PRECS
do
    echo "Running at precision $i..."
    mpirun -np 200 ./p3 3.0 5.0 $i
    mpirun -np 200 ./p4 3.0 5.0 $i
    echo

done

# Graph em!
gnuplot -persist -e "$GNUPLOT"
