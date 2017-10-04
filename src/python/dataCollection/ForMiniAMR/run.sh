#!/bin/bash -x
#COBALT --disable_preboot

cwd=`dirname "${0}"`

#export L1P_POLICY=std
#export BG_THREADLAYOUT=1   # 1 - default next core first; 2 - my core first

#Free bootable blocks
boot-block --reboot

NODES=$1

PROG=miniAMR # this made with TAU
PROG2=miniAMR.x # this made with HPCTW for obtaining trace result

CURRENT=`pwd`
BASENAME=`basename "$CURRENT"`

for iter in 1
do
 for TYPE in s1
 do
  #for size in 10 40 80 120 160 200 240 280
  for maxblockspe in 4000
  do
   for ppn in 1 4 16
   do
	echo
	echo "* * * * *"
	echo
	RANKS=`echo "$NODES*$ppn"|bc`

    DIR=${PROG}_n${NODES}_c${ppn}_${TYPE}
    SRCDESTFILE=${PROG}_n${NODES}_c${ppn}_${TYPE}_srcdest.txt
    RTRAWFILE=${PROG}_n${NODES}_c${ppn}_${TYPE}_routes.out
    RTFILE=${PROG}_n${NODES}_c${ppn}_${TYPE}_routes.txt
    HBFILE=${PROG}_n${NODES}_c${ppn}_${TYPE}_hopbytes.txt

	#npx=`echo "${NODES}/4"|bc`
	if [ $RANKS -eq 32 ]; then
        nx=2
        ny=2
        nz=1
        npx=4
        npy=4
        npz=2
    elif [ $RANKS -eq 64 ]; then
        nx=2
        ny=2
        nz=2
        npx=4
        npy=4
        npz=4
    elif [ $RANKS -eq 128 ]; then
        nx=4
        ny=2
        nz=2
        npx=8
        npy=4
        npz=4
	elif [ $RANKS -eq 256 ]; then
        nx=4
        ny=4
        nz=2
		npx=8
		npy=8
		npz=4
	elif [ $RANKS -eq 512 ]; then
        nx=4
        ny=4
        nz=4
		npx=8
		npy=8
		npz=8
	elif [ $RANKS -eq 1024 ]; then
        nx=8
        ny=4
        nz=4
		npx=16
		npy=8
		npz=8
	elif [ $RANKS -eq 2048 ]; then
        nx=8
        ny=8
        nz=4
		npx=16
		npy=16
		npz=8
	elif [ $RANKS -eq 4096 ]; then
        nx=8
        ny=8
        nz=8
		npx=16
		npy=16
		npz=16
	elif [ $RANKS -eq 8192 ]; then
        nx=16
        ny=8
        nz=8
		npx=32
		npy=16
		npz=16
	elif [ $RANKS -eq 16384 ]; then
        nx=16
        ny=16
        nz=8
		npx=32
		npy=32
		npz=16
	elif [ $RANKS -eq 32768 ]; then
        nx=16
        ny=16
        nz=16
		npx=32
		npy=32
		npz=32
	fi

    if [ $TYPE = s1 ]; then
        nx=2
        ny=2
        nz=2
    elif [ $TYPE = s2 ]; then
        nx=8
        ny=8
        nz=8
    fi

    #2 moving spheres
    #ARGS="--num_refine 4 --max_blocks $maxblockspe --init_x 1 --init_y 1 --init_z 1 --npx $npx --npy $npy --npz $npz --nx $nx --ny $ny --nz $nz --num_objects 2 --object 2 0 -1.10 -1.10 -1.10 0.030 0.030 0.030 1.5 1.5 1.5 0.0 0.0 0.0 --object 2 0 0.5 0.5 1.76 0.0 0.0 -0.025 0.75 0.75 0.75 0.0 0.0 0.0 --num_tsteps 100 --stages_per_ts 16 --report_perf 9"
    ARGS="--num_refine 1 --max_blocks $maxblockspe --init_x 1 --init_y 1 --init_z 1 --npx $npx --npy $npy --npz $npz --nx $nx --ny $ny --nz $nz --num_objects 2 --object 2 0 -1.10 -1.10 -1.10 0.030 0.030 0.030 1.5 1.5 1.5 0.0 0.0 0.0 --object 2 0 0.5 0.5 1.76 0.0 0.0 -0.025 0.75 0.75 0.75 0.0 0.0 0.0 --num_tsteps 20 --stages_per_ts 10 --report_perf 9"
    OUTPUT=${PROG}_${iter}_${maxblockspe}_N${NODES}_R${ppn}
    ENVS="PAMID_COLLECTIVES_MEMORY_OPTIMIZED=1"

    echo "Starting $PROB $iter $OUTPUT on $RANKS processes ($NODES nodes) with ARG=$ARGS"

    # manual mapping is set by file or not
    if [ -f mapping_n${NODES}_c${ppn}_${TYPE}.txt ];
    then
     # TAU
     runjob --np $RANKS -p $ppn --block $COBALT_PARTNAME --mapping mapping_n${NODES}_c${ppn}_${TYPE}.txt --verbose=INFO --envs ${ENVS} --envs TAU_TRACK_MESSAGE=1 --envs TAU_THROTTLE=0 --envs TAU_COMM_MATRIX=1 : ${PROG} ${ARGS} > $OUTPUT
     # HPCTW
     runjob --np $RANKS -p $ppn --block $COBALT_PARTNAME --mapping mapping_n${NODES}_c${ppn}_${TYPE}.txt --verbose=INFO --envs ${ENVS} --envs OUTPUT_ALL_RANKS=yes --envs SAVE_ALL_TASKS=yes --envs TRACE_SEND_PATTERN=yes : ${PROG2} ${ARGS} > $OUTPUT
    else
     # TAU
     runjob --np $RANKS -p $ppn --block $COBALT_PARTNAME --verbose=INFO --envs ${ENVS} --envs TAU_TRACK_MESSAGE=1 --envs TAU_THROTTLE=0 --envs TAU_COMM_MATRIX=1 : ${PROG} ${ARGS} > $OUTPUT
     # HPCTW
     runjob --np $RANKS -p $ppn --block $COBALT_PARTNAME --verbose=INFO --envs ${ENVS} --envs OUTPUT_ALL_RANKS=yes --envs SAVE_ALL_TASKS=yes --envs TRACE_SEND_PATTERN=yes : ${PROG2} ${ARGS} > $OUTPUT
     # make blanck mapping file
     echo '' > mapping_${NODES}_${ppn}_${TYPE}.txt
    fi

    echo "Finished running miniAMR"

    mkdir ${DIR}_tau
    mv profile.* ${DIR}_tau

    mkdir ${DIR}_pattern
    mv pattern.* ${DIR}_pattern

    mkdir ${DIR}_mpiprofile
    mv mpi_profile.* ${DIR}_mpiprofile

    # output src dest
    grep "sent to node" ${DIR}_tau/profile.* | grep -v MARKER | grep -i send | awk '{print $1,$6}' | awk -F'.' '{print $2,$4}' | awk '{print $1,$3}' | sort -n | uniq > ${SRCDESTFILE}

    #bgqroute
    if [ -f mapping_n${NODES}_c${ppn}_${TYPE}.txt ];
    then
     runjob --np $RANKS -p $ppn --block $COBALT_PARTNAME --mapping mapping_n${NODES}_c${ppn}_${TYPE}.txt --verbose=INFO : multiroutes $SRCDESTFILE > $RTRAWFILE
    else
     runjob --np $RANKS -p $ppn --block $COBALT_PARTNAME --verbose=INFO : multiroutes $SRCDESTFILE > $RTRAWFILE
    fi

    echo "Finished running multiroutes"

    # output routes
    grep "Hop" $RTRAWFILE > $RTFILE

    # output hop-byte
    grep "<" ${DIR}_pattern/pattern.* | grep -v "MPI" | awk -F / '{print $2}' | awk '{print $1, $2, $4, $8}' | awk -F ":" '{print $1 $2}' | cut -c16- > ${HBFILE}
	echo
	echo "* * * * *"
	echo

    # output shapes for adj mapping
    grep "BGQ Partition" ${PROG}_n${NODES}_c${ppn}_${TYPE}_mpiprofile/mpi_profile.*.0 | awk -F "<" '{print $2}' | awk -F ">" '{print $1}' > ${PROG}_n${NODES}_c${ppn}_${TYPE}_mapping.csv
    echo "4,3,2,1,0" >> ${PROG}_n${NODES}_c${ppn}_${TYPE}_mapping.csv

   done
  done
 done
done

grep COBALT_PARTNAME *.cobaltlog | awk '{print $8}' | awk '{print substr($0, 17)}' > location.txt

# prepare for running remapping ver

mkdir ../${BASENAME}_remap

cp location.txt ../${BASENAME}_remap/.

cp ${PROG} ../${BASENAME}_remap/.

cp ${PROG2} ../${BASENAME}_remap/.

cp multiroutes ../${BASENAME}_remap/.

cp run.py ../${BASENAME}_remap/.

cp run.sh ../${BASENAME}_remap/.

cp mapping* ../${BASENAME}_remap/.

exit
