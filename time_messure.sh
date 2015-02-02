#!/bin/bash
MAX_N=4;
CHUNKSTART=1;
CHUNKSTEP=10;
CHUNKEND=100;

if [[ $# -ge 1 ]]; then
    MAX_N=$1;
fi

if [[ $# -eq 4 ]]; then
    CHUNKSTART=$2;
    CHUNKSTEP=$3;
    CHUNKEND=$4;
fi

echo max_procs: $MAX_N chunkstart: $CHUNKSTART chunkstep: $chunkstep chunkend: $CHUNKEND

for n in `seq 1 1 $MAX_N`; do
    innerN=$n
    for n in `seq $CHUNKSTART $CHUNKSTEP $CHUNKEND`; do
        START_TIME=$SECONDS
        chunksize=$((1024*1024*$n))
        mpirun -n $innerN bin/mpi_searcher -s 23.75.241.41 -f ./testfile -c $chunksize
        ELAPSED_TIME=$(($SECONDS - $START_TIME))
        echo process count: $innerN  --  chunkzsize: $chunksize byte  --  time : $ELAPSED_TIME s
    done
done

