###
### Dependency: graph-tool (https://graph-tool.skewed.de)
###   see https://graph-tool.skewed.de/download to install
###
### Usage:
###   python draw_graph.py -ef csvEdgeFile -ef csvEdgeFile -vf csvVertexFile [-o outfilename] [-fm outformat] [-t {directed, undirected}] [-l {sfdp, fr, arf}] [-i {True, False}]
###   (e.g. python draw_graph.py -ef edge.csv)
###         python draw_graph.py -ef edge.csv -vf vertices.csv -o outabc -fm png -t directed -l fr -i False)
###
### FileFormat:
###   edgeFile: each row must includes "source(int),target(int),penwidth(int)"
###

import graph_tool.all as gt
#import pylab as pl
import math
import argparse

import color

## Parse Command Line Args
ap = argparse.ArgumentParser()
ap.add_argument('-ef',
                '--edgeFile',
                help='csv edge file',
                type=open,
                required=True)
ap.add_argument('-vf', '--vertexFile', help='csv vertex file', type=open)
ap.add_argument('-aef',
                '--aggregatedEdgeFile',
                help='csv aggregated edge file',
                type=open)
ap.add_argument('-avf',
                '--aggregatedVertexFile',
                help='csv aggregated vertex file',
                type=open)
ap.add_argument('-p',
                '--pin',
                help='use pin or not for graph layout',
                type=str,
                choices=['true', 'false'],
                default='false')
ap.add_argument('-cf',
                '--childVertexIndicesFile',
                help='csv child vertex indices  file',
                type=open)
ap.add_argument('-o', '--out', help='output file dir', default='')
ap.add_argument('-so',
                '--statOut',
                help='output statistical plot data or not',
                type=str,
                choices=['true', 'false'],
                default='false')
ap.add_argument('-fm', '--format', help='output image format', default='png')
ap.add_argument('-gt',
                '--graphType',
                help='graph type',
                type=str,
                choices=['directed', 'undirected'],
                default='undirected')
ap.add_argument('-l',
                '--layout',
                help='graph layout',
                type=str,
                choices=[
                    'sfdp', 'sfdpw', 'fr', 'frw', 'arf', 'arfw', 'radial_tree',
                    'random', 'none'
                ],
                default='sfdpw')
ap.add_argument('-i',
                '--interact',
                help='graph interaction',
                type=str,
                choices=['true', 'false'],
                default='true')
args = ap.parse_args()

## Set Graph Data
if (args.graphType == 'undirected'):
    g = gt.Graph(directed=False)
    ag = gt.Graph(directed=False)
else:
    g = gt.Graph(directed=True)
    ag = gt.Graph(directed=True)

# add vertex label property
g.vertex_properties['size'] = g.new_vertex_property('double')
g.vertex_properties['label'] = g.new_vertex_property('string')
g.vertex_properties['font_size'] = g.new_vertex_property('double')
g.vertex_properties['pos'] = g.new_vertex_property('vector<float>')
g.vertex_properties['pin'] = g.new_vertex_property('bool')
g.vertex_properties['groups'] = g.new_vertex_property('int')

ag.vertex_properties['size'] = ag.new_vertex_property('double')
ag.vertex_properties['label'] = ag.new_vertex_property('string')
ag.vertex_properties['font_size'] = ag.new_vertex_property('double')

# add edge distance and weight property
g.edge_properties['penwidth'] = g.new_edge_property('double')
g.edge_properties['color'] = g.new_edge_property('vector<float>')
ag.edge_properties['penwidth'] = ag.new_edge_property('double')
ag.edge_properties['color'] = ag.new_edge_property('vector<float>')

# load vertices
if (args.vertexFile):
    for line in args.vertexFile:
        v = g.add_vertex()
    print('finished loading vertices')

# load aggregated vertices
if (args.aggregatedVertexFile):
    for line in args.aggregatedVertexFile:
        av = ag.add_vertex()
    print('finished loading aggregated vertices')

# load childs
childrens = []
if (args.childVertexIndicesFile):
    for line in args.childVertexIndicesFile:
        elems = line.replace('\n', '').replace('\r', '').split(',')
        if (len(elems) > 0 and elems[0] != ''):
            items = map(int, elems)
            childrens.append(items)
        else:
            childrens.append([])

# load edges
e_penwidth = g.edge_properties['penwidth']
e_color = g.edge_properties['color']
v_size = g.vertex_properties['size']
for line in args.edgeFile:
    #items = map(int, line.split(','))
    items = line.replace('\n', '').replace('\r', '').split(',')
    source = int(items[0])
    target = int(items[1])
    weight = float(items[3])
    #weight = float(items[2])

    e = g.add_edge(source, target)
    e_penwidth[e] = weight

    # TODO: this code is temporary
    #if(items[2] > 0 and items[2] < 4):
    #    e_color[e] = color.ratioToHeatMapColor(float(items[2]) / 20.0);

    # here also calculate total penwidth for vertex size
    v = g.vertex(source)  # target vertex
    v_size[v] += weight

# load aggregated edges
ae_penwidth = ag.edge_properties['penwidth']
ae_color = ag.edge_properties['color']
av_size = ag.vertex_properties['size']
for line in args.aggregatedEdgeFile:
    #items = map(int, line.split(','))
    items = line.replace('\n', '').replace('\r', '').split(',')
    source = int(items[0])
    target = int(items[1])
    weight = float(items[3])
    #weight = float(items[2])

    ae = ag.add_edge(source, target)
    ae_penwidth[ae] = weight

    # TODO: this code is temporary
    #if(items[2] > 0 and items[2] < 4):
    #    e_color[e] = color.ratioToHeatMapColor(float(items[2]) / 20.0);

    # here also calculate total penwidth for vertex size
    av = ag.vertex(source)  # target vertex
    av_size[v] += weight

# if undirected, remove duplicated edges
if (g.is_directed() == False):
    gt.remove_parallel_edges(g)

if (ag.is_directed() == False):
    gt.remove_parallel_edges(ag)

print('finished loading edges')

## Basic Stats
# number of vertices and edges
numVertices = g.num_vertices()
numEdges = g.num_edges()
anumVertices = ag.num_vertices()
anumEdges = ag.num_edges()

# degree stats
aveDegTotal = gt.vertex_average(g, 'total')
histDegTotal = gt.vertex_hist(g, 'total')
maxDegTotal = histDegTotal[1][-1]
aaveDegTotal = gt.vertex_average(ag, 'total')
ahistDegTotal = gt.vertex_hist(ag, 'total')
amaxDegTotal = ahistDegTotal[1][-1]
# distance stats
histDist = gt.distance_histogram(g)
graphDiameter, ends = gt.pseudo_diameter(g)
ahistDist = gt.distance_histogram(ag)
agraphDiameter, ends = gt.pseudo_diameter(ag)

# degree for directed graph
if (g.is_directed() == True):
    aveDegIn = gt.vertex_average(g, 'in')
    aveDegOut = gt.vertex_average(g, 'out')
    histDegIn = gt.vertex_hist(g, 'in')
    histDegOut = gt.vertex_hist(g, 'out')
if (ag.is_directed() == True):
    aaveDegIn = gt.vertex_average(ag, 'in')
    aaveDegOut = gt.vertex_average(ag, 'out')
    ahistDegIn = gt.vertex_hist(ag, 'in')
    ahistDegOut = gt.vertex_hist(ag, 'out')

# print basic stats
print('==== non aggregated graph ====')
print('# Graph Information')
print('number of vertices: ' + str(numVertices))
print('number of edges: ' + str(numEdges))

if (g.is_directed() == True):
    print('in-degree(ave, std): ' + str(aveDegIn))
    print('out-degree(ave std): ' + str(aveDegOut))
else:
    print('degree(ave, std): ' + str(aveDegTotal))

print('pseudo diameter: ' + str(graphDiameter))
print('')

print('==== aggregated graph ====')
print('# Graph Information')
print('number of vertices: ' + str(anumVertices))
print('number of edges: ' + str(anumEdges))

if (ag.is_directed() == True):
    print('in-degree(ave, std): ' + str(aaveDegIn))
    print('out-degree(ave std): ' + str(aaveDegOut))
else:
    print('degree(ave, std): ' + str(aaveDegTotal))

print('pseudo diameter: ' + str(agraphDiameter))
print('')

# if (args.statOut == 'true'):
#     # plot degree distribution
#     # :-1 for excluding dtype=uint64 ??? not clear
#     if (g.is_directed() == True):
#         pl.plot(histDegIn[1][:-1], histDegIn[0] / numVertices, 'o')
#         pl.plot(histDegOut[1][:-1], histDegOut[0] / numVertices, 'o')
#         pl.legend(('in', 'out'), numpoints=1)
#     else :
#         pl.plot(histDegTotal[1][:-1], histDegTotal[0] / numVertices, 'o')
#
#     pl.gca().set_yscale('log')
#     pl.gca().set_xscale('log')
#     pl.xlabel('$k$')
#     pl.ylabel('$p(k)$')
#     pl.title('Degree Distribution')
#     pl.savefig(args.out + 'deg.' + args.format)
#     pl.clf()
#
#     # plot distance distribution
#     pl.plot(histDist[1][:-1], histDist[0], 'o')
#
#     pl.gca().set_yscale('log')
#     pl.xlabel('$distance$')
#     pl.ylabel('$counts$')
#     pl.title('Distance Distribution')
#     pl.savefig(args.out + 'dist.' + args.format)
#     pl.clf()
#
#     if (ag.is_directed() == True):
#         pl.plot(ahistDegIn[1][:-1], ahistDegIn[0] / anumVertices, 'o')
#         pl.plot(ahistDegOut[1][:-1], ahistDegOut[0] / anumVertices, 'o')
#         pl.legend(('in', 'out'), numpoints=1)
#     else :
#         pl.plot(ahistDegTotal[1][:-1], ahistDegTotal[0] / anumVertices, 'o')
#
#     pl.gca().set_yscale('log')
#     pl.gca().set_xscale('log')
#     pl.xlabel('$k$')
#     pl.ylabel('$p(k)$')
#     pl.title('Degree Distribution')
#     pl.savefig(args.out + 'aggregated_deg.' + args.format)
#     pl.clf()
#
# # plot distance distribution
# pl.plot(ahistDist[1][:-1], ahistDist[0], 'o')
#
# pl.gca().set_yscale('log')
# pl.xlabel('$distance$')
# pl.ylabel('$counts$')
# pl.title('Distance Distribution')
# pl.savefig(args.out + 'aggregated_dist.' + args.format)
# pl.clf()

## Draw Graph
#pos = gt.random_layout(g)
# set aggregated layout https://graph-tool.skewed.de/static/doc/draw.html
if (args.layout == 'sfdp'):
    apos = gt.sfdp_layout(ag)
elif (args.layout == 'sfdpw'):
    apos = gt.sfdp_layout(ag, eweight=ag.edge_properties['penwidth'])
elif (args.layout == 'fr'):
    apos = gt.fruchterman_reingold_layout(ag)
elif (args.layout == 'frw'):
    apos = gt.fruchterman_reingold_layout(
        ag, weight=ag.edge_properties['penwidth'])
elif (args.layout == 'arf'):
    # The arf layout algorithm is numerically unstable
    # It sometime causes an error.
    # See: https://git.skewed.de/count0/graph-tool/issues/239
    apos = gt.arf_layout(ag)
    #pos = gt.arf_layout(g, max_iter=100, dt=1e-4)
elif (args.layout == 'arfw'):
    apos = gt.arf_layout(ag, weight=ag.edge_properties['penwidth'])
    #pos = gt.arf_layout(g, max_iter=100, dt=1e-4, weight=g.edge_properties['penwidth'])
elif (args.layout == 'radial_tree'):
    # Here we fix vertex 0 as a root
    apos = gt.radial_tree_layout(ag, 68)
elif (args.layout == 'random'):
    apos = gt.random_layout(ag)

# set layout based on aggregated layout
##################
v_pos = g.vertex_properties['pos']
v_pin = g.vertex_properties['pin']
v_groups = g.vertex_properties['groups']
if (args.layout == 'sfdpw'):
    for av in ag.vertices():
        avIndex = ag.vertex_index[av]
        children = childrens[avIndex]
        p = apos[avIndex]
        pin = True
        for vIndex in children:
            v = g.vertex(vIndex)
            v_pos[v] = p
            v_pin[v] = pin
            v_groups[v] = avIndex
            pin = False
    if (pin == 'true'):
        pos = gt.sfdp_layout(g,
                             eweight=g.edge_properties['penwidth'],
                             pin=g.vertex_properties['pin'],
                             groups=g.vertex_properties['groups'],
                             pos=g.vertex_properties['pos'])
    else:
        pos = gt.sfdp_layout(g, eweight=g.edge_properties['penwidth'])
        #pos = gt.sfdp_layout(g, eweight=g.edge_properties['penwidth'], C=1.2, p=4, gamma=1.5) ## good for multiroute
        #pos = gt.sfdp_layout(g, eweight=g.edge_properties['penwidth'], C=0.4, gamma=1.5) ## good for miniMD large
        #pos = gt.sfdp_layout(g, eweight=g.edge_properties['penwidth'], C=1.2, p=4, gamma=1.5)

# get max and min for normalization
minXPos = 10000000.0
maxXPos = -10000000.0
minYPos = 10000000.0
maxYPos = -10000000.0
for v in g.vertices():
    minXPos = min(minXPos, pos[v][0])
    minYPos = min(minYPos, pos[v][1])
    maxXPos = max(maxXPos, pos[v][0])
    maxYPos = max(maxYPos, pos[v][1])

# pos normalization between -1.0 and 1.0
rangeX = maxXPos - minXPos
rangeY = maxYPos - minYPos
if (rangeX > rangeY):
    nXMin = -1.0  # normalized min
    nXMax = 1.0  # normalized max
    nYMin = nXMin * rangeY / rangeX
    nYMax = nXMax * rangeY / rangeX
else:
    nYMin = -1.0
    nYMax = 1.0
    nXMin = nYMin * rangeX / rangeY
    nXMax = nYMax * rangeX / rangeY

aminXPos = 10000000.0
amaxXPos = -10000000.0
aminYPos = 10000000.0
amaxYPos = -10000000.0
for av in ag.vertices():
    aminXPos = min(aminXPos, apos[av][0])
    aminYPos = min(aminYPos, apos[av][1])
    amaxXPos = max(amaxXPos, apos[av][0])
    amaxYPos = max(amaxYPos, apos[av][1])

# pos normalization between -1.0 and 1.0
arangeX = amaxXPos - aminXPos
arangeY = amaxYPos - aminYPos
if (arangeX > arangeY):
    anXMin = -1.0  # normalized min
    anXMax = 1.0  # normalized max
    anYMin = anXMin * arangeY / arangeX
    anYMax = anXMax * arangeY / arangeX
else:
    anYMin = -1.0
    anYMax = 1.0
    anXMin = anYMin * arangeX / arangeY
    anXMax = anYMax * arangeX / arangeY

f = open(args.out + 'positions_' + args.layout + '.csv', "w")
for v in g.vertices():
    x = (pos[v][0] * nXMin - pos[v][0] * nXMax + minXPos * nXMax -
         maxXPos * nXMin) / (maxXPos - minXPos)
    y = (pos[v][1] * nYMin - pos[v][1] * nYMax + minYPos * nYMax -
         maxYPos * nYMin) / (maxYPos - minYPos)
    f.write(str(x) + ',' + str(y) + '\n')
    #f.write(str(pos[v][0]) + "," + str(pos[v][1]) + "\n")
f.close()

# af = open(args.out + 'aggregated_positions_' + args.layout + '.csv', "w")
# for av in ag.vertices():
#     ax = ( apos[av][0] * anXMin - apos[av][0] * anXMax + aminXPos * anXMax - amaxXPos * anXMin ) / (amaxXPos - aminXPos)
#     ay = ( apos[av][1] * anYMin - apos[av][1] * anYMax + aminYPos * anYMax - amaxYPos * anYMin ) / (amaxYPos - aminYPos)
#     af.write(str(ax) + ',' + str(ay) + '\n')
# af.close()
af = open(args.out + 'aggregated_positions_' + args.layout + '.csv', "w")
for av in ag.vertices():
    ax = (apos[av][0] * nXMin - apos[av][0] * nXMax + aminXPos * nXMax -
          amaxXPos * nXMin) / (amaxXPos - aminXPos)
    ay = (apos[av][1] * nYMin - apos[av][1] * nYMax + aminYPos * nYMax -
          amaxYPos * nYMin) / (amaxYPos - aminYPos)
    af.write(str(ax) + ',' + str(ay) + '\n')
af.close()

if (args.layout != 'none'):
    #for v in g.vertices():
    #if (g.vertex_properties['size'][g.vertex(v)] < aveDegTotal[0] * 1.5):
    #    g.vertex_properties['label'][g.vertex(v)] = ''
    #else:
    #    g.vertex_properties['label'][g.vertex(v)] = g.vertex_index[v]
    for v in g.vertices():
        g.vertex_properties['font_size'][g.vertex(
            v)] = g.vertex_properties['size'][g.vertex(v)]
        #g.vertex_properties['font_size'][g.vertex(v)] = 3.0 * math.sqrt(g.vertex_properties['size'][g.vertex(v)])

    if (args.interact == 'true'):
        graph_output = None
    else:
        graph_output = args.out + 'graph_' + args.layout + '.' + args.format

    gt.graph_draw(
        g,
        pos=g.vertex_properties['pos'],
        vertex_fill_color=[44 / 255.0, 127 / 255.0, 184 / 255.0, 0.8],
        #vertex_size=g.vertex_properties['size'],
        #vertex_text=g.vertex_index,
        #vertex_text_position=0,
        #vertex_font_size=g.vertex_properties['font_size'],
        #vertex_text_offset=[-1, 0],
        edge_pen_width=g.edge_properties['penwidth'],
        edge_marker_size=2,
        edge_color=g.edge_properties['color'],
        output_size=(1000, 1000),
        output=graph_output)

    ####
    for av in ag.vertices():
        ag.vertex_properties['font_size'][ag.vertex(
            av)] = ag.vertex_properties['size'][ag.vertex(av)]
        #g.vertex_properties['font_size'][g.vertex(v)] = 3.0 * math.sqrt(g.vertex_properties['size'][g.vertex(v)])

    if (args.interact == 'true'):
        graph_output = None
    else:
        graph_output = args.out + 'aggregated_graph_' + args.layout + '.' + args.format

    gt.graph_draw(
        ag,
        pos=apos,
        vertex_fill_color=[44 / 255.0, 127 / 255.0, 184 / 255.0, 0.8],
        #vertex_size=g.vertex_properties['size'],
        #vertex_text=g.vertex_index,
        #vertex_text_position=0,
        #vertex_font_size=g.vertex_properties['font_size'],
        #vertex_text_offset=[-1, 0],
        edge_pen_width=ag.edge_properties['penwidth'],
        edge_marker_size=2,
        edge_color=ag.edge_properties['color'],
        output_size=(1000, 1000),
        output=graph_output)
