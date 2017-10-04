###
### Dependency: graph-tool (https://graph-tool.skewed.de)
###   see https://graph-tool.skewed.de/download to install
###

import graph_tool.all as gt
import argparse

## Parse Command Line Args
ap = argparse.ArgumentParser()
ap.add_argument('-ef', '--edgeFile', help='csv edge file', type=open, required=True)
ap.add_argument('-gt', '--graphType', help='graph type', type=str, choices=['directed', 'undirected'], default='undirected')
ap.add_argument('-l', '--layout', help='graph layout', type=str, choices=['sfdp', 'sfdp-w', 'fr', 'fr-w', 'arf', 'arf-w', 'radial_tree', 'random', 'none'], default='sfdp-w')
ap.add_argument('-o', '--out', help='output file name', default='result')
args = ap.parse_args()

## Set Graph Data
if(args.graphType == 'undirected'):
    g = gt.Graph(directed=False)
else:
    g = gt.Graph(directed=True)

g.edge_properties['penwidth'] = g.new_edge_property('int')

# load edges
e_penwidth = g.edge_properties['penwidth']

for line in args.edgeFile:
    items = map(int, line.split(','))
    e = g.add_edge(items[0], items[1])
    e_penwidth[e] = items[2];

# if undirected, remove duplicated edges
if (g.is_directed() == False):
    gt.remove_parallel_edges(g)

## Draw Graph
# set layout https://graph-tool.skewed.de/static/doc/draw.html
if (args.layout == 'sfdp'):
    pos = gt.sfdp_layout(g)
elif (args.layout == 'sfdp-w'):
    pos = gt.sfdp_layout(g, eweight=g.edge_properties['penwidth'])
elif (args.layout == 'fr'):
    pos = gt.fruchterman_reingold_layout(g)
elif (args.layout == 'fr-w'):
    pos = gt.fruchterman_reingold_layout(g, weight=g.edge_properties['penwidth'])
elif (args.layout == 'arf'):
    # The arf layout algorithm is numerically unstable
    # It sometime causes an error.
    # See: https://git.skewed.de/count0/graph-tool/issues/239
    pos = gt.arf_layout(g)
elif (args.layout == 'arf-w'):
    pos = gt.arf_layout(g, weight=g.edge_properties['penwidth'])
elif (args.layout == 'radial_tree'):
    # Here we fix vertex 0 as a root
    pos = gt.radial_tree_layout(g, 68)
elif (args.layout == 'random'):
    pos = gt.random_layout(g)

f = open(args.out + ".gml", 'w')
f.write('graph\n')
f.write('[\n')
f.write('\thierarchic\t0\n')
f.write('\tlabel\t\"\"\n')
f.write('\tdirected\t0\n')
for v in g.vertices():
    f.write('\tnode\n')
    f.write('\t[\n')
    f.write('\t\tid\t' + str(v) +'\n')
    f.write('\t\tlabel\t' + '\"\"' +'\n')
    f.write('\t\tgraphics\n')
    f.write('\t\t[\n')
    f.write('\t\t\tx\t' + str(pos[v][0]) + '\n')
    f.write('\t\t\ty\t' + str(pos[v][1]) + '\n')
    f.write('\t\t\tw\t' + str(10.0) + '\n')
    f.write('\t\t\th\t' + str(10.0) + '\n')
    f.write('\t\t\ttype\t' + '\"rectangle\"' + '\n')
    f.write('\t\t\tfill\t' + '\"#CCFFCC\"' + '\n')
    f.write('\t\t\toutline\t' + '\"#000000\"' + '\n')
    f.write('\t\t]\n')
    f.write('\t\tLabelGraphics\n')
    f.write('\t\t[')
    f.write('\t\t]')
    f.write('\t]\n')
for e in g.edges():
    f.write('\tedge\n')
    f.write('\t[\n')
    f.write('\t\tsource\t' + str(e.source()) +'\n')
    f.write('\t\ttarget\t' + str(e.target()) +'\n')
    f.write('\t\tgraphics\n')
    f.write('\t\t[\n')
    f.write('\t\t\tfill\t' + '\"#666666\"' + '\n')
    f.write('\t\t]\n')
    f.write('\t]\n')
f.write(']\n')
f.close()
