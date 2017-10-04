import os
import time
from subprocess import *

nodes = [32] #, 64, 128, 256, 512, 1024]

def runcmd (node):

	script = './run.sh ' + str(node) #+ ' ' + str(prob)
	cmd = 'qsub -A visualization -t 01:00:00 -n '+str(node)+' --mode script '+script
	print 'Executing ' + cmd
	jobid = Popen(cmd, shell=True, stdout=PIPE).communicate()[0]
	print 'Jobid : ' + jobid

	while True:
		cmd = 'qstat ' + jobid.strip() + ' | grep fujiwara | awk \'{print $1}\''
		jobrun = Popen(cmd, shell=True, stdout=PIPE).communicate()[0]
		if jobrun == '':
			break
		time.sleep(60)

	return jobid.strip()

for iter in range (1, 2):
 for node in nodes:
		print '\nStarting ' + ' on ' + str(node) + ' nodes' #+ str(rank)
		jobid = runcmd(node)
