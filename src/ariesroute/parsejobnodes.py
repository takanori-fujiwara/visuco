import os
import sys

def readGlobalConfiguration_(f, node_id, rank):

  #f = open("theta.computenodes", "r")
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

    if int(processor_id) == node_id:
      #print rank, processor_id, '('+cab_position, cab_row, cage, slot, cpu+')'
      print rank, processor_id, cab_position, cab_row, cage, slot, cpu
      break

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
    
    f = open(nodeconfig, "r")
    for node in range(int(nodes[0]),int(nodes[1])+1):
      rank = rank + 1
      readGlobalConfiguration_(f, node, rank)
      #print node
    f.close()

#config file
nodeconfig = sys.argv[1]
#current allocation
nidstringfile = sys.argv[2] 

#readGlobalConfiguration_()
readAllocation_()

