#include "Graph.h"

Graph::Graph(QVector<Vertex> vertices, QVector<Edge> edges, QVector<Route> routes)
{
    this->vertices = vertices;
    this->edges = edges;
    this->routes = routes;

    allocatingUnusedCoords = true;
    //allocatingUnusedCoords = false; // For Theta
    aveInDegOfEdges = aveInDegOfEdgesLoad = aveOutDegOfEdges = aveOutDegOfEdgesLoad = 0.0;
    medInDegOfEdges = medInDegOfEdgesLoad = medOutDegOfEdges = medOutDegOfEdgesLoad = 0;
    minInDegOfEdges = minInDegOfEdgesLoad = minOutDegOfEdges = minOutDegOfEdgesLoad = 0;
    maxInDegOfEdges = maxInDegOfEdgesLoad = maxOutDegOfEdges = maxOutDegOfEdgesLoad = 0;

    aveInDegOfRoutes = aveInDegOfRoutesLoad = aveOutDegOfRoutes = aveOutDegOfRoutesLoad = 0.0;
    medInDegOfRoutes = medInDegOfRoutesLoad = medOutDegOfRoutes = medOutDegOfRoutesLoad = 0;
    minInDegOfRoutes = minInDegOfRoutesLoad = minOutDegOfRoutes = minOutDegOfRoutesLoad = 0;
    maxInDegOfRoutes = maxInDegOfRoutesLoad = maxOutDegOfRoutes = maxOutDegOfRoutesLoad = 0;

    aveEdgesLoad = 0.0;
    medEdgesLoad = minEdgesLoad = maxEdgesLoad = 0;

    aveLengthOfRoutes = 0.0;
    medLengthOfRoutes = minLengthOfRoutes = maxLengthOfRoutes = 0;

    aveRoutesByte = 0.0;
    medRoutesByte = minRoutesByte = maxRoutesByte = 0.0;

    aveRoutesHopbyte = 0.0;
    medRoutesHopbyte = minRoutesHopbyte = maxRoutesHopbyte = 0.0;
}

bool Graph::loadFromRoutesFile(const QString &rtFilePath, const QString &hopbyteFilePath, int numOfCores, QVector<int> shape, QVector<int> order, QString outputDir, QVector<int> aggregatingDims)
{
    int numOfRanks = numOfCores;
    foreach(int i, shape) {
        numOfRanks *= i;
    }

    // Processing step
    // 1) load coords, edges, routes tagged by ranks from file
    // 2) aggregate results with ranks that have a same node (i.e. different cores but the same node)
    //    - by comparing the coord excluding T dimension, generate 'rank to node index' mapping
    //    - based on the mapping, generate aggregated version of coords, edges, routes

    QFile rtFile(rtFilePath);
    QFile hopbyteFile(hopbyteFilePath);

    if (rtFile.open(QIODevice::ReadOnly)) {
        ///
        /// 1) parse and store the result based on rank
        ///

        // vector for storing the parse result
        QVector< QVector<int> > vertexCoords( QVector<QVector <int> >(numOfRanks, QVector<int>()) );
        QVector< QVector<int> > edgeLoads( QVector< QVector<int> >(numOfRanks, QVector<int>(numOfRanks, 0)) );
        QVector< QVector<float> > edgeBytes( QVector< QVector<float> >(numOfRanks, QVector<float>(numOfRanks, 0.0)) );
        QVector< QVector<int> > routeLoads( QVector< QVector<int> >(numOfRanks, QVector<int>(numOfRanks, 0)) );
        QVector< QVector<float> > routeBytes( QVector< QVector<float> >(numOfRanks, QVector<float>(numOfRanks, 0.0)) );
        QVector< QVector< QVector<int> > > routeArcs( QVector< QVector< QVector<int> > > (numOfRanks, QVector< QVector<int> >(numOfRanks, QVector<int>())));
        bool isHopByteFileLoaded = false;

        if (hopbyteFilePath != "") {
            if (hopbyteFile.open(QIODevice::ReadOnly)) {
                QTextStream hbTs(&hopbyteFile);
                while (!hbTs.atEnd()) {
                    QString line = hbTs.readLine();
                    QStringList elements = line.split(" ");
                    if(elements.size() < 3) {
                        qWarning() << "this line doesn't have enough info: " << line;
                    }
                    int src = elements.at(0).toInt();
                    int dest = elements.at(1).toInt();
                    float byte = elements.at(2).toFloat();
                    routeBytes[src][dest] = byte;
                }
                isHopByteFileLoaded = true;
                hopbyteFile.close();
            }
        }

        QTextStream rtTs(&rtFile);
        while (!rtTs.atEnd()) {
            QString line = rtTs.readLine();
            QStringList elements = parseRouteLine(line);
            if(elements.size() < 6) {
                qWarning() << "this line doesn't have enough info: " << line;
            }
            int hopNum = elements.at(0).toInt();
            int src = elements.at(1).toInt();
            int dest = elements.at(2).toInt();
            int source = elements.at(3).toInt();

            QVector<int> sourceCoord;
            foreach (const QString &str, elements.at(4).split(" ")) {
                sourceCoord.push_back(str.toInt());
            }
            int target = elements.at(5).toInt();
            QVector<int> targetCoord;
            foreach (const QString &str, elements.at(6).split(" ")) {
                targetCoord.push_back(str.toInt());
            }

            // these are overwriting when the same rank nodes is called (no problem)
            vertexCoords[source] = sourceCoord;
            vertexCoords[target] = targetCoord;
            edgeLoads[source][target] += 1;
            if (isHopByteFileLoaded) {
                edgeBytes[source][target] += routeBytes[src][dest];
            } else {
                edgeBytes[source][target] += 1.0;
            }

            if(hopNum == 1) {
                routeLoads[src][dest] += 1;
                routeArcs[src][dest].push_back(source);
            }
            routeArcs[src][dest].push_back(target);
            //routeBytes[src][dest] = 1.0; // for the case hopbyte file is not used
        }
        rtFile.close();

        ///
        /// 2) generate graph object based on the parsed result
        ///
        // 0. make rank-to-index map
        int currentMaxVertexIndex = 0;
        QMap< QVector<int>, int> coordExcludedLastToVertexIndex;
        QMap< int, int> rankToVertexIndex;
        qSort(aggregatingDims.begin(), aggregatingDims.end(), qGreater<int>());

        for (int rank = 0; rank < vertexCoords.size(); ++rank) {
            QVector<int> coord = vertexCoords.at(rank);

            if (coord.size() > 0) {
                QVector<int> coordExcludedLast = vertexCoords.at(rank);
                coordExcludedLast.pop_back();
                foreach (int dim, aggregatingDims) {
                    coordExcludedLast.removeAt(dim);
                }

                QMap< QVector<int>, int>::const_iterator i = coordExcludedLastToVertexIndex.find(coordExcludedLast); // if not exists, this returns end() value

                if (i == coordExcludedLastToVertexIndex.end()) {
                    coordExcludedLastToVertexIndex.insert(coordExcludedLast, currentMaxVertexIndex);
                    currentMaxVertexIndex++;
                }

                int vIndex = coordExcludedLastToVertexIndex[coordExcludedLast];

                rankToVertexIndex.insert(rank, vIndex);
            }
        }

        QVector<int> vertexInDegOfEdges( QVector <int>(currentMaxVertexIndex, 0) );
        QVector<int> vertexOutDegOfEdges( QVector <int>(currentMaxVertexIndex, 0) );
        QVector<int> vertexInDegOfEdgesLoad( QVector <int>(currentMaxVertexIndex, 0) );
        QVector<int> vertexOutDegOfEdgesLoad( QVector <int>(currentMaxVertexIndex, 0) );
        QVector<float> vertexInDegOfEdgesByte( QVector <float>(currentMaxVertexIndex, 0.0) );
        QVector<float> vertexOutDegOfEdgesByte( QVector <float>(currentMaxVertexIndex, 0.0) );
        QVector<int> vertexInDegOfRoutes( QVector <int>(currentMaxVertexIndex, 0) );
        QVector<int> vertexOutDegOfRoutes( QVector <int>(currentMaxVertexIndex, 0) );
        QVector<int> vertexInDegOfRoutesLoad( QVector <int>(currentMaxVertexIndex, 0) );
        QVector<int> vertexOutDegOfRoutesLoad( QVector <int>(currentMaxVertexIndex, 0) );

        // 1. edges
        // aggregate same index edges
        QVector< QVector<int> > edgeTotalLoads( QVector< QVector<int> >(currentMaxVertexIndex, QVector<int>(currentMaxVertexIndex, 0)) );
        QVector< QVector<float> > edgeTotalBytes( QVector< QVector<float> >(currentMaxVertexIndex, QVector<float>(currentMaxVertexIndex, 0.0)) );
        for (int s = 0; s < edgeLoads.size(); ++s) {
            for (int t = 0; t < edgeLoads.at(s).size(); ++t) {
                int load = edgeLoads.at(s).at(t);
                float byte = edgeBytes.at(s).at(t);

                if (load > 0) {
                    if (rankToVertexIndex.contains(s) && rankToVertexIndex.contains(t)) {
                        int sourceIndex = rankToVertexIndex[s];
                        int targetIndex = rankToVertexIndex[t];
                        edgeTotalLoads[sourceIndex][targetIndex] += load;
                        edgeTotalBytes[sourceIndex][targetIndex] += byte;
                    }
                }
            }
        }
        // add edges in graph
        int edgeIndex = 0;
        for (int s = 0; s < edgeTotalLoads.size(); ++s) {
            for (int t = 0; t < edgeTotalLoads.at(s).size(); ++t) {
                int load = edgeTotalLoads.at(s).at(t);
                float byte = edgeTotalBytes.at(s).at(t);
                if (load > 0) {
                    Edge e(edgeIndex, s, t, load, byte);
                    edges.push_back(e);

                    vertexOutDegOfEdges[s] += 1;
                    vertexInDegOfEdges[t] += 1;
                    vertexOutDegOfEdgesLoad[s] += load;
                    vertexInDegOfEdgesLoad[t] += load;
                    vertexOutDegOfEdgesByte[s] += byte;
                    vertexInDegOfEdgesByte[t] += byte;

                    edgeIndex++;
                }
            }
        }
        reorderEdgesByByte();

        // 2. routes
        // aggregate same index routes
        QVector< QVector<int> > routeTotalLoads( QVector< QVector<int> >(currentMaxVertexIndex, QVector<int>(currentMaxVertexIndex, 0)) );
        QVector< QVector<float> > routeTotalBytes( QVector< QVector<float> >(currentMaxVertexIndex, QVector<float>(currentMaxVertexIndex, 0)) );
        QVector< QVector< QVector<int> > > routeAggregatedArcs( QVector< QVector< QVector<int> > > (currentMaxVertexIndex, QVector< QVector<int> >(currentMaxVertexIndex, QVector<int>())));

        for (int s = 0; s < routeLoads.size(); ++s) {
            for (int d = 0; d < routeLoads.at(s).size(); ++d) {
                int load = routeLoads.at(s).at(d);
                float byte = 1.0;
                if (isHopByteFileLoaded) byte = routeBytes.at(s).at(d);
                if (load > 0) {
                    if (rankToVertexIndex.contains(s) && rankToVertexIndex.contains(d)) {
                        int srcIndex = rankToVertexIndex[s];
                        int destIndex = rankToVertexIndex[d];
                        routeTotalLoads[srcIndex][destIndex] += load;
                        /// TODO: maybe need to fix here to handle the case the some of routes before
                        routeAggregatedArcs[srcIndex][destIndex] = routeArcs[s][d];

                        // TODO: here is total value now. we also can change this to maximum byte.
                        routeTotalBytes[srcIndex][destIndex] += byte;
                    }
                }
            }
        }
        // add routes in graph
        int routeIndex = 0;
        for (int s = 0; s < routeTotalLoads.size(); ++s) {
            for (int d = 0; d < routeTotalLoads.at(s).size(); ++d) {
                int load = routeTotalLoads.at(s).at(d);
                float byte = routeTotalBytes.at(s).at(d);
                if (load > 0) {
                    QVector<int> arcs;
                    foreach(int a, routeAggregatedArcs[s][d]) {
                        if (rankToVertexIndex.contains(a)) {
                            arcs.push_back(rankToVertexIndex[a]);
                        }
                    }
                    Route r(routeIndex, s, d, arcs, byte, load, 0);
                    routes.push_back(r);

                    vertexOutDegOfRoutes[s] += 1;
                    vertexInDegOfRoutes[d] += 1;
                    vertexOutDegOfRoutesLoad[s] += load;
                    vertexInDegOfRoutesLoad[d] += load;

                    routeIndex++;
                }
            }
        }
        reorderRoutesByByte();

        // 3. vertices
        QVector< QVector<int> > ranksForAllIndices(currentMaxVertexIndex, QVector<int>());
        QVector<QSet<QVector<int>>> coordsForAllIndices(currentMaxVertexIndex, QSet<QVector<int>>());
        for (int rank = 0; rank < vertexCoords.size(); ++rank) {
            if(rankToVertexIndex.contains(rank)){
                int vIndex = rankToVertexIndex[rank];
                ranksForAllIndices[vIndex].push_back(rank);

                QVector<int> coordExcludedLast = vertexCoords.at(rank);
                coordExcludedLast.pop_back();
                coordsForAllIndices[vIndex].insert(coordExcludedLast);
            }
        }

        for (int index = 0; index < ranksForAllIndices.size(); ++index) {
            QVector<int> coordExcludedLast = vertexCoords.at(ranksForAllIndices[index].at(0));
            coordExcludedLast.pop_back();
            foreach (int dim, aggregatingDims) {
                coordExcludedLast.removeAt(dim);
            }

            Vertex v(index, ranksForAllIndices.at(index), coordExcludedLast, coordsForAllIndices.at(index).toList().toVector());

            v.setInDegOfEdges(vertexInDegOfEdges.at(index));
            v.setOutDegOfEdges(vertexOutDegOfEdges.at(index));
            v.setInDegOfRoutes(vertexInDegOfRoutes.at(index));
            v.setOutDegOfRoutes(vertexOutDegOfRoutes.at(index));
            v.setInDegOfEdgesLoad(vertexInDegOfEdgesLoad.at(index));
            v.setOutDegOfEdgesLoad(vertexOutDegOfEdgesLoad.at(index));
            v.setInDegOfEdgesByte(vertexInDegOfEdgesByte.at(index));
            v.setOutDegOfEdgesByte(vertexOutDegOfEdgesByte.at(index));
            v.setInDegOfRoutesLoad(vertexInDegOfRoutesLoad.at(index));
            v.setOutDegOfRoutesLoad(vertexOutDegOfRoutesLoad.at(index));

            vertices.push_back(v);
        }

        // 4. post calculation (set average degree, etc)
        calcAndSetStatisticalInfo();
        genAndSetSourceTargetToEdgeIndex();
        genAndSetCoordToVertexIndex();
        QVector<int> shapeAfterAggregation = shape;
        QVector<int> orderWithoutAggregatedDims = order;
        foreach (int dim, aggregatingDims) {
            shapeAfterAggregation.remove(dim);
            orderWithoutAggregatedDims.remove(dim);
        }
        QVector<int> orderAfterAggregation;
        QVector<int> sortedOrderWithoutAggregatedDims = orderWithoutAggregatedDims;
        qSort(sortedOrderWithoutAggregatedDims.begin(), sortedOrderWithoutAggregatedDims.end());
        foreach (int o, orderWithoutAggregatedDims) {
            int rank = sortedOrderWithoutAggregatedDims.indexOf(o);
            orderAfterAggregation.push_back(rank);
        }

        if (allocatingUnusedCoords) {
            allocateUnusedCoordsToVertices(shapeAfterAggregation, orderAfterAggregation);
        }

        genAndSetMaxEdgeLoadOfRoutes();
        genAndSetMaxEdgeByteOfRoutes();

        // show graph info summary
        summary();

        // 5. output results
        if (!QDir(outputDir).exists()) {
            QDir().mkdir(outputDir);
        }

        QString agg;
        if (aggregatingDims.size() > 0) agg = "aggregated_";
        outputVerticesInCsv(outputDir + "/" + agg + "vertices.csv");
        outputEdgesInCsv(outputDir + "/" + agg + "edges.csv");
        outputRoutesInCsv(outputDir + "/" + agg + "routes.csv");
        outputSummaryInCsv(outputDir + "/" + agg + "summary.csv");

        outputRouteWeightMatInCsv(outputDir + "/" + agg + "route_weight_mat.csv");

    } else {
        qWarning() << "Couldn't open routes file. file path: " << rtFilePath;
        return false;
    }

    return true;
}

// TODO: should be support the case numOfRanks is indicated
bool Graph::loadFromGraphFiles(const QString &vertexFilePath, const QString &edgeFilePath, const QString &routeFilePath, const QString &summaryFilePath)
{
    vertices.clear();
    edges.clear();
    routes.clear();

    // file format
    // vertex: index,coord1 coord2 ... coordX,rank1 rank2 ... rankY, in-Deg of edge, ...
    // edge: source, target, load
    // route: src, pt1, pt2, pt3, ... , dest
    // summary: num of vertex, num of edge, ...
    QFile vFile(vertexFilePath);
    QFile eFile(edgeFilePath);
    QFile rtFile(routeFilePath);
    QFile smFile(summaryFilePath);

    // edges
    if (eFile.open(QIODevice::ReadOnly)) {
        QTextStream ts(&eFile);

        int edgeIndex = 0;
        while (!ts.atEnd()) {
            QString line = ts.readLine();
            QStringList elements = line.split(",");

            if (elements.size() < 4) {
                qWarning() << "there are not enough elements in edge file";
                return false;
            }

            int source = elements.at(0).toInt();
            int target = elements.at(1).toInt();
            int load = elements.at(2).toInt();
            float byte = elements.at(3).toFloat();

            Edge e(edgeIndex, source, target, load, byte);
            edges.push_back(e);

            edgeIndex++;
        }

        eFile.close();
        reorderEdgesByByte();
    } else {
        qWarning() << "Couldn't open edge file. file path: " << edgeFilePath;
        return false;
    }

    // routes
    if (rtFile.open(QIODevice::ReadOnly)) {
        QTextStream ts(&rtFile);

        int routeIndex = 0;
        while (!ts.atEnd()) {
            QString line = ts.readLine();
            QStringList elements = line.split(",");

            if (elements.size() < 5) {
                qWarning() << "there are not enough elements in route file";
                return false;
            }

            QStringList arcsStr = elements.at(0).split(" ");
            int src = arcsStr.first().toInt();
            int dest = arcsStr.last().toInt();
            QVector<int> arcs;
            foreach(QString s, arcsStr) {
                arcs.push_back(s.toInt());
            }
            int load = elements.at(1).toInt();
            float byte = elements.at(2).toFloat();
            int maxEdgeLoad = elements.at(3).toInt();
            float maxEdgeByte = elements.at(4).toFloat();

            Route r(routeIndex, src, dest, arcs, byte, load, maxEdgeLoad, maxEdgeByte);
            routes.push_back(r);

            routeIndex++;
        }
        rtFile.close();
        reorderRoutesByByte();
    } else {
        qWarning() << "Couldn't open vertex file. file path: " << routeFilePath;
        return false;
    }


    //vertices
    if (vFile.open(QIODevice::ReadOnly)) {

        QTextStream ts(&vFile);

        while (!ts.atEnd()) {
            QString line = ts.readLine();

            QStringList elements = line.split(",");

            if (elements.size() < 14) {
                qWarning() << "there are not enough elements in vertex file";
                return false;
            }

            int vertexIndex = elements.at(0).toInt();

            QStringList coordsStr;
            if (elements.size() > 1) coordsStr = elements.at(1).split(" ");
            QVector<int> coord;
            foreach(QString s, coordsStr) {
                coord.push_back(s.toInt());
            }


            QStringList childCoordsStr;
            if (elements.size() > 2) childCoordsStr = elements.at(2).split(";");
            QVector<QVector<int>> childCoords;
            foreach(QString s, childCoordsStr) {
                QStringList childCoordStr;

                QVector<int> childCoord;
                childCoordStr = s.split(" ");
                foreach(QString s2, childCoordStr) {
                    childCoord.push_back(s2.toInt());
                }

                childCoords.push_back(childCoord);
            }

            QStringList ranksStr;
            if (elements.size() > 3) ranksStr = elements.at(3).split(" ");
            QVector<int> ranks;
            foreach(QString r, ranksStr) {
                ranks.push_back(r.toInt());
            }

            Vertex v(vertexIndex, ranks, coord, childCoords);

            v.setInDegOfEdges(elements.at(4).toInt());
            v.setInDegOfEdgesLoad(elements.at(5).toInt());
            v.setInDegOfEdgesByte(elements.at(6).toFloat());
            v.setOutDegOfEdges(elements.at(7).toInt());
            v.setOutDegOfEdgesLoad(elements.at(8).toInt());
            v.setOutDegOfEdgesByte(elements.at(9).toFloat());
            v.setInDegOfRoutes(elements.at(10).toInt());
            v.setOutDegOfRoutes(elements.at(11).toInt());
            v.setInDegOfRoutesLoad(elements.at(12).toInt());
            v.setOutDegOfRoutesLoad(elements.at(13).toInt());

            vertices.push_back(v);

            vertexIndex++;
        }
        vFile.close();

    } else {
        qWarning() << "Couldn't open vertex file. file path: " << vertexFilePath;
        return false;
    }

    if (smFile.open(QIODevice::ReadOnly)) {

        QTextStream ts(&smFile);

        while (!ts.atEnd()) {
            QString line = ts.readLine();

            QStringList elements = line.split(",");

            if (elements.size() < 63) {
                qWarning() << "there are not enough elements in summary file";
                return false;
            }

            aveEdgesLoad = elements.at(3).toDouble();
            medEdgesLoad = elements.at(4).toInt();
            minEdgesLoad = elements.at(5).toInt();
            maxEdgesLoad = elements.at(6).toInt();
            aveEdgesByte = elements.at(7).toDouble();
            medEdgesByte = elements.at(8).toFloat();
            minEdgesByte = elements.at(9).toFloat();
            maxEdgesByte = elements.at(10).toFloat();
            aveLengthOfRoutes = elements.at(11).toDouble();
            medLengthOfRoutes = elements.at(12).toInt();
            minLengthOfRoutes = elements.at(13).toInt();
            maxLengthOfRoutes = elements.at(14).toInt();
            aveRoutesByte = elements.at(15).toDouble();
            medRoutesByte = elements.at(16).toFloat();
            minRoutesByte = elements.at(17).toFloat();
            maxRoutesByte = elements.at(18).toFloat();
            aveRoutesHopbyte = elements.at(19).toDouble();
            medRoutesHopbyte = elements.at(20).toFloat();
            minRoutesHopbyte = elements.at(21).toFloat();
            maxRoutesHopbyte = elements.at(22).toFloat();
            aveInDegOfEdges = elements.at(23).toDouble();
            medInDegOfEdges = elements.at(24).toInt();
            minInDegOfEdges = elements.at(25).toInt();
            maxInDegOfEdges = elements.at(26).toInt();
            aveInDegOfEdgesLoad = elements.at(27).toDouble();
            medInDegOfEdgesLoad = elements.at(28).toInt();
            minInDegOfEdgesLoad = elements.at(29).toInt();
            maxInDegOfEdgesLoad = elements.at(30).toInt();
            aveInDegOfEdgesByte = elements.at(31).toDouble();
            medInDegOfEdgesByte = elements.at(32).toFloat();
            minInDegOfEdgesByte = elements.at(33).toFloat();
            maxInDegOfEdgesByte = elements.at(34).toFloat();
            aveOutDegOfEdges = elements.at(35).toInt();
            medOutDegOfEdges = elements.at(36).toInt();
            minOutDegOfEdges = elements.at(37).toInt();
            maxOutDegOfEdges = elements.at(38).toInt();
            aveOutDegOfEdgesLoad = elements.at(39).toDouble();
            medOutDegOfEdgesLoad = elements.at(40).toInt();
            minOutDegOfEdgesLoad = elements.at(41).toInt();
            maxOutDegOfEdgesLoad = elements.at(42).toInt();
            aveOutDegOfEdgesByte = elements.at(43).toDouble();
            medOutDegOfEdgesByte = elements.at(44).toFloat();
            minOutDegOfEdgesByte = elements.at(45).toFloat();
            maxOutDegOfEdgesByte = elements.at(46).toFloat();
            aveInDegOfRoutes = elements.at(47).toDouble();
            medInDegOfRoutes = elements.at(48).toInt();
            minInDegOfRoutes = elements.at(49).toInt();
            maxInDegOfRoutes = elements.at(50).toInt();
            aveInDegOfRoutesLoad = elements.at(51).toDouble();
            medInDegOfRoutesLoad = elements.at(52).toInt();
            minInDegOfRoutesLoad = elements.at(53).toInt();
            maxInDegOfRoutesLoad = elements.at(54).toInt();
            aveOutDegOfRoutes = elements.at(55).toDouble();
            medOutDegOfRoutes = elements.at(56).toInt();
            minOutDegOfRoutes = elements.at(57).toInt();
            maxOutDegOfRoutes = elements.at(58).toInt();
            aveOutDegOfRoutesLoad = elements.at(59).toDouble();
            medOutDegOfRoutesLoad = elements.at(60).toInt();
            minOutDegOfRoutesLoad = elements.at(61).toInt();
            maxOutDegOfRoutesLoad = elements.at(62).toInt();
        }
    } else {
        qWarning() << "Couldn't open vertex file. file path: " << summaryFilePath;
        return false;
    }

    genAndSetSourceTargetToEdgeIndex();
    genAndSetCoordToVertexIndex();
    //genAndSetMaxEdgeLoadOfRoutes();
    //genAndSetMaxEdgeByteOfRoutes();

    // show graph info summary
    summary();

    return true;
}

bool Graph::loadFromPosFile(const QString &filePath)
{
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly)) {
        int index = 0;
        int vSize = vertices.size();

        QTextStream ts(&file);
        while (!ts.atEnd()) {
            if (index >= vSize) {
                qWarning() << "number of positions in a file is larger than vertices";
                break;
            }
            QString line = ts.readLine();
            QStringList pos = line.split(",");
            vertices[index].setGraphPos(QPointF( pos.at(0).toFloat(), pos.at(1).toFloat()));
            index++;
        }
    } else {
        qWarning() << "Couldn't open positions file. file path: " << filePath;
        return false;
    }

    return true;
}

QStringList Graph::parseRouteLine(QString line)
{
    // line format -- Hop #1: [68-124] 68 (0 2 0 2 0) -> 100 (0 3 0 2 0)
    // line format -- Hop 1: [70-126] 70 (0 2 0 3 0 0) -> 102 (0 3 0 3 0 0)
    // result -- ["1", "68", "124", " 68 ", "0 2 0 2 0", " 100 ", "0 3 0 2 0"]
    QStringList result;
    QStringList tmpSplitResult;
    int tmpIndex1, tmpIndex2;

    // TODO: need to add error handling
    line = line.mid(line.indexOf("Hop") + 3); // 1: [68-124] 68 (0 2 0 2 0) -> 100 (0 3 0 2 0)
    QString hopNum = line.mid(0, line.indexOf(":"));
    tmpIndex1 = line.indexOf("[");
    tmpIndex2 = line.indexOf("]");
    QString srcAndDest = line.mid(tmpIndex1 + 1, tmpIndex2 - tmpIndex1 - 1); // 68-124
    tmpSplitResult = srcAndDest.split("-");
    QString src = tmpSplitResult[0];
    QString dest = tmpSplitResult[1];

    line = line.mid(tmpIndex2 + 1); //  68 (0 2 0 2 0) -> 100 (0 3 0 2 0)
    tmpSplitResult = line.split("->");
    QString sourceAndCoord = tmpSplitResult[0]; //  68 (0 2 0 2 0)
    QString targetAndCoord = tmpSplitResult[1]; //  100 (0 3 0 2 0)
    tmpIndex1 = sourceAndCoord.indexOf("(");
    tmpIndex2 = sourceAndCoord.indexOf(")");
    QString source = sourceAndCoord.mid(0, tmpIndex1);
    QString sCoord = sourceAndCoord.mid(tmpIndex1 + 1, tmpIndex2 - tmpIndex1 - 1);
    tmpIndex1 = targetAndCoord.indexOf("(");
    tmpIndex2 = targetAndCoord.indexOf(")");
    QString target= targetAndCoord.mid(0, tmpIndex1);
    QString tCoord = targetAndCoord.mid(tmpIndex1 + 1, tmpIndex2 - tmpIndex1 - 1);

    return result << hopNum << src << dest << source << sCoord << target << tCoord;
}

void Graph::calcAndSetStatisticalInfo()
{
    // vertices
    for (int i = 0; i < vertices.size(); ++i) {
        Vertex v = vertices.at(i);
        if (i == 0) {
            aveInDegOfEdges = minInDegOfEdges = maxInDegOfEdges = v.getInDegOfEdges();
            aveInDegOfEdgesLoad = minInDegOfEdgesLoad = maxInDegOfEdgesLoad = v.getInDegOfEdgesLoad();
            aveInDegOfEdgesByte = minInDegOfEdgesByte = maxInDegOfEdgesByte = v.getInDegOfEdgesByte();
            aveOutDegOfEdges = minOutDegOfEdges = maxOutDegOfEdges = v.getOutDegOfEdges();
            aveOutDegOfEdgesLoad = minOutDegOfEdgesLoad = maxOutDegOfEdgesLoad = v.getOutDegOfEdgesLoad();
            aveOutDegOfEdgesByte = minOutDegOfEdgesByte = maxOutDegOfEdgesByte = v.getOutDegOfEdgesByte();
            aveInDegOfRoutes = minInDegOfRoutes = maxOutDegOfRoutes = v.getInDegOfRoutes();
            aveInDegOfRoutesLoad = minInDegOfRoutesLoad = maxOutDegOfRoutesLoad = v.getInDegOfRoutesLoad();
            aveOutDegOfRoutes = minOutDegOfRoutes = maxOutDegOfRoutes = v.getOutDegOfRoutes();
            aveOutDegOfRoutesLoad = minOutDegOfRoutesLoad = maxOutDegOfRoutesLoad = v.getOutDegOfRoutesLoad();
        } else {
            aveInDegOfEdges += v.getInDegOfEdges();
            minInDegOfEdges = min(minInDegOfEdges, v.getInDegOfEdges());
            maxInDegOfEdges = max(maxInDegOfEdges, v.getInDegOfEdges());
            aveInDegOfEdgesLoad += v.getInDegOfEdgesLoad();
            minInDegOfEdgesLoad = min(minInDegOfEdgesLoad, v.getInDegOfEdgesLoad());
            maxInDegOfEdgesLoad = max(maxInDegOfEdgesLoad, v.getInDegOfEdgesLoad());
            aveInDegOfEdgesByte += v.getInDegOfEdgesByte();
            minInDegOfEdgesByte = min(minInDegOfEdgesByte, v.getInDegOfEdgesByte());
            maxInDegOfEdgesByte = max(maxInDegOfEdgesByte, v.getInDegOfEdgesByte());

            aveOutDegOfEdges += v.getOutDegOfEdges();
            minOutDegOfEdges = min(minOutDegOfEdges, v.getOutDegOfEdges());
            maxOutDegOfEdges = max(maxOutDegOfEdges, v.getOutDegOfEdges());
            aveOutDegOfEdgesLoad += v.getOutDegOfEdgesLoad();
            minOutDegOfEdgesLoad = min(minOutDegOfEdgesLoad, v.getOutDegOfEdgesLoad());
            maxOutDegOfEdgesLoad = max(maxOutDegOfEdgesLoad, v.getOutDegOfEdgesLoad());
            aveOutDegOfEdgesByte += v.getOutDegOfEdgesByte();
            minOutDegOfEdgesByte = min(minOutDegOfEdgesByte, v.getOutDegOfEdgesByte());
            maxOutDegOfEdgesByte = max(maxOutDegOfEdgesByte, v.getOutDegOfEdgesByte());

            aveInDegOfRoutes += v.getInDegOfRoutes();
            minInDegOfRoutes = min(minInDegOfRoutes, v.getInDegOfRoutes());
            maxInDegOfRoutes = max(maxInDegOfRoutes, v.getInDegOfRoutes());
            aveInDegOfRoutesLoad += v.getInDegOfRoutesLoad();
            minInDegOfRoutesLoad = min(minInDegOfRoutesLoad, v.getInDegOfRoutesLoad());
            maxInDegOfRoutesLoad = max(maxInDegOfRoutesLoad, v.getInDegOfRoutesLoad());

            aveOutDegOfRoutes += v.getOutDegOfRoutes();
            minOutDegOfRoutes = min(minOutDegOfRoutes, v.getOutDegOfRoutes());
            maxOutDegOfRoutes = max(maxOutDegOfRoutes, v.getOutDegOfRoutes());
            aveOutDegOfRoutesLoad += v.getOutDegOfRoutesLoad();
            minOutDegOfRoutesLoad = min(minOutDegOfRoutesLoad, v.getOutDegOfRoutesLoad());
            maxOutDegOfRoutesLoad = max(maxOutDegOfRoutesLoad, v.getOutDegOfRoutesLoad());
        }
    }
    aveInDegOfEdges /= (float)vertices.size();
    aveInDegOfEdgesLoad /= (float)vertices.size();
    aveInDegOfEdgesByte /= (float)vertices.size();
    aveOutDegOfEdges /= (float)vertices.size();
    aveOutDegOfEdgesLoad /= (float)vertices.size();
    aveOutDegOfEdgesByte /= (float)vertices.size();
    aveInDegOfRoutes /= (float)vertices.size();
    aveOutDegOfRoutes /= (float)vertices.size();
    aveInDegOfRoutesLoad /= (float)vertices.size();
    aveOutDegOfRoutesLoad /= (float)vertices.size();

    int numVertices = vertices.size();
    int medianIndex1 = numVertices / 2;
    int medianIndex2 = numVertices / 2 + 1;
    QVector<Vertex> sortedVertices;

    sortedVertices = genSortedVerticesByInDegOfEdges();
    if (numVertices % 2 != 0) medInDegOfEdges = sortedVertices[medianIndex2].getInDegOfEdges();
    else medInDegOfEdges = (sortedVertices[medianIndex1].getInDegOfEdges() + sortedVertices[medianIndex2].getInDegOfEdges()) * 0.5;

    sortedVertices = genSortedVerticesByInDegOfEdgesLoad();
    if (numVertices % 2 != 0) medInDegOfEdgesLoad = sortedVertices[medianIndex2].getInDegOfEdgesLoad();
    else medInDegOfEdgesLoad = (sortedVertices[medianIndex1].getInDegOfEdgesLoad() + sortedVertices[medianIndex2].getInDegOfEdgesLoad()) * 0.5;

    sortedVertices = genSortedVerticesByInDegOfEdgesByte();
    if (numVertices % 2 != 0) medInDegOfEdgesByte = sortedVertices[medianIndex2].getInDegOfEdgesByte();
    else medInDegOfEdgesByte = (sortedVertices[medianIndex1].getInDegOfEdgesByte() + sortedVertices[medianIndex2].getInDegOfEdgesByte()) * 0.5;

    sortedVertices = genSortedVerticesByOutDegOfEdges();
    if (numVertices % 2 != 0) medOutDegOfEdges = sortedVertices[medianIndex2].getOutDegOfEdges();
    else medOutDegOfEdges = (sortedVertices[medianIndex1].getOutDegOfEdges() + sortedVertices[medianIndex2].getOutDegOfEdges()) * 0.5;

    sortedVertices = genSortedVerticesByOutDegOfEdgesLoad();
    if (numVertices % 2 != 0) medOutDegOfEdgesLoad = sortedVertices[medianIndex2].getOutDegOfEdgesLoad();
    else medOutDegOfEdgesLoad = (sortedVertices[medianIndex1].getOutDegOfEdgesLoad() + sortedVertices[medianIndex2].getOutDegOfEdgesLoad()) * 0.5;

    sortedVertices = genSortedVerticesByOutDegOfEdgesByte();
    if (numVertices % 2 != 0) medOutDegOfEdgesByte = sortedVertices[medianIndex2].getOutDegOfEdgesByte();
    else medOutDegOfEdgesLoad = (sortedVertices[medianIndex1].getOutDegOfEdgesByte() + sortedVertices[medianIndex2].getOutDegOfEdgesByte()) * 0.5;

    sortedVertices = genSortedVerticesByInDegOfRoutes();
    if (numVertices % 2 != 0) medInDegOfRoutes = sortedVertices[medianIndex2].getInDegOfRoutes();
    else medInDegOfRoutes = (sortedVertices[medianIndex1].getInDegOfRoutes() + sortedVertices[medianIndex2].getInDegOfRoutes()) * 0.5;

    sortedVertices = genSortedVerticesByInDegOfRoutesLoad();
    if (numVertices % 2 != 0) medInDegOfRoutesLoad = sortedVertices[medianIndex2].getInDegOfRoutesLoad();
    else medInDegOfRoutesLoad = (sortedVertices[medianIndex1].getInDegOfRoutesLoad() + sortedVertices[medianIndex2].getInDegOfRoutesLoad()) * 0.5;

    sortedVertices = genSortedVerticesByOutDegOfRoutes();
    if (numVertices % 2 != 0) medOutDegOfRoutes = sortedVertices[medianIndex2].getOutDegOfRoutes();
    else medOutDegOfRoutes = (sortedVertices[medianIndex1].getOutDegOfRoutes() + sortedVertices[medianIndex2].getOutDegOfRoutes()) * 0.5;

    sortedVertices = genSortedVerticesByOutDegOfRoutesLoad();
    if (numVertices % 2 != 0) medOutDegOfRoutesLoad = sortedVertices[medianIndex2].getOutDegOfRoutesLoad();
    else medOutDegOfRoutesLoad = (sortedVertices[medianIndex1].getOutDegOfRoutesLoad() + sortedVertices[medianIndex2].getOutDegOfRoutesLoad()) * 0.5;

    // edges
    for (int i = 0; i < edges.size(); ++i) {
        Edge e = edges.at(i);
        if (i == 0) {
            aveEdgesLoad = minEdgesLoad = maxEdgesLoad = e.getLoad();
            aveEdgesByte = minEdgesByte = maxEdgesByte = e.getByte();
        } else {
            aveEdgesLoad += e.getLoad();
            minEdgesLoad = min(minEdgesLoad, e.getLoad());
            maxEdgesLoad = max(maxEdgesLoad, e.getLoad());
            aveEdgesByte += e.getByte();
            minEdgesByte = min(minEdgesByte, e.getByte());
            maxEdgesByte = max(maxEdgesByte, e.getByte());
        }
    }
    aveEdgesLoad  /= (float)edges.size();
    aveEdgesByte  /= (float)edges.size();

    int numEdges = edges.size();
    medianIndex1 = numEdges / 2;
    medianIndex2 = numEdges / 2 + 1;

    QVector<Edge> sortedEdges = genSortedEdgesByLoad();
    if (numEdges % 2 != 0) medEdgesLoad = sortedEdges[medianIndex2].getLoad();
    else medEdgesLoad = (sortedEdges[medianIndex1].getLoad() + sortedEdges[medianIndex2].getLoad()) * 0.5;

    sortedEdges = genSortedEdgesByByte();
    if (numEdges % 2 != 0) medEdgesByte = sortedEdges[medianIndex2].getByte();
    else medEdgesLoad = (sortedEdges[medianIndex1].getByte() + sortedEdges[medianIndex2].getByte()) * 0.5;

    // routes
    for (int i = 0; i < routes.size(); ++i) {
        Route r = routes.at(i);
        if (i == 0) {
            aveLengthOfRoutes = minLengthOfRoutes = maxLengthOfRoutes = r.getArcs().size() - 1;
            aveRoutesByte = minRoutesByte = maxRoutesByte = r.getByte();
            aveRoutesHopbyte = minRoutesHopbyte = maxRoutesHopbyte = r.getHopbyte();
        } else {
            aveLengthOfRoutes += r.getArcs().size() - 1;
            minLengthOfRoutes = min(minLengthOfRoutes, r.getArcs().size() - 1);
            maxLengthOfRoutes = max(maxLengthOfRoutes, r.getArcs().size() - 1);
            aveRoutesByte += r.getByte();
            minRoutesByte = min(minRoutesByte, r.getByte());
            maxRoutesByte = max(maxRoutesByte, r.getByte());
            aveRoutesHopbyte += r.getHopbyte();
            minRoutesHopbyte = min(minRoutesByte, r.getHopbyte());
            maxRoutesHopbyte = max(maxRoutesByte, r.getHopbyte());
        }
    }
    aveLengthOfRoutes /= (float)routes.size();
    aveRoutesByte /= (float)routes.size();
    aveRoutesHopbyte /= (float)routes.size();

    int numRoutes = routes.size();
    medianIndex1 = numRoutes / 2;
    medianIndex2 = numRoutes / 2 + 1;
    QVector<Route> sortedRoutes;
    if (numRoutes % 2 != 0) {
        medLengthOfRoutes = routes[medianIndex2].getLength();
        sortedRoutes = genSortedRoutesByByte();
        medRoutesByte = sortedRoutes[medianIndex2].getByte();
        sortedRoutes = genSortedRoutesByHopbyte();
        medRoutesHopbyte = sortedRoutes[medianIndex2].getByte();
    } else {
        medLengthOfRoutes = (routes[medianIndex1].getLength() + routes[medianIndex2].getLength()) * 0.5;
        sortedRoutes = genSortedRoutesByByte();
        medRoutesByte = (sortedRoutes[medianIndex1].getByte() + sortedRoutes[medianIndex2].getByte()) * 0.5;
        sortedRoutes = genSortedRoutesByHopbyte();
        medRoutesHopbyte = (sortedRoutes[medianIndex1].getByte() + sortedRoutes[medianIndex2].getByte()) * 0.5;
    }
}

void Graph::summary()
{
    qDebug() << "===== Graph Summary =====";
    qDebug() << "num of vertices: " << vertices.size();
    qDebug() << "num of edges: " << edges.size();
    qDebug() << "num of routes: " << routes.size();
    qDebug("");
    qDebug() << "load on edges: ave "    << aveEdgesLoad      << ", med " << medEdgesLoad      << ", min " << minEdgesLoad      << ", max " << maxEdgesLoad;
    qDebug() << "byte on edges: ave "    << aveEdgesByte      << ", med " << medEdgesByte      << ", min " << minEdgesByte      << ", max " << maxEdgesByte;
    qDebug() << "length of routes: ave " << aveLengthOfRoutes << ", med " << medLengthOfRoutes << ", min"  << minLengthOfRoutes << ", max " << maxLengthOfRoutes;
    qDebug() << "byte on routes: ave " << aveRoutesByte << ", med " << medRoutesByte << ", min"  << minRoutesByte << ", max " << maxRoutesByte;
    qDebug() << "hop-byte on routes: ave " << aveRoutesHopbyte << ", med " << medRoutesHopbyte << ", min"  << minRoutesHopbyte << ", max " << maxRoutesHopbyte;
    qDebug("");
    qDebug() << "in-degree with edges (unweighted): ave "          << aveInDegOfEdges      << ", med " << medInDegOfEdges      << ", min " << minInDegOfEdges      << ", max " << maxInDegOfEdges;
    qDebug() << "in-degree with edges (weighted with load): ave "  << aveInDegOfEdgesLoad  << ", med " << medInDegOfEdgesLoad  << ", min " << minInDegOfEdgesLoad  << ", max " << maxInDegOfEdgesLoad;
    qDebug() << "out-degree with edges (unweighted): ave "         << aveOutDegOfEdges     << ", med " << medOutDegOfEdges     << ", min " << minOutDegOfEdges     << ", max " << maxOutDegOfEdges;
    qDebug() << "out-degree with edges (weighted with load): ave " << aveOutDegOfEdgesLoad << ", med " << medOutDegOfEdgesLoad << ", min " << minOutDegOfEdgesLoad << ", max " << maxOutDegOfEdgesLoad;
    qDebug("");
    qDebug() << "in-degree with routes (unweighted): ave "          << aveInDegOfRoutes      << ", med " << medInDegOfRoutes      << ", min " << minInDegOfRoutes      << ", max " << maxInDegOfRoutes;
    qDebug() << "in-degree with routes (weighted with load): ave "  << aveInDegOfRoutesLoad  << ", med " << medInDegOfRoutesLoad  << ", min " << minInDegOfRoutesLoad  << ", max " << maxInDegOfRoutesLoad;
    qDebug() << "out-degree with routes (unweighted): ave "         << aveOutDegOfRoutes     << ", med " << medOutDegOfRoutes     << ", min " << minOutDegOfRoutes     << ", max " << maxOutDegOfRoutes;
    qDebug() << "out-degree with routes (weighted with load): ave " << aveOutDegOfRoutesLoad << ", med " << medOutDegOfRoutesLoad << ", min " << minOutDegOfRoutesLoad << ", max " << maxOutDegOfRoutesLoad;
    qDebug("");
    qDebug() << "total hop-byte: " << calcTotalHobByte();
}

double Graph::jaccardSim(QVector<int> v1, QVector<int> v2)
{
    if(v1.size() != v2.size()){
        qWarning() << "vectors not the same size";
        exit(1);
    }

    int a11 = 0, a00 = 0;

    for (int i = 0; i < v1.size(); ++i) {
        if (v1[i] == v2[i]) {
            if (v1[i]) a11++;
            else a00++;
        }
    }
    if (v1.size() - (a11 + a00) == 0) {
        return 0;
    }
    return double(a11) / double(v1.size() - a00);
}

double Graph::calcTotalHobByte()
{
    double result = 0.0;
    foreach (Route r, routes) {
        result += r.getHopbyte();
    }
    return result;
}

void Graph::genAndSetSourceTargetToEdgeIndex()
{
    sourceTargetToEdgeIndex.clear();
    foreach(Edge e, edges) {
        QPair<int, int> sourceTarget = qMakePair(e.getSource(), e.getTarget());
        sourceTargetToEdgeIndex.insert(sourceTarget, e.getIndex());
    }
}

void Graph::genAndSetCoordToVertexIndex()
{
    coordToVertexIndex.clear();
    foreach(Vertex v, vertices) {
        coordToVertexIndex.insert(v.getCoord(), v.getIndex());
    }
}

void Graph::replaceOneCoordToVertexIndex(Vertex vertex)
{
    QVector<int> coord;
    coordToVertexIndex.remove(coord);
    coordToVertexIndex.insert(coord, vertex.getIndex());
}

void Graph::genAndSetMaxEdgeLoadOfRoutes()
{
    for (int i = 0; i < routes.size(); ++i) {
        int maxEdgeLoad = 0;
        QVector<int> arcs = routes[i].getArcs();
        for (int j = 0; j < arcs.size() - 1; ++j) {
            Edge e = getEdgeWithSourceTarget(arcs.at(j), arcs.at(j+1));
            if (e.getLoad() > maxEdgeLoad) maxEdgeLoad = e.getLoad();
        }
        routes[i].setMaxEdgeLoad(maxEdgeLoad);
    }
}

void Graph::genAndSetMaxEdgeByteOfRoutes()
{
    for (int i = 0; i < routes.size(); ++i) {
        float maxEdgeByte = 0.0;
        QVector<int> arcs = routes[i].getArcs();
        for (int j = 0; j < arcs.size() - 1; ++j) {
            Edge e = getEdgeWithSourceTarget(arcs.at(j), arcs.at(j+1));
            if (e.getByte() > maxEdgeByte) maxEdgeByte = e.getByte();
        }
        routes[i].setMaxEdgeByte(maxEdgeByte);
    }
}

void Graph::allocateUnusedCoordsToVertices(QVector<int> shape, QVector<int> order)
{
    int numOfNodes = 1;
    foreach(int i, shape) {
        numOfNodes *= i;
    }
    qDebug() << numOfNodes;
    int divNumStart = 1;
    for (int i = 0; i < order.size() - 1; ++i) {
        divNumStart *= shape.at(order.at(i));
    }

    for (int i = 0; i < numOfNodes; ++i) {

        QVector<int> coord(shape.size(), 0);
        int divNum = divNumStart;
        int n = i;

        for(int j = 0; j < order.size() - 1; ++j) {
            int o1 = order.at(order.size() - 1 - j);
            int o2 = order.at(order.size() - 2 - j);
            int quot = n / divNum;
            coord[o1] = quot;

            n -= quot * divNum;
            divNum /= shape.at(o2);
        }
        coord[order.at(0)] = n;

        QMap<QVector<int>, int>::const_iterator iter = coordToVertexIndex.find(coord);

        // if find unused coord, make vertex that has the unused coord
        if ( iter == coordToVertexIndex.end() ) {
            int nextIndex = vertices.size();
            vertices.push_back(Vertex(nextIndex, QVector<int>(), coord));
        }
    }
}

void Graph::outputVerticesInCsv(QString csvFilePath)
{
    QFile file(csvFilePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) qWarning() << "cannot open file: " << csvFilePath;
    QTextStream out(&file);

    foreach (Vertex v, vertices) {
        QVector<int> coord = v.getCoord();
        QVector<QVector<int>> childCoords = v.getChildCoords();
        QVector<int> ranks = v.getRanks();

        out << v.getIndex();
        for (int i = 0; i < coord.size(); ++i) {
            if (i == 0) out << "," << coord.at(i);
            else out << " " << coord.at(i);
        }
        if (coord.size() == 0) {
            out << ",";
        }

        for (int i = 0; i < childCoords.size(); ++i) {
            for (int j = 0; j < childCoords.at(i).size(); ++j) {
                if (i == 0 && j == 0) out << "," << childCoords.at(i).at(j);
                else if (i != 0 && j == 0) out << ";" << childCoords.at(i).at(j);
                else out << " " << childCoords.at(i).at(j);
            }
        }
        if (childCoords.size() == 0) {
            out << ",";
        }

        for (int i = 0; i < ranks.size(); ++i) {
            if (i == 0) out << "," << ranks.at(i);
            else out << " " << ranks.at(i);
        }
        if (ranks.size() == 0) {
            out << ",";
        }
        out << "," << v.getInDegOfEdges()  << "," << v.getInDegOfEdgesLoad()  << "," << v.getInDegOfEdgesByte()
            << "," << v.getOutDegOfEdges() << "," << v.getOutDegOfEdgesLoad() << "," << v.getOutDegOfEdgesByte();
        out << "," << v.getInDegOfRoutes() << "," << v.getInDegOfRoutesLoad() << "," << v.getOutDegOfRoutes() << "," << v.getOutDegOfRoutesLoad();
        out << "\n";
    }
}

void Graph::outputEdgesInCsv(QString csvFilePath)
{
    QFile file(csvFilePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) qWarning() << "cannot open file: " << csvFilePath;
    QTextStream out(&file);

    foreach (Edge e, edges) {
        out << e.getSource() << "," << e.getTarget() << "," << e.getLoad() << "," << e.getByte() << "\n";
    }
    file.close();
}

void Graph::outputRoutesInCsv(QString csvFilePath)
{
    QFile file(csvFilePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) qWarning() << "cannot open file: " << csvFilePath;
    QTextStream out(&file);

    foreach (Route r, routes) {
        QVector<int> arcs = r.getArcs();
        for (int i = 0; i < arcs.size(); ++i) {
            if (i > 0) out << " ";
            out << arcs.at(i);
        }
        out << "," << r.getLoad() << "," << r.getByte() << "," << r.getMaxEdgeLoad() << "," << r.getMaxEdgeByte();
        out << "\n";
    }
}

void Graph::outputRouteSrcDestLoadInCsv(QString csvFilePath)
{
    QFile file(csvFilePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) qWarning() << "cannot open file: " << csvFilePath;
    QTextStream out(&file);

    foreach (Route r, routes) {
        out << r.getSrc() << "," << r.getDest() << "," << r.getLoad() << "\n";
    }
}

// TODO: this is just for testing with c1 now. change later.
// Probably, if we need to think precise results, we should produce this from raw route file
// Also this produces symmetric matrix because of requirement of R libarary
void Graph::outputRouteWeightMatInCsv(QString csvFilePath, QString outputOrderFilePath)
{
    // value 0: no route between src and dest, some value: sum of loads between nodes (no consideration of order)
    QVector<int> outputOrder;
    if (outputOrderFilePath != "") {
        QFile orderFile(outputOrderFilePath);
        if (orderFile.open(QIODevice::ReadOnly)) {
            QTextStream ts(&orderFile);

            while (!ts.atEnd()) {
                QString line = ts.readLine();
                outputOrder.push_back(line.toInt());
            }

            orderFile.close();
        } else {
            qWarning() << "Couldn't open output order file. file path: " << outputOrderFilePath;
        }
    }

    QFile file(csvFilePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) qWarning() << "cannot open file: " << csvFilePath;
    QTextStream out(&file);

    QVector<QVector<float> > connMat(vertices.size(), QVector<float>(vertices.size(), 0));

    foreach (Route r, routes) {
        int src = r.getSrc();
        int dest = r.getDest();

        // load
        //connMat[src][dest] += r.getLoad();
        //connMat[dest][src] += r.getLoad();

        // hop byte
        //connMat[src][dest] += r.getHopbyte();
        //connMat[src][dest] += r.getHopbyte();

        // byte
        connMat[src][dest] += r.getByte();
        connMat[src][dest] += r.getByte();
    }

    for (int i = 0; i < connMat.size(); ++i) {
        for (int j = 0; j < connMat.at(i).size(); ++j) {
            int x, y;
            if (outputOrder.size() != 0) {
                x = outputOrder.at(i);
                y = outputOrder.at(j);
            } else {
                x = i;
                y = j;
            }

            // to make symmetric matrix, take max value
            if (j == 0) out << qMax(connMat.at(x).at(y), connMat.at(y).at(x));
            else out << "," << qMax(connMat.at(x).at(y), connMat.at(y).at(x));
        }
        out << "\n";
    }
}

void Graph::outputSummaryInCsv(QString csvFilePath)
{
    QFile file(csvFilePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) qWarning() << "cannot open file: " << csvFilePath;
    QTextStream out(&file);

    out << vertices.size() << "," << edges.size() << "," << routes.size() << ",";
    out << aveEdgesLoad          << "," << medEdgesLoad          << "," << minEdgesLoad          << "," << maxEdgesLoad         << ",";
    out << aveEdgesByte          << "," << medEdgesByte          << "," << minEdgesByte          << "," << maxEdgesByte         << ",";
    out << aveLengthOfRoutes     << "," << medLengthOfRoutes     << "," << minLengthOfRoutes     << "," << maxLengthOfRoutes    << ",";
    out << aveRoutesByte         << "," << medRoutesByte         << "," << minRoutesByte         << "," << maxRoutesByte        << ",";
    out << aveRoutesHopbyte      << "," << medRoutesHopbyte      << "," << minRoutesHopbyte      << "," << maxRoutesHopbyte     << ",";
    out << aveInDegOfEdges       << "," << medInDegOfEdges       << "," << minInDegOfEdges       << "," << maxInDegOfEdges      << ",";
    out << aveInDegOfEdgesLoad   << "," << medInDegOfEdgesLoad   << "," << minInDegOfEdgesLoad   << "," << maxInDegOfEdgesLoad  << ",";
    out << aveInDegOfEdgesByte   << "," << medInDegOfEdgesByte   << "," << minInDegOfEdgesByte   << "," << maxInDegOfEdgesByte  << ",";
    out << aveOutDegOfEdges      << "," << medOutDegOfEdges      << "," << minOutDegOfEdges      << "," << maxOutDegOfEdges     << ",";
    out << aveOutDegOfEdgesLoad  << "," << medOutDegOfEdgesLoad  << "," << minOutDegOfEdgesLoad  << "," << maxOutDegOfEdgesLoad << ",";
    out << aveOutDegOfEdgesByte  << "," << medOutDegOfEdgesByte  << "," << minOutDegOfEdgesByte  << "," << maxOutDegOfEdgesByte << ",";
    out << aveInDegOfRoutes      << "," << medInDegOfRoutes      << "," << minInDegOfRoutes      << "," << maxInDegOfRoutes     << ",";
    out << aveInDegOfRoutesLoad  << "," << medInDegOfRoutesLoad  << "," << minInDegOfRoutesLoad  << "," << maxInDegOfRoutesLoad << ",";
    out << aveOutDegOfRoutes     << "," << medOutDegOfRoutes     << "," << minOutDegOfRoutes     << "," << maxOutDegOfRoutes    << ",";
    out << aveOutDegOfRoutesLoad << "," << medOutDegOfRoutesLoad << "," << minOutDegOfRoutesLoad << "," << maxOutDegOfRoutesLoad;
    out << "\n";
}

void Graph::outputMappingFile(QString filePath, QString outputOrderFilePath)
{
    QVector<int> outputOrder;
    if (outputOrderFilePath != "") {
        QFile orderFile(outputOrderFilePath);
        if (orderFile.open(QIODevice::ReadOnly)) {
            QTextStream ts(&orderFile);

            while (!ts.atEnd()) {
                QString line = ts.readLine();
                outputOrder.push_back(line.toInt());
            }

            orderFile.close();
        } else {
            qWarning() << "Couldn't open output order file. file path: " << outputOrderFilePath;
        }
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) qWarning() << "cannot open file: " << filePath;
    QTextStream out(&file);

    // to be safe, store every ranks coord without T (maybe not necessary)
    int numDims = vertices[0].getCoord().size() + 1; // added T dimension
    int numCores = vertices[0].getRanks().size();
    int numRanks = vertices.size() * numCores;
    QVector<QVector<int> > rankCoords(numRanks, QVector<int>(numDims, 0));

    foreach(Vertex v, vertices) {
        QVector<int> coordWithoutT = v.getCoord();
        QVector<int> ranks = v.getRanks();

        for (int i = 0; i < ranks.size(); ++i) {
            QVector<int> coord = coordWithoutT;
            coord.push_back(i);

            rankCoords[ranks.at(i)] = coord;
        }
    }
    for (int i = 0; i < rankCoords.size(); ++i) {
        QVector<int> coord;
        if (outputOrder.size() == 0) coord = rankCoords.at(i);
        else coord = rankCoords.at(outputOrder.at(i));
        for (int j = 0; j < coord.size(); ++j) {
            out << coord.at(j);
            if (j != coord.size() - 1) out << " ";
        }
        out << "\n";
    }

}

void Graph::outputVerticesSimilarityInCsv(QString csvFilePath)
{
    QFile file(csvFilePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) qWarning() << "cannot open file: " << csvFilePath;
    QTextStream out(&file);

    // Similarity based on Jaccard Coeff
    QVector<QVector<int> > verticesRoutesMat = QVector<QVector<int> >(vertices.size(), QVector<int>(routes.size(), 0));
    for (int i = 0; i < routes.size(); ++i) {
        int rtIndex = routes[i].getIndex();
        QVector<int> arcs = routes[i].getArcs();
        foreach(int vIndex, arcs) {
            verticesRoutesMat[vIndex][rtIndex] = 1;
        }
    }

    for (int i = 0; i < verticesRoutesMat.size(); ++i) {
        for (int j = i + 1; j < verticesRoutesMat.size(); ++j) {
            double sim = jaccardSim(verticesRoutesMat.at(i), verticesRoutesMat.at(j));
            if (sim > 0.0) {
                out << i << "," << j << "," << sim << "\n";
            }
        }
    }

//    // Similarity based on length of route
//    foreach(Route rt, routes) {
//        if (rt.getLength() > aveLengthOfRoutes) {
//            out << rt.getSrc() << "," << rt.getDest() << "," << rt.getLength() << "\n";
//        }
//    }
}

void Graph::outputRoutesSimilarityInCsv(QString csvFilePath)
{
    QFile file(csvFilePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) qWarning() << "cannot open file: " << csvFilePath;
    QTextStream out(&file);

    // Similarity based on Jaccard Coeff
    QVector<QVector<int> > routesVerticesMat = QVector<QVector<int> >(routes.size(), QVector<int>(vertices.size(), 0));
    for (int i = 0; i < routes.size(); ++i) {
        int rtIndex = routes[i].getIndex();
        QVector<int> arcs = routes[i].getArcs();
        foreach(int vIndex, arcs) {
            routesVerticesMat[rtIndex][vIndex] = 1;
        }
    }

    for (int i = 0; i < routesVerticesMat.size(); ++i) {
        for (int j = i + 1; j < routesVerticesMat.size(); ++j) {
            double sim = jaccardSim(routesVerticesMat.at(i), routesVerticesMat.at(j));
            if (sim > 0.0) {
                out << i << "," << j << "," << sim << "\n";
            }
        }
    }
}

bool Graph::loadLessThan(Edge edge1, Edge edge2) { return edge1.getLoad() < edge2.getLoad(); }
bool Graph::edgeByteLessThan(Edge edge1, Edge edge2) { return edge1.getByte() < edge2.getByte(); }
bool Graph::lengthLessThan(Route route1, Route route2){ return route1.getLength() < route2.getLength(); }
bool Graph::maxEdgeLoadLessThan(Route route1, Route route2){ return route1.getMaxEdgeLoad() < route2.getMaxEdgeLoad(); }
bool Graph::routeByteLessThan(Route route1, Route route2) { return route1.getByte() < route2.getByte(); }
bool Graph::hopbyteLessThan(Route route1, Route route2) { return route1.getHopbyte() < route2.getHopbyte(); }
bool Graph::inDegOfEdgesLessThan(Vertex vertex1, Vertex vertex2) { return vertex1.getInDegOfEdges() < vertex2.getInDegOfEdges(); }
bool Graph::inDegOfEdgesLoadLessThan(Vertex vertex1, Vertex vertex2) { return vertex1.getInDegOfEdgesLoad() < vertex2.getInDegOfEdgesLoad(); }
bool Graph::inDegOfEdgesByteLessThan(Vertex vertex1, Vertex vertex2) { return vertex1.getInDegOfEdgesByte() < vertex2.getInDegOfEdgesByte(); }
bool Graph::inDegOfRoutesLessThan(Vertex vertex1, Vertex vertex2) { return vertex1.getInDegOfRoutes() < vertex2.getInDegOfRoutes(); }
bool Graph::inDegOfRoutesLoadLessThan(Vertex vertex1, Vertex vertex2) { return vertex1.getInDegOfRoutesLoad() < vertex2.getInDegOfRoutesLoad(); }
bool Graph::outDegOfEdgesLessThan(Vertex vertex1, Vertex vertex2) { return vertex1.getOutDegOfEdges() < vertex2.getOutDegOfEdges(); }
bool Graph::outDegOfEdgesLoadLessThan(Vertex vertex1, Vertex vertex2) { return vertex1.getOutDegOfEdgesLoad() < vertex2.getOutDegOfEdgesLoad(); }
bool Graph::outDegOfEdgesByteLessThan(Vertex vertex1, Vertex vertex2) { return vertex1.getOutDegOfEdgesByte() < vertex2.getOutDegOfEdgesByte(); }
bool Graph::outDegOfRoutesLessThan(Vertex vertex1, Vertex vertex2) { return vertex1.getOutDegOfRoutes() < vertex2.getOutDegOfRoutes(); }
bool Graph::outDegOfRoutesLoadLessThan(Vertex vertex1, Vertex vertex2) { return vertex1.getOutDegOfRoutesLoad() < vertex2.getOutDegOfRoutesLoad(); }
bool Graph::inMinusOutDegOfEdgesLoadLessThan(Vertex vertex1, Vertex vertex2) { return vertex1.getInMinusOutDegOfEdgesLoad() < vertex2.getInMinusOutDegOfEdgesLoad(); }
bool Graph::outMinusInDegOfEdgesLoadLessThan(Vertex vertex1, Vertex vertex2) { return vertex1.getOutMinusInDegOfEdgesLoad() < vertex2.getOutMinusInDegOfEdgesLoad(); }
bool Graph::inMinusOutDegOfEdgesByteLessThan(Vertex vertex1, Vertex vertex2) { return vertex1.getInMinusOutDegOfEdgesByte() < vertex2.getInMinusOutDegOfEdgesByte(); }
bool Graph::outMinusInDegOfEdgesByteLessThan(Vertex vertex1, Vertex vertex2) { return vertex1.getOutMinusInDegOfEdgesByte() < vertex2.getOutMinusInDegOfEdgesByte(); }

void Graph::reorderEdgesByLoad()
{
    qSort(edges.begin(), edges.end(), loadLessThan);

    // reallocate indices
    for (int i = 0; i < edges.size(); ++i) {
        edges[i].setIndex(i);
    }
}

void Graph::reorderEdgesByByte()
{
    qSort(edges.begin(), edges.end(), edgeByteLessThan);

    // reallocate indices
    for (int i = 0; i < edges.size(); ++i) {
        edges[i].setIndex(i);
    }
}

void Graph::reorderRoutesByLength()
{
    qSort(routes.begin(), routes.end(), lengthLessThan);

    // reallocate indices
    for (int i = 0; i < routes.size(); ++i) {
        routes[i].setIndex(i);
    }
}

void Graph::reorderRoutesByByte()
{
    qSort(routes.begin(), routes.end(), routeByteLessThan);

    // reallocate indices
    for (int i = 0; i < routes.size(); ++i) {
        routes[i].setIndex(i);
    }
}

void Graph::reorderRoutesByHopByte()
{
    qSort(routes.begin(), routes.end(), hopbyteLessThan);

    // reallocate indices
    for (int i = 0; i < routes.size(); ++i) {
        routes[i].setIndex(i);
    }
}

void Graph::reorderRoutesByMetric(QString metric)
{
    if (metric == "length") reorderRoutesByLength();
    else if (metric == "byte") reorderRoutesByByte();
    else if (metric == "hopbyte") reorderRoutesByHopByte();
    else qWarning() << "undefined metric: see reorderRoutesbyMetric in Graph class";
}

QVector<Vertex> Graph::genSortedVerticesByInDegOfEdges()
{
    QVector<Vertex> cpVertices = vertices;
    qSort(cpVertices.begin(), cpVertices.end(), inDegOfEdgesLessThan);

    return cpVertices;
}

QVector<Vertex> Graph::genSortedVerticesByInDegOfEdgesLoad()
{
    QVector<Vertex> cpVertices = vertices;
    qSort(cpVertices.begin(), cpVertices.end(), inDegOfEdgesLoadLessThan);

    return cpVertices;
}

QVector<Vertex> Graph::genSortedVerticesByInDegOfEdgesByte()
{
    QVector<Vertex> cpVertices = vertices;
    qSort(cpVertices.begin(), cpVertices.end(), inDegOfEdgesByteLessThan);

    return cpVertices;
}

QVector<Vertex> Graph::genSortedVerticesByOutDegOfEdges()
{
    QVector<Vertex> cpVertices = vertices;
    qSort(cpVertices.begin(), cpVertices.end(), outDegOfEdgesLessThan);

    return cpVertices;
}

QVector<Vertex> Graph::genSortedVerticesByOutDegOfEdgesLoad()
{
    QVector<Vertex> cpVertices = vertices;
    qSort(cpVertices.begin(), cpVertices.end(), outDegOfEdgesLoadLessThan);

    return cpVertices;
}

QVector<Vertex> Graph::genSortedVerticesByOutDegOfEdgesByte()
{
    QVector<Vertex> cpVertices = vertices;
    qSort(cpVertices.begin(), cpVertices.end(), outDegOfEdgesByteLessThan);

    return cpVertices;
}

QVector<Vertex> Graph::genSortedVerticesByInDegOfRoutes()
{
    QVector<Vertex> cpVertices = vertices;
    qSort(cpVertices.begin(), cpVertices.end(), inDegOfRoutesLessThan);

    return cpVertices;
}

QVector<Vertex> Graph::genSortedVerticesByInDegOfRoutesLoad()
{
    QVector<Vertex> cpVertices = vertices;
    qSort(cpVertices.begin(), cpVertices.end(), inDegOfRoutesLoadLessThan);

    return cpVertices;
}

QVector<Vertex> Graph::genSortedVerticesByOutDegOfRoutes()
{
    QVector<Vertex> cpVertices = vertices;
    qSort(cpVertices.begin(), cpVertices.end(), outDegOfRoutesLessThan);

    return cpVertices;
}

QVector<Vertex> Graph::genSortedVerticesByOutDegOfRoutesLoad()
{
    QVector<Vertex> cpVertices = vertices;
    qSort(cpVertices.begin(), cpVertices.end(), outDegOfRoutesLoadLessThan);

    return cpVertices;
}

QVector<Vertex> Graph::genSortedVerticesByInMinusOutDegOfEdgesLoad()
{
    QVector<Vertex> cpVertices = vertices;
    qSort(cpVertices.begin(), cpVertices.end(), inMinusOutDegOfEdgesLoadLessThan);

    return cpVertices;
}

QVector<Vertex> Graph::genSortedVerticesByInMinusOutDegOfEdgesByte()
{
    QVector<Vertex> cpVertices = vertices;
    qSort(cpVertices.begin(), cpVertices.end(), inMinusOutDegOfEdgesByteLessThan);

    return cpVertices;
}

QVector<Vertex> Graph::genSortedVerticesByOutMinusInDegOfEdgesLoad()
{
    QVector<Vertex> cpVertices = vertices;
    qSort(cpVertices.begin(), cpVertices.end(), outMinusInDegOfEdgesLoadLessThan);

    return cpVertices;
}

QVector<Vertex> Graph::genSortedVerticesByOutMinusInDegOfEdgesByte()
{
    QVector<Vertex> cpVertices = vertices;
    qSort(cpVertices.begin(), cpVertices.end(), outMinusInDegOfEdgesByteLessThan);

    return cpVertices;
}

QVector<Edge> Graph::genSortedEdgesByLoad()
{
    QVector<Edge> cpEdges = edges;
    qSort(cpEdges.begin(), cpEdges.end(), loadLessThan);

    return cpEdges;
}

QVector<Edge> Graph::genSortedEdgesByByte()
{
    QVector<Edge> cpEdges = edges;
    qSort(cpEdges.begin(), cpEdges.end(), edgeByteLessThan);

    return cpEdges;
}

QVector<Route> Graph::genSortedRoutesByLength()
{
    QVector<Route> cpRoutes = routes;
    qSort(cpRoutes.begin(), cpRoutes.end(), lengthLessThan);

    return cpRoutes;
}

QVector<Route> Graph::genSortedRoutesByByte()
{
    QVector<Route> cpRoutes = routes;
    qSort(cpRoutes.begin(), cpRoutes.end(), routeByteLessThan);

    return cpRoutes;
}

void Graph::sortRoutesByLength(QVector<Route> &routes)
{
    qSort(routes.begin(), routes.end(), lengthLessThan);
}

void Graph::sortRoutesByByte(QVector<Route> &routes)
{
    qSort(routes.begin(), routes.end(), routeByteLessThan);
}

void Graph::sortRoutesByHopbyte(QVector<Route> &routes)
{
    qSort(routes.begin(), routes.end(), hopbyteLessThan);
}


void Graph::sortRoutesByMetric(QVector<Route> &routes, QString metric)
{
    if (metric == "length") sortRoutesByLength(routes);
    else if (metric == "byte") sortRoutesByByte(routes);
    else if (metric == "hopbyte") sortRoutesByHopbyte(routes);
    else qWarning() << "undefined metric: see sortRoutesbyMetric in Graph class";
}

QVector<Route> Graph::genSortedRoutesByHopbyte()
{
    QVector<Route> cpRoutes = routes;
    qSort(cpRoutes.begin(), cpRoutes.end(), hopbyteLessThan);

    return cpRoutes;
}

QVector<Route>  Graph::genSortedRoutesByMetric(QString metric)
{
    QVector<Route> cpRoutes = routes;
    sortRoutesByMetric(cpRoutes, metric);

    return cpRoutes;
}

void Graph::replaceRoutes(QVector<Route> altRoutes)
{
    // replace the same index route with alt route
    foreach(Route altRoute, altRoutes) {
        int rtIndex = altRoute.getIndex();
        // reduce load of original route for every edge in the original route
        Route originalRoute = routes[rtIndex];
        QVector<int> originalArcs = originalRoute.getArcs();
        for (int i = 0; i < originalArcs.size() - 1; ++i) {
            int eIndex = sourceTargetToEdgeIndex[qMakePair(originalArcs[i], originalArcs[i + 1])];
            edges[eIndex].setLoad(edges[eIndex].getLoad() - originalRoute.getLoad());
        }

        // plus load of alt route for every edge in the alt route
        QVector<int> altArcs = altRoute.getArcs();
        for (int i = 0; i < altArcs.size() - 1; ++i) {
            int eIndex = sourceTargetToEdgeIndex[qMakePair(altArcs[i], altArcs[i + 1])];
            edges[eIndex].setLoad(edges[eIndex].getLoad() + altRoute.getLoad());
        }

        // replacte route
        routes[rtIndex] = altRoute;
    }

    // update graph info
    int numOfNodes = vertices.size();
    QVector<int> vertexInDegOfEdges( QVector <int>(numOfNodes, 0) );
    QVector<int> vertexOutDegOfEdges( QVector <int>(numOfNodes, 0) );
    QVector<int> vertexInDegOfRoutes( QVector <int>(numOfNodes, 0) );
    QVector<int> vertexOutDegOfRoutes( QVector <int>(numOfNodes, 0) );
    QVector<int> vertexInDegOfEdgesLoad( QVector <int>(numOfNodes, 0) );
    QVector<int> vertexOutDegOfEdgesLoad( QVector <int>(numOfNodes, 0) );
    QVector<int> vertexInDegOfRoutesLoad( QVector <int>(numOfNodes, 0) );
    QVector<int> vertexOutDegOfRoutesLoad( QVector <int>(numOfNodes, 0) );
    // edges
    foreach(Edge e, edges) {
        int source = e.getSource();
        int target = e.getTarget();
        int load = e.getLoad();
        vertexOutDegOfEdges[source] += 1;
        vertexInDegOfEdges[target] += 1;
        vertexOutDegOfEdgesLoad[source] += load;
        vertexInDegOfEdgesLoad[target] += load;
    }
    reorderEdgesByByte();
    // routes
    foreach(Route rt, routes) {
        int src = rt.getSrc();
        int dest = rt.getDest();
        int load = rt.getLoad();
        vertexOutDegOfRoutes[src] += 1;
        vertexInDegOfRoutes[dest] += 1;
        vertexOutDegOfRoutesLoad[src] += load;
        vertexInDegOfRoutesLoad[dest] += load;
    }
    reorderRoutesByByte();
    // vertices
    foreach(Vertex v, vertices) {
        int vIndex = v.getIndex();
        v.setInDegOfEdges(vertexInDegOfEdges[vIndex]);
        v.setInDegOfEdgesLoad(vertexInDegOfEdgesLoad[vIndex]);
        v.setOutDegOfEdges(vertexOutDegOfEdges[vIndex]);
        v.setOutDegOfEdgesLoad(vertexOutDegOfEdgesLoad[vIndex]);
        v.setInDegOfRoutes(vertexInDegOfRoutes[vIndex]);
        v.setInDegOfRoutesLoad(vertexInDegOfRoutesLoad[vIndex]);
        v.setOutDegOfRoutes(vertexOutDegOfRoutes[vIndex]);
        v.setOutDegOfRoutesLoad(vertexOutDegOfRoutesLoad[vIndex]);
    }
    // set average degree, etc
    calcAndSetStatisticalInfo();
    genAndSetSourceTargetToEdgeIndex();
    genAndSetCoordToVertexIndex();
    genAndSetMaxEdgeLoadOfRoutes();
    genAndSetMaxEdgeByteOfRoutes();

    summary();
}

void Graph::updateGraphInfo()
{

    // update graph info
    int numOfNodes = vertices.size();
    QVector<int> vertexInDegOfEdges( QVector <int>(numOfNodes, 0) );
    QVector<int> vertexOutDegOfEdges( QVector <int>(numOfNodes, 0) );
    QVector<int> vertexInDegOfRoutes( QVector <int>(numOfNodes, 0) );
    QVector<int> vertexOutDegOfRoutes( QVector <int>(numOfNodes, 0) );
    QVector<int> vertexInDegOfEdgesLoad( QVector <int>(numOfNodes, 0) );
    QVector<int> vertexOutDegOfEdgesLoad( QVector <int>(numOfNodes, 0) );
    QVector<float> vertexInDegOfEdgesByte( QVector <float>(numOfNodes, 0.0) );
    QVector<float> vertexOutDegOfEdgesByte( QVector <float>(numOfNodes, 0.0) );
    QVector<int> vertexInDegOfRoutesLoad( QVector <int>(numOfNodes, 0) );
    QVector<int> vertexOutDegOfRoutesLoad( QVector <int>(numOfNodes, 0) );

    // edges
    foreach(Edge e, edges) {
        int source = e.getSource();
        int target = e.getTarget();
        int load = e.getLoad();
        float byte = e.getByte();
        vertexOutDegOfEdges[source] += 1;
        vertexInDegOfEdges[target] += 1;
        vertexOutDegOfEdgesLoad[source] += load;
        vertexInDegOfEdgesLoad[target] += load;
        vertexOutDegOfEdgesByte[source] += byte;
        vertexInDegOfEdgesByte[target] += byte;
    }
    reorderEdgesByByte();

    // routes
    foreach(Route rt, routes) {
        int src = rt.getSrc();
        int dest = rt.getDest();
        int load = rt.getLoad();
        vertexOutDegOfRoutes[src] += 1;
        vertexInDegOfRoutes[dest] += 1;
        vertexOutDegOfRoutesLoad[src] += load;
        vertexInDegOfRoutesLoad[dest] += load;
    }
    reorderRoutesByByte();

    // vertices
    foreach(Vertex v, vertices) {
        int vIndex = v.getIndex();
        v.setInDegOfEdges(vertexInDegOfEdges[vIndex]);
        v.setInDegOfEdgesLoad(vertexInDegOfEdgesLoad[vIndex]);
        v.setInDegOfEdgesByte(vertexInDegOfEdgesByte[vIndex]);
        v.setOutDegOfEdges(vertexOutDegOfEdges[vIndex]);
        v.setOutDegOfEdgesLoad(vertexOutDegOfEdgesLoad[vIndex]);
        v.setOutDegOfEdgesByte(vertexOutDegOfEdgesByte[vIndex]);
        v.setInDegOfRoutes(vertexInDegOfRoutes[vIndex]);
        v.setInDegOfRoutesLoad(vertexInDegOfRoutesLoad[vIndex]);
        v.setOutDegOfRoutes(vertexOutDegOfRoutes[vIndex]);
        v.setOutDegOfRoutesLoad(vertexOutDegOfRoutesLoad[vIndex]);
    }

    // set average degree, etc
    calcAndSetStatisticalInfo();
    genAndSetSourceTargetToEdgeIndex();
    genAndSetCoordToVertexIndex();
    genAndSetMaxEdgeLoadOfRoutes();
    genAndSetMaxEdgeByteOfRoutes();

    summary();
}

void Graph::setVertices(QVector<Vertex> vertices) { this->vertices = vertices; }
void Graph::setVertexAt(int pos, Vertex vertex) { this->vertices[pos] = vertex; }
void Graph::setEdges(QVector<Edge> edges) { this->edges = edges; }
void Graph::setEdgeAt(int pos, Edge edge) { this->edges[pos] = edge; }
void Graph::setEdgeLoadAt(int pos, int load) { this->edges[pos].setLoad(load); }
void Graph::setEdgeByteAt(int pos, float byte) { this->edges[pos].setByte(byte); }
void Graph::incEdgeLoadAt(int pos, int inc) { this->edges[pos].incLoad(inc); }
void Graph::incEdgeByteAt(int pos, float inc) { this->edges[pos].incByte(inc); }
void Graph::addEdge(Edge edge) { edges.push_back(edge); }

void Graph::updateEdgeLoadBasedOnRoutes()
{
    // clear loads
    for (int i = 0; i < edges.size(); ++i) {
        edges[i].setLoad(0);
    }

    foreach(Route rt, routes) {
        QVector<int> arcs = rt.getArcs();
        for (int i = 0; i < arcs.size() - 1; ++i) {
            int source = arcs.at(i);
            int target = arcs.at(i + 1);
            int eIndex = getEdgeIndexWithSourceTarget(source, target);
            if (eIndex < 0) { // unused edge at this moment
                edges.push_back(Edge(edges.size(), source, target, rt.getLoad()));
            } else {
                edges[eIndex].incLoad(rt.getLoad());
            }
        }
    }
}

void Graph::outputRouteListWithTCoordFromRoutesFile(QString routesFilePath, QString outputFilePath, int numOfCores, QVector<int> shape)
{
    int numOfRanks = numOfCores;
    foreach(int i, shape) {
        numOfRanks *= i;
    }

    QFile routesFile(routesFilePath);

    QVector< QVector< QVector<int> > > routeArcs( QVector< QVector< QVector<int> > > (numOfRanks, QVector< QVector<int> >(numOfRanks, QVector<int>())));

    if (routesFile.open(QIODevice::ReadOnly)) {
        QTextStream ts(&routesFile);
        while (!ts.atEnd()) {
            QString line = ts.readLine();
            QStringList elements = parseRouteLine(line);
            if(elements.size() < 6) {
                qWarning() << "this line doesn't have enough info: " << line;
            }
            int hopNum = elements.at(0).toInt();
            int src = elements.at(1).toInt();
            int dest = elements.at(2).toInt();
            int source = elements.at(3).toInt();

            QVector<int> sourceCoord;
            foreach (const QString &str, elements.at(4).split(" ")) {
                sourceCoord.push_back(str.toInt());
            }
            int target = elements.at(5).toInt();
            QVector<int> targetCoord;
            foreach (const QString &str, elements.at(6).split(" ")) {
                targetCoord.push_back(str.toInt());
            }

            if(hopNum == 1) {
                routeArcs[src][dest].push_back(source);
            }
            routeArcs[src][dest].push_back(target);
        }
        routesFile.close();
    }

    QFile outFile(outputFilePath);
    if (!outFile.open(QIODevice::WriteOnly | QIODevice::Text)) qWarning() << "cannot open file: " << outputFilePath;
    QTextStream out(&outFile);

    for (int i = 0; i < routeArcs.size(); ++i) {
        for (int j = 0; j < routeArcs.at(i).size(); ++j) {
            QVector<int> arcs = routeArcs.at(i).at(j);
            if (arcs.size() > 0) {
                out << i << "," << j << ",";
                for (int k = 0 ; k < arcs.size(); ++k) {
                    int a = arcs.at(k);
                    if (k == 0) out << a;
                    else out << " " << a;
                }
                out << "\n";
            }
        }
    }
}

void Graph::setRoutes(QVector<Route> routes) { this->routes = routes; }
void Graph::setRouteAt(int pos, Route route) { this->routes[pos] = route; }
void Graph::setLayout(QString layout) { this->layout = layout; }
void Graph::setData(QString data) { this->data = data; }
void Graph::setCoordToVertexIndex(QMap<QVector<int>, int> coordToVertexIndex) { this->coordToVertexIndex = coordToVertexIndex; }

QVector<Vertex> Graph::getVertices() { return vertices; }
QVector<Edge> Graph::getEdges() { return edges; }
QVector<Route> Graph::getRoutes() { return routes; }
int Graph::getRouteLengthAt(int index) { return routes[index].getLength(); }
int Graph::getNumEdges() { return edges.size(); }
int Graph::getNumRoutes() { return routes.size(); }
int Graph::getNumVertices() { return vertices.size(); }
QString Graph::getLayout() { return layout; }
QString Graph::getData() { return data; }
QMap<QVector<int>, int> Graph::getCoordToVertexIndex() { return coordToVertexIndex; }

Vertex Graph::getVertexAt(int index) { return vertices[index]; }
Vertex Graph::getVertexWithCoord(QVector<int> coord) { return vertices[coordToVertexIndex[coord]]; }
int Graph::getVertexIndexWithCoord(QVector<int> coord) { return coordToVertexIndex[coord]; }
Edge Graph::getEdgeAt(int index) { return edges[index]; }
Edge Graph::getEdgeWithSourceTarget(int source, int target)
{
    QMap< QPair<int, int>, int>::const_iterator it = sourceTargetToEdgeIndex.find(qMakePair(source, target)); // if not exists, this returns end() value
    if (it == sourceTargetToEdgeIndex.end()) {
        return Edge(-1, source, target, 0);
    }

    return edges[it.value()];
}
int Graph::getEdgeIndexWithSourceTarget(int source, int target) {
    QMap< QPair<int, int>, int>::const_iterator it = sourceTargetToEdgeIndex.find(qMakePair(source, target)); // if not exists, this returns end() value
    if (it == sourceTargetToEdgeIndex.end()) return -1;
    return it.value();
}
Route Graph::getRouteAt(int index) { return routes[index]; }
QVector<Route> Graph::getRoutesContainingSourceTarget(int source, int target)
{
    QVector<Route> result;
    // TODO: think faster way later
    foreach(Route r, routes) {
        for (int i = 0; i < r.getArcs().size() - 1; ++i) {
            if (r.getArcs().at(i) == source && r.getArcs().at(i + 1) == target) {
                result.push_back(r);
                break;
            }
        }
    }
    return result;
}

double Graph::getAveInDegOfEdges() { return aveInDegOfEdges; }
int    Graph::getMinInDegOfEdges() { return minInDegOfEdges; }
int    Graph::getMaxInDegOfEdges() { return maxInDegOfEdges; }
double Graph::getAveInDegOfEdgesLoad() { return aveInDegOfEdgesLoad; }
int    Graph::getMinInDegOfEdgesLoad() { return minInDegOfEdgesLoad; }
int    Graph::getMaxInDegOfEdgesLoad() { return maxInDegOfEdgesLoad; }
double Graph::getAveInDegOfEdgesByte() { return aveInDegOfEdgesByte; }
float  Graph::getMinInDegOfEdgesByte() { return minInDegOfEdgesByte; }
float  Graph::getMaxInDegOfEdgesByte() { return maxInDegOfEdgesByte; }
double Graph::getAveOutDegOfEdges() { return aveOutDegOfEdges; }
int    Graph::getMinOutDegOfEdges() { return minOutDegOfEdges; }
int    Graph::getMaxOutDegOfEdges() { return maxOutDegOfEdges; }
double Graph::getAveOutDegOfEdgesLoad() { return aveOutDegOfEdgesLoad; }
int    Graph::getMinOutDegOfEdgesLoad() { return minOutDegOfEdgesLoad; }
int    Graph::getMaxOutDegOfEdgesLoad() { return maxOutDegOfEdgesLoad; }
double Graph::getAveOutDegOfEdgesByte() { return aveOutDegOfEdgesByte; }
float  Graph::getMinOutDegOfEdgesByte() { return minOutDegOfEdgesByte; }
float  Graph::getMaxOutDegOfEdgesByte() { return maxOutDegOfEdgesByte; }
double Graph::getAveInDegOfRoutes() { return aveInDegOfRoutes; }
int    Graph::getMinInDegOfRoutes() { return minInDegOfRoutes; }
int    Graph::getMaxInDegOfRoutes() { return maxInDegOfRoutes; }
double Graph::getAveInDegOfRoutesLoad() { return aveInDegOfRoutesLoad; }
int    Graph::getMinInDegOfRoutesLoad() { return minInDegOfRoutesLoad; }
int    Graph::getMaxInDegOfRoutesLoad() { return maxInDegOfRoutesLoad; }
double Graph::getAveOutDegOfRoutes() { return aveOutDegOfRoutes; }
int    Graph::getMinOutDegOfRoutes() { return minOutDegOfRoutes; }
int    Graph::getMaxOutDegOfRoutes() { return maxOutDegOfRoutes; }
double Graph::getAveOutDegOfRoutesLoad() { return aveOutDegOfRoutesLoad; }
int    Graph::getMinOutDegOfRoutesLoad() { return minOutDegOfRoutesLoad; }
int    Graph::getMaxOutDegOfRoutesLoad() { return maxOutDegOfRoutesLoad; }
double Graph::getAveEdgesLoad() { return aveEdgesLoad; }
int    Graph::getMinEdgesLoad() { return minEdgesLoad; }
int    Graph::getMaxEdgesLoad() { return maxEdgesLoad; }
double Graph::getAveEdgesByte() { return aveEdgesByte; }
float  Graph::getMinEdgesByte() { return minEdgesByte; }
float  Graph::getMaxEdgesByte() { return maxEdgesByte; }
double Graph::getAveLengthOfRoutes() { return aveLengthOfRoutes; }
int    Graph::getMinLengthOfRoutes() { return minLengthOfRoutes; }
int    Graph::getMaxLengthOfRoutes() { return maxLengthOfRoutes; }
double Graph::getAveRoutesByte() { return aveRoutesByte; }
float  Graph::getMinRoutesByte() { return minRoutesByte; }
float  Graph::getMaxRoutesByte() { return maxRoutesByte; }
double Graph::getAveRoutesHopbyte() { return aveRoutesHopbyte; }
float  Graph::getMinRoutesHopbyte() { return minRoutesHopbyte; }
float  Graph::getMaxRoutesHopbyte() { return maxRoutesHopbyte; }

float Graph::getMetric(QString elemType, QString metric, QString valueType)
{
    bool defined = true;
    float result = 0.0;

    if (elemType == "edge") {
        if (metric == "load") {
            if (valueType == "ave") result = (float)getAveEdgesLoad();
            else if (valueType == "min") result = (float)getMinEdgesLoad();
            else if (valueType == "max") result = (float)getMaxEdgesLoad();
            else defined = false;
        } else if (metric == "byte") {
            if (valueType == "ave") result = (float)getAveEdgesByte();
            else if (valueType == "min") result = getMinEdgesByte();
            else if (valueType == "max") result = getMaxEdgesByte();
            else defined = false;
        } else {
            defined = false;
        }
    } else if (elemType == "route") {
        if (metric == "length") {
            if (valueType == "ave") result = (float)getAveLengthOfRoutes();
            else if (valueType == "min") result = (float)getMinLengthOfRoutes();
            else if (valueType == "max") result = (float)getMaxLengthOfRoutes();
            else defined = false;
        } else if (metric == "byte") {
            if (valueType == "ave") result = (float)getAveRoutesByte();
            else if (valueType == "min") result = getMinRoutesByte();
            else if (valueType == "max") result = getMaxRoutesByte();
            else defined = false;
        } else if (metric == "hopbyte") {
            if (valueType == "ave") result = (float)getAveRoutesHopbyte();
            else if (valueType == "min") result = getMinRoutesHopbyte();
            else if (valueType == "max") result = getMaxRoutesHopbyte();
            else defined = false;
        } else {
            defined = false;
        }
    } else {
        defined = false;
    }

    if (!defined) qWarning() << "undefined: check getMetric in Graph class";

    return result;
}
