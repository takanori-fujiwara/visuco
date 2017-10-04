#include "RouteGuide.h"

RouteGuide::RouteGuide()
{
}

RouteGuide::RouteGuide(Graph* graph, AdjMatrix* adjMat)
{
    this->graph = graph;
    this->adjMat = adjMat;
    graphForUpdate = *graph;
}

void RouteGuide::initilize(Graph* graph, AdjMatrix *adjMat)
{
    this->graph = graph;
    this->adjMat = adjMat;
    graphForUpdate = *graph;
}

QVector<Route> RouteGuide::findAltRoutes(Route route, QString type, int lengthTolerance, float loadTolerance)
{
    QVector< QVector<int> > mat = adjMat->getAdjMat();
    int n = graphForUpdate.getNumVertices();
    QVector<int> visitedNodes(n, 0);
    QList< QVector<int> > queue;
    QVector<Route> results;

    int src = route.getSrc();
    int dest = route.getDest();
    int length = route.getLength();
    int maxEdgeLoad = route.getMaxEdgeLoad();
    float maxEdgeByte = route.getMaxEdgeByte();

    QVector<int> srcs;
    if (type == "src-dest-fixed" || type == "src-fixed") {
        srcs.push_back(src);
    } else if (type == "non-fixed") {
        for (int i = 0; i < n; ++i) {
            srcs.push_back(i); // if src is not fixed, all nodes are handled as a src node
        }
    }

    foreach(int s, srcs) {
        ///
        /// Here is main algorithm extended from BFS algorithm
        ///

        /// 1. initialization
        queue.push_back(QVector<int>({s}));

        /// 2. loop to obtain every routes that has length less than original route length + tolerance
        while (queue.size() != 0) {
            QVector<int> q = queue.takeFirst();

            // keep graph traversing until length less than length + lengthTolerance
            if (q.size() <= length + lengthTolerance) {
                int currentVertexIndex = q.last();

                // vertex index to column num
                int currentColNum = adjMat->coordToColNum(graphForUpdate.getVertexAt(currentVertexIndex).getCoord());

                // TODO: here should be changed to adjLst for reducing calc cost
                for (int i = 0; i < mat.at(currentColNum).size(); ++i) {
                    if (mat.at(currentColNum).at(i) == 1) {

                        // column num -> coord -> vertex index
                        int vIndex = graphForUpdate.getVertexIndexWithCoord(adjMat->colNumToCoord(i));
                        QVector<int> nextQ = q;
                        nextQ.push_back(vIndex);

                        if (visitedNodes[vIndex] == 0) {
                            visitedNodes[vIndex] = 1;
                            queue.push_back(nextQ);
                        }

                        // after reaching the destination, judge the route should be stored or not based on the heaviest load
                        if (vIndex == dest || type != "src-dest-fixed") { // if not src-dest-fixed, all nodes are handled as a destination node
                            int altMaxEdgeLoad = 0;
                            float altMaxEdgeByte = 0.0;
                            for (int j = 0; j < nextQ.size() - 1; ++j) {
                                Edge e = graphForUpdate.getEdgeWithSourceTarget(nextQ.at(j), nextQ.at(j+1));
                                int altLoad = e.getLoad();
                                float altByte = e.getByte();

                                if (!(route.includesEdge(e, true))) {
                                    altLoad += route.getLoad(); // if the route use this alt route, every load of edge which is not included in original route will increase by load of route
                                    altByte += route.getByte();
                                }

                                if (altByte > altMaxEdgeByte) altMaxEdgeByte = altByte;
                                if (altLoad > altMaxEdgeLoad) altMaxEdgeLoad = altLoad;
                                if (altMaxEdgeByte > altMaxEdgeByte + loadTolerance) break;
                            }

                            if (altMaxEdgeByte < maxEdgeByte + loadTolerance) {
                                results.push_back(Route(route.getIndex(), nextQ.first(), nextQ.last(), nextQ, route.getByte(), route.getLoad(), altMaxEdgeLoad, altMaxEdgeByte)); // allocate the same index with original one
                            }
                        }
                    }
                }
            }
        }
    }

    // when alt route cannot be found, return original route
    //if (results.size() == 0) results.push_back(route);

    return results;
}

QVector<Route> RouteGuide::findAltRoutesForRemapping(Route route, int altSrc, int altDest, int thresLength, int thresMaxLoad, QVector<QVector<int> > &edgeLoadIncInfoForRemapping)
{
    QVector< QVector<int> > mat = adjMat->getAdjMat();
    int n = graphForUpdate.getNumVertices();
    QVector<int> visitedNodes(n, 0);
    QList< QVector<int> > queue;
    QVector<Route> results;

    int src = altSrc;
    int dest = altDest;

    QVector<int> srcs;
    srcs.push_back(src);

    foreach(int s, srcs) {
        ///
        /// Here is main algorithm extended from BFS algorithm
        ///

        /// 1. initialization
        queue.push_back(QVector<int>({s}));

        /// 2. loop to obtain every routes that has length less than original route length + tolerance
        while (queue.size() != 0) {
            QVector<int> q = queue.takeFirst();

            // keep graph traversing until length less than length + lengthTolerance
            if (q.size() <= thresLength) {
                int currentVertexIndex = q.last();

                // vertex index to column num
                int currentColNum = adjMat->coordToColNum(graphForUpdate.getVertexAt(currentVertexIndex).getCoord());

                // TODO: here should be changed to adjLst for reducing calc cost
                for (int i = 0; i < mat.at(currentColNum).size(); ++i) {
                    if (mat.at(currentColNum).at(i) == 1) {

                        // column num -> coord -> vertex index
                        int vIndex = graphForUpdate.getVertexIndexWithCoord(adjMat->colNumToCoord(i));
                        QVector<int> nextQ = q;
                        nextQ.push_back(vIndex);

                        if (visitedNodes[vIndex] == 0) {
                            visitedNodes[vIndex] = 1;
                            queue.push_back(nextQ);
                        }

                        // after reaching the destination, judge the route should be stored or not based on the heaviest load
                        if (vIndex == dest) { // if not src-dest-fixed, all nodes are handled as a destination node
                            int altMaxEdgeLoad = 0;
                            for (int j = 0; j < nextQ.size() - 1; ++j) {
                                int eSource = nextQ.at(j);
                                int eTarget = nextQ.at(j+1);
                                Edge e = graphForUpdate.getEdgeWithSourceTarget(eSource, eTarget);
                                int altLoad = e.getLoad() + edgeLoadIncInfoForRemapping.at(eSource).at(eTarget);

                                if (!(route.includesEdge(e, true))) {
                                    altLoad += route.getLoad(); // if the route use this alt route, every load of edge which is not included in original route will increase by load of route
                                }
                                if (altLoad > altMaxEdgeLoad) altMaxEdgeLoad = altLoad;
                                if (altMaxEdgeLoad > thresMaxLoad) break;
                            }

                            if (altMaxEdgeLoad <= thresMaxLoad) {
                                results.push_back(Route(route.getIndex(), nextQ.first(), nextQ.last(), nextQ, route.getLoad(), altMaxEdgeLoad)); // allocate the same index with original one
                            }
                        }
                    }
                }
            }
        }
    }

    // when alt route cannot be found, return original route
    //if (results.size() == 0) results.push_back(route);

    return results;
}

bool RouteGuide::lengthLessThan(Route route1, Route route2){ return route1.getLength() < route2.getLength(); }
bool RouteGuide::maxEdgeLoadLessThan(Route route1, Route route2){ return route1.getMaxEdgeLoad() < route2.getMaxEdgeLoad(); }
bool RouteGuide::lengthLessThanThenMaxEdgeLoadLessThan(Route route1, Route route2)
{
    if (route1.getLength() < route2.getLength()) return true;
    else if (route1.getLength() > route2.getLength()) return false;
    else if (route1.getMaxEdgeLoad() < route2.getMaxEdgeLoad()) return true;
    else return false;
}

bool RouteGuide::maxEdgeLoadLessThanThenLengthLessThan(Route route1, Route route2)
{
    if (route1.getMaxEdgeLoad() < route2.getMaxEdgeLoad()) return true;
    else if (route1.getMaxEdgeLoad() > route2.getMaxEdgeLoad()) return false;
    else if (route1.getLength() < route2.getLength()) return true;
    else return false;
}

void RouteGuide::reorderAltRoutesByLength(QVector<Route> &routes)
{
    qSort(routes.begin(), routes.end(), lengthLessThan);
}

void RouteGuide::reorderAltRoutesByMaxEdgeLoad(QVector<Route> &routes)
{
    qSort(routes.begin(), routes.end(), maxEdgeLoadLessThan);
}

void RouteGuide::reorderAltRoutesByLengthThenMaxEdgeLoad(QVector<Route> &routes)
{
    qSort(routes.begin(), routes.end(), lengthLessThanThenMaxEdgeLoadLessThan);
}

void RouteGuide::reorderAltRoutesByMaxEdgeLoadThenLength(QVector<Route> &routes)
{
    qSort(routes.begin(), routes.end(), maxEdgeLoadLessThanThenLengthLessThan);
}

void RouteGuide::filterRoutesByPassedVertex(QVector<Route> &routes, int vertexIndex)
{
    // Remove all routes that does not pass vertex Index
    QMutableVectorIterator<Route> it(routes);
    while (it.hasNext()) {
        if (!it.next().getArcs().contains(vertexIndex)) it.remove();
    }
}

void RouteGuide::filterRoutesBySrcOrDest(QVector<Route> &routes, int vertexIndex)
{
    // Remove all routes that does not have vertex Index as src or dest
    QMutableVectorIterator<Route> it(routes);
    while (it.hasNext()) {
        Route rt = it.next();
        if (!(rt.getSrc() == vertexIndex || rt.getDest() == vertexIndex)) it.remove();
    }
}

void RouteGuide::filterRoutesBySrc(QVector<Route> &routes, int srcVertexIndex)
{
    // Remove all routes that does not have vertex Index as src
    QMutableVectorIterator<Route> it(routes);
    while (it.hasNext()) {
        Route rt = it.next();
        if (!(rt.getSrc() == srcVertexIndex)) it.remove();
    }
}

void RouteGuide::filterRoutesByDest(QVector<Route> &routes, int destVertexIndex)
{
    // Remove all routes that does not have vertex Index as dest
    QMutableVectorIterator<Route> it(routes);
    while (it.hasNext()) {
        Route rt = it.next();
        if (!(rt.getDest() == destVertexIndex)) it.remove();
    }
}

bool RouteGuide::isConnectedAllVerticesRelatedWithReplacingInOneRoute(Route &route, int originalVertexIndex, int altVertexIndex, QVector< QVector<int> > &adjacencyMatrix)
{
    QVector<int> arcs = route.getArcs();

    // store pre and next nodes with original dest and alternative dest in each route
    QVector<int> preAndNextIndicesWithOriginalDest;
    int originalDestInd = arcs.indexOf(originalVertexIndex);
    if (originalDestInd > 0) preAndNextIndicesWithOriginalDest.push_back(arcs.at(originalDestInd - 1));
    if (originalDestInd < arcs.size() - 1) preAndNextIndicesWithOriginalDest.push_back(arcs.at(originalDestInd + 1));

    QVector<int> preAndNextIndicesWithAltDest;
    int altDestInd = arcs.indexOf(altVertexIndex);
    if (altDestInd > 0) preAndNextIndicesWithAltDest.push_back(arcs.at(altDestInd - 1));
    if (altDestInd < arcs.size() - 1) preAndNextIndicesWithAltDest.push_back(arcs.at(altDestInd + 1));

    // check adjacent or not by using adjMat
    foreach(int i, preAndNextIndicesWithOriginalDest) {
       int colNum = adjMat->coordToColNum(graph->getVertexAt(i).getCoord());
       int rowNum = adjMat->coordToColNum(graph->getVertexAt(altVertexIndex).getCoord());
       if (adjacencyMatrix.at(colNum).at(rowNum) != 1) return false;
    }
    foreach(int i, preAndNextIndicesWithAltDest) {
        int colNum = adjMat->coordToColNum(graph->getVertexAt(i).getCoord());
        int rowNum = adjMat->coordToColNum(graph->getVertexAt(originalVertexIndex).getCoord());
        if (adjacencyMatrix.at(colNum).at(rowNum) != 1) return false;
    }

    return true;
}

QVector<Route> RouteGuide::candidateRoutesWhoseVerticesRelatedWithReplacingAreConnected(Route originalRoute, QVector<Route> candidateRoutes)
{
    QVector<Route> result;

    int originalDest = originalRoute.getDest();
    int altDest;

    QVector< QVector<int> > mat = adjMat->getAdjMat();

    QVector<Route> filteredRoutesWithOriginalDest = graph->getRoutes();
    filterRoutesByPassedVertex(filteredRoutesWithOriginalDest, originalDest);

    QVector<Route> filteredRoutesWithCandidateDest = graph->getRoutes();

    foreach(Route candRt, candidateRoutes) {
        altDest = candRt.getDest();
        filterRoutesByPassedVertex(filteredRoutesWithCandidateDest, altDest);

        bool connectedAllVerticesRelatedWithReplacing = true;

        // first check vertices related with original dest
        foreach(Route rt, filteredRoutesWithOriginalDest) {
            if(!isConnectedAllVerticesRelatedWithReplacingInOneRoute(rt, originalDest, altDest, mat)) {
                connectedAllVerticesRelatedWithReplacing = false;
                break;
            }
        }

        // check vertices related with alt dest
        foreach(Route rt, filteredRoutesWithCandidateDest) {
            if(!isConnectedAllVerticesRelatedWithReplacingInOneRoute(rt, originalDest, altDest, mat)) {
                connectedAllVerticesRelatedWithReplacing = false;
                break;
            }
        }

        if (connectedAllVerticesRelatedWithReplacing) {
            result.push_back(candRt);
        }
    }

    return result;
}

Route RouteGuide::suggestRoute(QString priority, Route route, QString type, int lengthTolerance, float loadTolerance)
{
    Route result;

    if (type == "src-fixed") {
        result = suggestRouteSrcFixed(priority, route, lengthTolerance, loadTolerance);
    } else if (type == "src-dest-fixed") {
        chrono::high_resolution_clock::time_point t1 = chrono::high_resolution_clock::now();

        result = suggestRouteSrcDestFixed(priority, route, lengthTolerance, loadTolerance);

        chrono::high_resolution_clock::time_point t2 = chrono::high_resolution_clock::now();
        chrono::duration<double> time_span = chrono::duration_cast<chrono::duration<double> >(t2 - t1);
        qDebug() << "Time: " << time_span.count();
    }

    return result;
}

Route RouteGuide::suggestRouteSrcDestFixed(QString priority, Route route, int lengthTolerance, float loadTolerance)
{
    Route result;
    QVector<Route> candidateRoutes = findAltRoutes(route, "src-dest-fixed", lengthTolerance, loadTolerance);

    if (candidateRoutes.size() == 0) {
        result = route;
    } else {
        if (priority == "lengthFirst") reorderAltRoutesByLengthThenMaxEdgeLoad(candidateRoutes);
        else if (priority == "loadFirst") reorderAltRoutesByMaxEdgeLoadThenLength(candidateRoutes);

        result = candidateRoutes.first();
    }

    return result;
}

Route RouteGuide::suggestRouteSrcFixed(QString priority, Route route, int lengthTolerance, float loadTolerance)
{
    // TODO: adj mat should be changed to non-copy version.
    QVector< QVector<int> > mat = adjMat->getAdjMat();

    int destBefore = route.getDest(); // destination vertex index wanted to be replaced

    // get all routes that have the same source vertex or destination vertex with destBefore
    QSet<int> verticesAdjacentWithOriginalDest;
    int currentColNum = adjMat->coordToColNum(graphForUpdate.getVertexAt(destBefore).getCoord());
    for (int i = 0; i < mat.at(currentColNum).size(); ++i) {
        if (mat.at(currentColNum).at(i) == 1) {
            // column num -> coord -> vertex index
            int vIndex = graphForUpdate.getVertexIndexWithCoord(adjMat->colNumToCoord(i));
            verticesAdjacentWithOriginalDest.insert(vIndex);
        }
    }

    QVector<Route> routesWhichHasOriginalDestAsSrc = graph->getRoutes();
    QVector<Route> routesWhichHasOriginalDestAsDest = graph->getRoutes();
    routesWhichHasOriginalDestAsSrc.remove(route.getIndex()); // remove original route
    routesWhichHasOriginalDestAsDest.remove(route.getIndex()); // remove original route
    filterRoutesBySrc(routesWhichHasOriginalDestAsSrc, destBefore);
    filterRoutesByDest(routesWhichHasOriginalDestAsDest, destBefore);

    reorderAltRoutesByLength(routesWhichHasOriginalDestAsSrc);
    reorderAltRoutesByLength(routesWhichHasOriginalDestAsDest);

    //
    // Main Part 1: get all candidates for alternative routes
    //
    QVector<Route> candidateRoutes = findAltRoutes(route, "src-fixed", lengthTolerance, loadTolerance);

    // reorder to reduce the searching time in below foreach loop
    if (priority == "lengthFirst") reorderAltRoutesByLengthThenMaxEdgeLoad(candidateRoutes);
    else if (priority == "loadFirst") reorderAltRoutesByMaxEdgeLoadThenLength(candidateRoutes);

    //qDebug() << "original route" << route.getArcs();
    //
    // Main Part 2:
    //
    foreach(Route candidate, candidateRoutes) {
        //qDebug() << "candidate route" << candidate.getArcs();
        edgeLoadIncInfoForRemapping = QVector<QVector<int> >(graph->getNumVertices(), QVector<int>(graph->getNumVertices(), 0));
        routesWillBeUpdatedForRemapping.clear();

        int destAfter = candidate.getDest();

        QVector<Route> routesWhichHasAltDestAsSrc = graph->getRoutes();
        QVector<Route> routesWhichHasAltDestAsDest = graph->getRoutes();
        routesWhichHasAltDestAsSrc.remove(route.getIndex()); // remove original route
        routesWhichHasAltDestAsDest.remove(route.getIndex()); // remove original route
        filterRoutesBySrc(routesWhichHasAltDestAsSrc, destAfter);
        filterRoutesByDest(routesWhichHasAltDestAsDest, destAfter);

        reorderAltRoutesByLength(routesWhichHasAltDestAsSrc);
        reorderAltRoutesByLength(routesWhichHasAltDestAsDest);

        bool foundNewPath = true;
        int lengthDiffSum = 0;
        foreach(Route r, routesWhichHasOriginalDestAsSrc) {
            QVector<Route> altRs = findAltRoutesForRemapping(r, destAfter, r.getDest(), route.getLength(), route.getMaxEdgeLoad(), edgeLoadIncInfoForRemapping);
            Route altR;
            if (altRs.size() == 0) {
                //qDebug() << r.getArcs() << graph->getVertexAt(r.getSrc()).getCoord() << graph->getVertexAt(r.getDest()).getCoord();
                foundNewPath = false;
                break;
            } else {
                if (priority == "lengthFirst") reorderAltRoutesByLengthThenMaxEdgeLoad(altRs);
                else if (priority == "loadFirst") reorderAltRoutesByMaxEdgeLoadThenLength(altRs);
                //reorderAltRoutesByMaxEdgeLoadThenLength(altRs);

                altR = altRs.first();
                lengthDiffSum = lengthDiffSum - r.getLength() + altR.getLength();

                routesWillBeUpdatedForRemapping.push_back(altR);

                // update temp load info based on which route is deleted and which route is added.
                for (int i = 0; i < r.getArcs().size() - 1; ++i) {
                    int eSource = r.getArcs().at(i);
                    int eTarget = r.getArcs().at(i + 1);
                    edgeLoadIncInfoForRemapping[eSource][eTarget] -= r.getLoad();
                }
                for (int i = 0; i < altR.getArcs().size() - 1; ++i) {
                    int eSource = altR.getArcs().at(i);
                    int eTarget = altR.getArcs().at(i + 1);
                    edgeLoadIncInfoForRemapping[eSource][eTarget] += r.getLoad();
                }
            }
        }

        if (foundNewPath) {
            foreach(Route r, routesWhichHasOriginalDestAsDest) {
                // TODO: tmp code here
                QVector<Route> altRs = findAltRoutesForRemapping(r, r.getSrc(), destAfter, route.getLength(), route.getMaxEdgeLoad(), edgeLoadIncInfoForRemapping);
                Route altR;

                if (altRs.size() == 0) {
                    foundNewPath = false;
                    break;
                } else {
                    if (priority == "lengthFirst") reorderAltRoutesByLengthThenMaxEdgeLoad(altRs);
                    else if (priority == "loadFirst") reorderAltRoutesByMaxEdgeLoadThenLength(altRs);

                    altR = altRs.first();
                    lengthDiffSum = lengthDiffSum - r.getLength() + altR.getLength();
                    routesWillBeUpdatedForRemapping.push_back(altR);

                    // update temp load info based on which route is deleted and which route is added.
                    for (int i = 0; i < r.getArcs().size() - 1; ++i) {
                        int eSource = r.getArcs().at(i);
                        int eTarget = r.getArcs().at(i + 1);
                        edgeLoadIncInfoForRemapping[eSource][eTarget] -= r.getLoad();
                    }
                    for (int i = 0; i < altR.getArcs().size() - 1; ++i) {
                        int eSource = altR.getArcs().at(i);
                        int eTarget = altR.getArcs().at(i + 1);
                        edgeLoadIncInfoForRemapping[eSource][eTarget] += r.getLoad();
                    }
                }
            }
        }

        if (foundNewPath) {
            foreach(Route r, routesWhichHasAltDestAsSrc) {
                // TODO: tmp code here
                QVector<Route> altRs = findAltRoutesForRemapping(r, destBefore, r.getDest(), route.getLength(), route.getMaxEdgeLoad(), edgeLoadIncInfoForRemapping);
                Route altR;

                if (altRs.size() == 0) {
                    foundNewPath = false;
                    break;
                } else {
                    if (priority == "lengthFirst") reorderAltRoutesByLengthThenMaxEdgeLoad(altRs);
                    else if (priority == "loadFirst") reorderAltRoutesByMaxEdgeLoadThenLength(altRs);

                    altR = altRs.first();
                    lengthDiffSum = lengthDiffSum - r.getLength() + altR.getLength();

                    routesWillBeUpdatedForRemapping.push_back(altR);

                    // update temp load info based on which route is deleted and which route is added.
                    for (int i = 0; i < r.getArcs().size() - 1; ++i) {
                        int eSource = r.getArcs().at(i);
                        int eTarget = r.getArcs().at(i + 1);
                        edgeLoadIncInfoForRemapping[eSource][eTarget] -= r.getLoad();
                    }
                    for (int i = 0; i < altR.getArcs().size() - 1; ++i) {
                        int eSource = altR.getArcs().at(i);
                        int eTarget = altR.getArcs().at(i + 1);
                        edgeLoadIncInfoForRemapping[eSource][eTarget] += r.getLoad();
                    }
                }
            }
        }

        if (foundNewPath) {
            foreach(Route r, routesWhichHasAltDestAsDest) {
                r.setDest(destBefore);
                // TODO: tmp code here
                QVector<Route> altRs = findAltRoutesForRemapping(r, r.getSrc(), destBefore, route.getLength(), route.getMaxEdgeLoad(), edgeLoadIncInfoForRemapping);
                Route altR;

                if (altRs.size() == 0) {
                    foundNewPath = false;
                    break;
                } else {
                    if (priority == "lengthFirst") reorderAltRoutesByLengthThenMaxEdgeLoad(altRs);
                    else if (priority == "loadFirst") reorderAltRoutesByMaxEdgeLoadThenLength(altRs);

                    altR = altRs.first();
                    lengthDiffSum = lengthDiffSum - r.getLength() + altR.getLength();

                    routesWillBeUpdatedForRemapping.push_back(altR);

                    // update temp load info based on which route is deleted and which route is added.
                    for (int i = 0; i < r.getArcs().size() - 1; ++i) {
                        int eSource = r.getArcs().at(i);
                        int eTarget = r.getArcs().at(i + 1);
                        edgeLoadIncInfoForRemapping[eSource][eTarget] -= r.getLoad();
                    }
                    for (int i = 0; i < altR.getArcs().size() - 1; ++i) {
                        int eSource = altR.getArcs().at(i);
                        int eTarget = altR.getArcs().at(i + 1);
                        edgeLoadIncInfoForRemapping[eSource][eTarget] += r.getLoad();
                    }
                }
            }
        }

        if (foundNewPath && lengthDiffSum <= 0) {
            return candidate;
        }
    }

    return route;
}

void RouteGuide::reconstructRoutesBasedOnReplacedDests(QVector<Route> &routes, int destBefore, int destAfter)
{
    // TODO: adj mat should be changed to non-copy version.
    QVector< QVector<int> > mat = adjMat->getAdjMat();

    QSet<int> verticesAdjacentWithOriginalDest;
    int colNum = adjMat->coordToColNum(graphForUpdate.getVertexAt(destBefore).getCoord());
    for (int i = 0; i < mat.at(colNum).size(); ++i) {
        if (mat.at(colNum).at(i) == 1) {
            // column num -> coord -> vertex index
            int vIndex = graphForUpdate.getVertexIndexWithCoord(adjMat->colNumToCoord(i));
            verticesAdjacentWithOriginalDest.insert(vIndex);
        }
    }

    QVector<int> verticesAdjacentWithAltDest;
    // vertex index to column num
    colNum = adjMat->coordToColNum(graphForUpdate.getVertexAt(destAfter).getCoord());
    for (int i = 0; i < mat.at(colNum).size(); ++i) {
        if (mat.at(colNum).at(i) == 1) {
            // column num -> coord -> vertex index
            int vIndex = graphForUpdate.getVertexIndexWithCoord(adjMat->colNumToCoord(i));
            verticesAdjacentWithAltDest.push_back(vIndex);
        }
    }

    QVector<Route> filteredRoutesWithOriginalDest = graph->getRoutes();
    filterRoutesBySrcOrDest(filteredRoutesWithOriginalDest, destBefore);

    QVector<Route> filteredRoutesWithAltDest = graph->getRoutes();
    filterRoutesBySrcOrDest(filteredRoutesWithAltDest, destAfter);

    foreach(Route rt, filteredRoutesWithOriginalDest) {
        QVector<int> arcs = rt.getArcs();
        if (rt.getSrc() == destBefore) {
            for (int i = 0; i < arcs.size(); ++i) {
                int index = arcs.size() - 1 - i;
                // search from tail
                if (verticesAdjacentWithAltDest.contains(arcs.at(index))) {
                    if (index - 1 >= 0) {
                        arcs.remove(1, index - 1);
                    }
                }
            }
        } else { // <- rt.getDest() == destBefore
            for (int i = 0; i < arcs.size(); ++i) {
                int index = i;
                // search from head
                if (verticesAdjacentWithAltDest.contains(arcs.at(index))) {
                    if ((arcs.size() - 1) - (index + 1) >= 0) {
                       arcs.remove(index + 1, (arcs.size() - 1) - (index + 1));
                    }
                }
            }
        }
        routes[rt.getIndex()].setArcs(arcs);
    }

    foreach(Route rt, filteredRoutesWithAltDest) {
        QVector<int> arcs = rt.getArcs();
        if (rt.getSrc() == destAfter) {
            for (int i = 0; i < arcs.size(); ++i) {
                int index = arcs.size() - 1 - i;
                // search from tail
                if (verticesAdjacentWithOriginalDest.contains(arcs.at(index))) {
                    if (index - 1 >= 0) {
                        arcs.remove(1, index - 1);
                    }
                }
            }
        } else { // <- rt.getDest() == destBefore
            for (int i = 0; i < arcs.size(); ++i) {
                int index = i;
                // search from head
                if (verticesAdjacentWithOriginalDest.contains(arcs.at(index))) {
                    if ((arcs.size() - 1) - (index + 1) >= 0) {
                        arcs.remove(index + 1, (arcs.size() - 1) - (index + 1));
                    }
                }
            }
        }
        routes[rt.getIndex()].setArcs(arcs);
    }
}

void RouteGuide::replaceVerticesInMiddleOfRoutes(QVector<Route> &routes, int destBefore, int destAfter)
{
    foreach(Route rt, routes) {
        QVector<int> arcs = rt.getArcs();
        for (int i = 1; i < arcs.size() - 1; ++i) {
            if (arcs.at(i) == destBefore) {
                arcs[i] = destAfter;
            } else if (arcs.at(i) == destAfter) {
                arcs[i] = destBefore;
            }
        }

        routes[rt.getIndex()].setArcs(arcs);
    }
}

void RouteGuide::updateGraphForUpdate(Route originalRoute, Route altRoute)
{
    // only update necessary infos for suggestion (not run recalc static info about graph, etc)
    // updating items: vertices, coordToVertexIndex, edges, routes
    int destOriginal = originalRoute.getArcs().last();
    int destAlt = altRoute.getArcs().last();
    QVector<int> originalArcs = originalRoute.getArcs();
    QVector<int> altArcs = altRoute.getArcs();

    // in case destination node replacing is needed
    if (destOriginal != destAlt) {
        Vertex vertexOriginal = graphForUpdate.getVertexAt(destOriginal);
        Vertex vertexAlt = graphForUpdate.getVertexAt(destAlt);

        QVector<int> ranksOriginal = vertexOriginal.getRanks();
        QVector<int> ranksAlt = vertexAlt.getRanks();

        /// 1. related routes modification
        QVector<Route> modifiedRoutes = graphForUpdate.getRoutes();
        foreach(Route r, routesWillBeUpdatedForRemapping) {
            modifiedRoutes[r.getIndex()] = r;
        }
        graphForUpdate.setRoutes(modifiedRoutes);

        /// 2. replacing two vertices
        // switch index and ranks
        vertexOriginal.setIndex(destAlt);
        vertexOriginal.setRanks(ranksAlt);
        vertexAlt.setIndex(destOriginal);
        vertexAlt.setRanks(ranksOriginal);

        // replace vertices on a graph
        graphForUpdate.setVertexAt(destOriginal, vertexAlt);
        graphForUpdate.setVertexAt(destAlt, vertexOriginal);
        // update coordToVertexIndex map
        graphForUpdate.replaceOneCoordToVertexIndex(vertexOriginal);
        graphForUpdate.replaceOneCoordToVertexIndex(vertexAlt);
        graphForUpdate.updateEdgeLoadBasedOnRoutes();
    } else {
        // reduce load and byte of route for every edge in the original route
        for (int i = 0; i < originalArcs.size() - 1; ++i) {
            int eIndex = graphForUpdate.getEdgeIndexWithSourceTarget(originalArcs[i], originalArcs[i + 1]);
            graphForUpdate.incEdgeLoadAt(eIndex, -originalRoute.getLoad());
            graphForUpdate.incEdgeByteAt(eIndex, -originalRoute.getByte());
        }

        // plus load and byte of route for every edge in the alt route
        for (int i = 0; i < altArcs.size() - 1; ++i) {
            int eIndex = graphForUpdate.getEdgeIndexWithSourceTarget(altArcs[i], altArcs[i + 1]);
            if (eIndex < 0) {
                graphForUpdate.addEdge(Edge(graphForUpdate.getNumEdges(), altArcs[i], altArcs[i + 1], altRoute.getLoad(), altRoute.getByte()));
            } else {
                graphForUpdate.incEdgeLoadAt(eIndex, altRoute.getLoad());
                graphForUpdate.incEdgeByteAt(eIndex, altRoute.getByte());
            }
        }

        // replace route
        graphForUpdate.setRouteAt(originalRoute.getIndex(), altRoute);
    }
}

void RouteGuide::updateGraph()
{
    // TODO: probably, just switch the pointer to graphForUpdate is better (but, need to think which strucure has high modularity)
    // vertices, coordToVertexIndex, edges, routes are updated
    graph->setVertices(graphForUpdate.getVertices());
    graph->setEdges(graphForUpdate.getEdges());
    graph->setRoutes(graphForUpdate.getRoutes());
    graph->setCoordToVertexIndex(graphForUpdate.getCoordToVertexIndex());

    graph->updateGraphInfo();
}

void RouteGuide::clearReplacableVertexMat()
{
    replacableVertexMat.clear();
}
