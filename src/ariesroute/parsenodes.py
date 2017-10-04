import os
import sys

def readGlobalConfiguration_(): #f, node_id, rank):

  #f = open("theta.computenodes", "r")
  f = open(nodeconfig, "r")
  cab_position, cab_row, cage, slot, cpu, processor_id = '', '', '', '', '', ''

  while True:
    line = f.readline()
    if not line: break
    words = line.split(',')
    for word in words:
      if 'cab_position' in word:
       cab_position=word.split('=')[1]
      elif 'cab_row' in word:
       cab_row=word.split('=')[1]
      elif 'cage=' in word:
       cage=word.split('=')[1]
      elif 'slot=' in word:
       slot=word.split('=')[1]
      elif 'cpu=' in word:
       cpu=word.split('=')[1]
      elif 'processor_id=' in word:
       processor_id=word.split('=')[1]

    print -1, processor_id, cab_position, cab_row, cage, slot, cpu
  f.close()

#3572-3579,3727-3734
def readAllocation_():
  
  f = open(nidstringfile, "r")
  nidstring = f.readline()
  f.close()

	#parse
  nodegroups = []
  if nidstring.find(',') == -1:
    nodegroups.append(nidstring)
  else:
    nodegroups = nidstring.split(',')

  rank = -1
  for group in nodegroups:
    nodes = []
    if group.find('-') == -1:
      nodes.append(group)
      nodes.append(group) #to generalize the below loop
    else:
      nodes = group.split('-')
    
    for node in range(int(nodes[0]),int(nodes[1])+1):
      rank = rank + 1
      #readGlobalConfiguration_(f, node, rank)
      #print node

#config file
nodeconfig = sys.argv[1]
#current allocation
nidstringfile = sys.argv[2] 

readGlobalConfiguration_()
#readAllocation_()

