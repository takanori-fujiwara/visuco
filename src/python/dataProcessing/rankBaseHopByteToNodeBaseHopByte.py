import argparse

# parse args
ap = argparse.ArgumentParser()
ap.add_argument('-hf', '--hopByteFile', help='hopbyte file', type=open, required=True)
ap.add_argument('-tf', '--tableFile', help='job-node table file', type=open, required=True)
ap.add_argument('-o', '--out', help='output file', type=str, default='out.txt')
args = ap.parse_args()

of = open(args.out, 'w')

# load table to dic
jobToNode = {}
for line in args.tableFile:
    elems = line.replace("\n", "").split(' ')
    if(len(elems) >= 2):
        job = elems[0]
        node = elems[1]
        jobToNode[job] = node

for line in args.hopByteFile:
    elems = line.replace("\n", "").split(' ')
    if(len(elems) >= 3):
        sourceNode = jobToNode[elems[0]]
        destNode = jobToNode[elems[1]]
        if (len(elems) == 3):
            of.write(str(sourceNode) + " " + str(destNode) + " " + str(elems[2]) + "\n")
        elif (len(elems) == 4):
            of.write(str(sourceNode) + " " + str(destNode) + " " + str(elems[2]) + str(elems[3]) + "\n")

of.close()
args.tableFile.close()
args.hopByteFile.close()
