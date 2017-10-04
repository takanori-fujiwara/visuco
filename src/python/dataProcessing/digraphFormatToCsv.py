import argparse

# parse args
ap = argparse.ArgumentParser()
ap.add_argument('-f', '--file', help='digraph dot file', type=open, required=True)
ap.add_argument('-o', '--out', help='output file', type=str, default='out.csv')
args = ap.parse_args()

of = open(args.out, 'w')
for line in args.file:
    sourceAndOthers = line.split('->')
    if(len(sourceAndOthers) > 1):
        source = int(sourceAndOthers[0])
        if(line.find("[") >= 0): # when included weight
            targetAndOthers = sourceAndOthers[1].split("[");
            target = int(targetAndOthers[0])
            penwidth = int(targetAndOthers[1].split("penwidth=")[1].split("]")[0]);
            of.write(str(source) + "," + str(target) + "," +str(penwidth) + "\n")
        else: # when not included weight
            target = int(sourceAndOthers[1].split(";")[0])
            penwidth = -1
            of.write(str(source) + "," + str(target) + "," + str(penwidth) + "\n")
of.close()
args.file.close()
