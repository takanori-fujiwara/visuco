#!/bin/bash -x
#COBALT --disable_preboot

#export L1P_POLICY=std
#export BG_THREADLAYOUT=1   # 1 - default next core first; 2 - my core first

#Free bootable blocks
boot-block --reboot

NODES=$1

PROG=miniMD

for iter in 1
do
 for ppn in 1 4 16
 do
  for TYPE in w s1 s2
  do
		echo
		echo "* * * * *"
		echo

        DIR=${PROG}_n${NODES}_c${ppn}_${TYPE}
        SRCDESTFILE=${PROG}_n${NODES}_c${ppn}_${TYPE}_srcdest.txt
        RTRAWFILE=${PROG}_n${NODES}_c${ppn}_${TYPE}_routes.out
        RTFILE=${PROG}_n${NODES}_c${ppn}_${TYPE}_routes.txt

		ARG="  -i input/in.lj.miniMD.$NODES.$ppn.$TYPE"
		RANKS=`echo "$NODES*$ppn"|bc`
		OUTPUT=${PROG}_N${NODES}_R${ppn}_${TYPE}
		echo "Starting $PROB $iter $OUTPUT on $RANKS processes ($NODES nodes) with ARG=$ARG"
		runjob --np $RANKS -p $ppn --block $COBALT_PARTNAME --verbose=INFO --envs "PAMID_COLLECTIVES_MEMORY_OPTIMIZED=1" --envs "TAU_TRACK_MESSAGE=1" --envs "TAU_THROTTLE=0" --envs "TAU_COMM_MATRIX=1" : ${PROG} ${ARG}

        echo "Finished running miniMD"

        mkdir $DIR

        mv profile.* $DIR

        # grep "sent to node" <dirname>/profile.* | grep -v MARKER | grep -i send | awk '{print $1,$6}' | awk -F'.' '{print $2,$4}' | awk '{print $1,$3}' | sort -n | uniq > file
        grep "sent to node" ${DIR}/profile.* | grep -v MARKER | grep -i send | awk '{print $1,$6}' | awk -F'.' '{print $2,$4}' | awk '{print $1,$3}' | sort -n | uniq > ${SRCDESTFILE}

        # bgqoute
        #runjob --np $RANKS -p $ppn --block $COBALT_PARTNAME --verbose=INFO : multiroutes file > $<unique id
        runjob --np $RANKS -p $ppn --block $COBALT_PARTNAME --verbose=INFO : multiroutes $SRCDESTFILE > $RTRAWFILE

        echo "Finished running multiroutes"

        grep "Hop" $RTRAWFILE > $RTFILE

		echo
		echo "* * * * *"
		echo
  done
 done
done

exit
