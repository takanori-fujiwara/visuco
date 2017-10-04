import os
import time
from subprocess import *

nodes = [512]

def runcmd (node):
	script = './run.sh ' + str(node)
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

# def editInLjMiniMD(node, core, type): # type: weak, strong1, strong2

for iter in range (2, 3):
 for node in nodes:
		print '\nStarting ' + 'on ' + str(node) + ' nodes'
		jobid = runcmd(node)
