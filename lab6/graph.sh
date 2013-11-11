#!/bin/bash

FILE1='./p3_output.csv'
FILE2='./p4_output.csv'
PRECS='0.01 0.005 0.001 0.0005 0.0001 0.00005 0.00001 0.000005 0.000001'
GNUPLOT="set datafile sep ','; set logscale x; set logscale y; plot '$FILE1' u 1:4 w lines, '$FILE2' u 1:4 w lines"

# If files don't exist, create them
if ! ([ -f $FILE1 ] && [ -f $FILE2 ])
then
    # Call p3, p4 with increasing values for precision
    for i in $PRECS
    do
        echo "Running at precision $i..."
        mpirun -np 250 ./p3 3.0 5.0 $i
        mpirun -np 250 ./p4 3.0 5.0 $i
        echo

    done
fi

# Graph em!
gnuplot -persist -e "$GNUPLOT"
