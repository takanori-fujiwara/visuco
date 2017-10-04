#!/bin/bash
#COBALT -t 10
#COBALT -q default
#COBALT -A  EarlyPerf_theta 

echo "COBALT_JOBID" $COBALT_JOBID
echo "COBALT_PARTNAME" $COBALT_PARTNAME
echo "COBALT_JOBSIZE" $COBALT_JOBSIZE

export MPICH_RANK_REORDER_DISPLAY=true

depth=1
rpn=1
FILE=miniAMR_n${COBALT_JOBSIZE}_c${rpn}_s2_srcdest.txt 
echo $FILE

echo ${COBALT_PARTNAME} > location.txt
python parsenodes.py theta.computenodes location.txt > nodetable

#this file will give you rank to node mapping, if required
python parsejobnodes.py theta.computenodes location.txt > jobnodetable

#change the file name to whatever, this contains information about Hops
#requires nodetable, and tau output file $FILE
OUTPUT="hopinfo"
aprun -n $((COBALT_JOBSIZE*rpn)) -N $rpn -d $depth -j 1 -cc depth ./multiroutes $FILE > $OUTPUT

status=$?
exit $status

