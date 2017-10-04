#ifndef GRAPH_H
#define GRAPH_H

#include <QVector>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDebug>
#include <algorithm>

#include "Vertex.h"
#include "Edge.h"
#include "Route.h"

using namespace std;

class Graph
{
public:
    Graph(QVector<Vertex> vertices = QVector<Vertex>(), QVector<Edge> edges = QVector<Edge>(), QVector<Route> routes = QVector<Route>());

    bool loadFromRoutesFile(const QString &rtFilePath, const QString &hopbyteFilePath, int numOfCores, QVector<int> shape, QVector<int> order, QString outputDir, QVector<int> aggregatingDims = QVector<int>());
    bool loadFromGraphFiles(const QString &vertexFilePath, const QString &edgeFilePath, const QString &routeFilePath, const QString &summaryFilePath);
    bool loadFromPosFile(const QString &filePath);
    QStringList parseRouteLine(QString line);
    void summary();

    double jaccardSim(QVector<int> v1, QVector<int> v2);
    double calcTotalHobByte();

    static bool loadLessThan(Edge edge1, Edge edge2);
    static bool edgeByteLessThan(Edge edge1, Edge edge2);
    static bool lengthLessThan(Route route1, Route route2);
    static bool maxEdgeLoadLessThan(Route route1, Route route2);
    static bool routeByteLessThan(Route route1, Route route2);
    static bool hopbyteLessThan(Route route1, Route route2);
    static bool inDegOfEdgesLessThan(Vertex vertex1, Vertex vertex2);
    static bool inDegOfEdgesLoadLessThan(Vertex vertex1, Vertex vertex2);
    static bool inDegOfEdgesByteLessThan(Vertex vertex1, Vertex vertex2);
    static bool outDegOfEdgesLessThan(Vertex vertex1, Vertex vertex2);
    static bool outDegOfEdgesLoadLessThan(Vertex vertex1, Vertex vertex2);
    static bool outDegOfEdgesByteLessThan(Vertex vertex1, Vertex vertex2);
    static bool inDegOfRoutesLessThan(Vertex vertex1, Vertex vertex2);
    static bool inDegOfRoutesLoadLessThan(Vertex vertex1, Vertex vertex2);
    static bool outDegOfRoutesLessThan(Vertex vertex1, Vertex vertex2);
    static bool outDegOfRoutesLoadLessThan(Vertex vertex1, Vertex vertex2);
    static bool inMinusOutDegOfEdgesLoadLessThan(Vertex vertex1, Vertex vertex2);
    static bool outMinusInDegOfEdgesLoadLessThan(Vertex vertex1, Vertex vertex2);
    static bool inMinusOutDegOfEdgesByteLessThan(Vertex vertex1, Vertex vertex2);
    static bool outMinusInDegOfEdgesByteLessThan(Vertex vertex1, Vertex vertex2);

    void reorderEdgesByLoad();
    void reorderEdgesByByte();
    void reorderRoutesByLength();
    void reorderRoutesByByte();
    void reorderRoutesByHopByte();
    void reorderRoutesByMetric(QString metric);
//    void reorderAggregatedEdgesByLoad();
//    void reorderAggregatedEdgesByByte();
//    void reorderAggregatedRoutesByLength();
//    void reorderAggregatedRoutesByByte();
//    void reorderAggregatedRoutesByHopByte();
//    void reorderAggregatedRoutesByMetric(QString metric);
    QVector<Vertex> genSortedVerticesByInDegOfEdges();
    QVector<Vertex> genSortedVerticesByInDegOfEdgesLoad();
    QVector<Vertex> genSortedVerticesByInDegOfEdgesByte();
    QVector<Vertex> genSortedVerticesByOutDegOfEdges();
    QVector<Vertex> genSortedVerticesByOutDegOfEdgesLoad();
    QVector<Vertex> genSortedVerticesByOutDegOfEdgesByte();
    QVector<Vertex> genSortedVerticesByInDegOfRoutes();
    QVector<Vertex> genSortedVerticesByInDegOfRoutesLoad();
    QVector<Vertex> genSortedVerticesByOutDegOfRoutes();
    QVector<Vertex> genSortedVerticesByOutDegOfRoutesLoad();
    QVector<Vertex> genSortedVerticesByInMinusOutDegOfEdgesLoad();
    QVector<Vertex> genSortedVerticesByOutMinusInDegOfEdgesLoad();
    QVector<Vertex> genSortedVerticesByInMinusOutDegOfEdgesByte();
    QVector<Vertex> genSortedVerticesByOutMinusInDegOfEdgesByte();
    QVector<Edge>  genSortedEdgesByLoad();
    QVector<Edge>  genSortedEdgesByByte();
    QVector<Route>  genSortedRoutesByLength();
    QVector<Route>  genSortedRoutesByByte();
    QVector<Route>  genSortedRoutesByHopbyte();
    QVector<Route>  genSortedRoutesByMetric(QString metric);
    static void sortRoutesByLength(QVector<Route> &routes);
    static void sortRoutesByByte(QVector<Route> &routes);
    static void sortRoutesByHopbyte(QVector<Route> &routes);
    static void sortRoutesByMetric(QVector<Route> &routes, QString metric);

    void replaceRoutes(QVector<Route> altRoutes);

    void setVertices(QVector<Vertex> vertices);
    void setVertexAt(int pos, Vertex vertex);
    void setEdges(QVector<Edge> edges);
    void setEdgeAt(int pos, Edge edge);
    void setEdgeLoadAt(int pos, int load);
    void setEdgeByteAt(int pos, float byte);
    void incEdgeLoadAt(int pos, int inc);
    void incEdgeByteAt(int pos, float inc);
    void addEdge(Edge edge);
    void updateEdgeLoadBasedOnRoutes();
    void setRoutes(QVector<Route> routes);
    void setRouteAt(int pos, Route route);
    void setLayout(QString layout);
    void setData(QString data);
    void setCoordToVertexIndex(QMap< QVector<int>, int> coordToVertexIndex);

    QVector<Vertex> getVertices();
    QVector<Edge> getEdges();
    QVector<Route> getRoutes();
    int getRouteLengthAt(int index);
    int getNumEdges();
    int getNumRoutes();
    int getNumVertices();
    QString getLayout();
    QString getData();
    QMap< QVector<int>, int> getCoordToVertexIndex();

    Vertex getVertexAt(int index);
    Vertex getVertexWithCoord(QVector<int> coord);
    int getVertexIndexWithCoord(QVector<int> coord);
    Edge getEdgeAt(int index);
    Edge getEdgeWithSourceTarget(int source, int target);
    int getEdgeIndexWithSourceTarget(int source, int target);
    Route getRouteAt(int index);
    QVector<Route> getRoutesContainingSourceTarget(int source, int target);
    void replaceOneCoordToVertexIndex(Vertex vertex);
    void updateGraphInfo();

    double getAveInDegOfEdges();
    int    getMinInDegOfEdges();
    int    getMaxInDegOfEdges();
    double getAveInDegOfEdgesLoad();
    int    getMinInDegOfEdgesLoad();
    int    getMaxInDegOfEdgesLoad();
    double getAveInDegOfEdgesByte();
    float  getMinInDegOfEdgesByte();
    float  getMaxInDegOfEdgesByte();
    double getAveOutDegOfEdges();
    int    getMinOutDegOfEdges();
    int    getMaxOutDegOfEdges();
    double getAveOutDegOfEdgesLoad();
    int    getMinOutDegOfEdgesLoad();
    int    getMaxOutDegOfEdgesLoad();
    double getAveOutDegOfEdgesByte();
    float  getMinOutDegOfEdgesByte();
    float  getMaxOutDegOfEdgesByte();
    double getAveInDegOfRoutes();
    int    getMinInDegOfRoutes();
    int    getMaxInDegOfRoutes();
    double getAveInDegOfRoutesLoad();
    int    getMinInDegOfRoutesLoad();
    int    getMaxInDegOfRoutesLoad();
    double getAveOutDegOfRoutes();
    int    getMinOutDegOfRoutes();
    int    getMaxOutDegOfRoutes();
    double getAveOutDegOfRoutesLoad();
    int    getMinOutDegOfRoutesLoad();
    int    getMaxOutDegOfRoutesLoad();
    double getAveEdgesLoad();
    int    getMinEdgesLoad();
    int    getMaxEdgesLoad();
    double getAveEdgesByte();
    float  getMinEdgesByte();
    float  getMaxEdgesByte();
    double getAveLengthOfRoutes();
    int    getMinLengthOfRoutes();
    int    getMaxLengthOfRoutes();
    double getAveRoutesByte();
    float  getMinRoutesByte();
    float  getMaxRoutesByte();
    double getAveRoutesHopbyte();
    float  getMinRoutesHopbyte();
    float  getMaxRoutesHopbyte();
    float  getMetric(QString elemType, QString metric, QString valueType);

    void outputVerticesInCsv(QString csvFilePath);
    void outputEdgesInCsv(QString csvFilePath);
    void outputRoutesInCsv(QString csvFilePath);
    void outputRouteSrcDestLoadInCsv(QString csvFilePath);
    void outputSummaryInCsv(QString csvFilePath);
    void outputMappingFile(QString filePath,  QString outputOrderFilePath = "");
    void outputVerticesSimilarityInCsv(QString csvFilePath);
    void outputRoutesSimilarityInCsv(QString csvFilePath);
    void outputRouteWeightMatInCsv(QString csvFilePath, QString outputOrderFilePath = ""); // just for test now
    void outputRouteListWithTCoordFromRoutesFile(QString routesFilePath, QString outputFilePath, int numOfCores, QVector<int> shape);

private:
    QVector<Vertex> vertices;
    QVector<Edge> edges;
    QVector<Route> routes;
//    QVector<Vertex> aggregatedVertices;
//    QVector<Edge> aggregatedEdges;
//    QVector<Route> aggregatedRoutes;
    QString layout;
    QString data;

    bool allocatingUnusedCoords;
    double aveInDegOfEdges;
    int    medInDegOfEdges;
    int    minInDegOfEdges;
    int    maxInDegOfEdges;
    double aveInDegOfEdgesLoad;
    int    medInDegOfEdgesLoad;
    int    minInDegOfEdgesLoad;
    int    maxInDegOfEdgesLoad;
    double aveInDegOfEdgesByte;
    float  medInDegOfEdgesByte;
    float  minInDegOfEdgesByte;
    float  maxInDegOfEdgesByte;
    double aveOutDegOfEdges;
    int    medOutDegOfEdges;
    int    minOutDegOfEdges;
    int    maxOutDegOfEdges;
    double aveOutDegOfEdgesLoad;
    int    medOutDegOfEdgesLoad;
    int    minOutDegOfEdgesLoad;
    int    maxOutDegOfEdgesLoad;
    double aveOutDegOfEdgesByte;
    float  medOutDegOfEdgesByte;
    float  minOutDegOfEdgesByte;
    float  maxOutDegOfEdgesByte;
    double aveInDegOfRoutes;
    int    medInDegOfRoutes;
    int    minInDegOfRoutes;
    int    maxInDegOfRoutes;
    double aveInDegOfRoutesLoad;
    int    medInDegOfRoutesLoad;
    int    minInDegOfRoutesLoad;
    int    maxInDegOfRoutesLoad;
    double aveOutDegOfRoutes;
    int    medOutDegOfRoutes;
    int    minOutDegOfRoutes;
    int    maxOutDegOfRoutes;
    double aveOutDegOfRoutesLoad;
    int    medOutDegOfRoutesLoad;
    int    minOutDegOfRoutesLoad;
    int    maxOutDegOfRoutesLoad;
    double aveEdgesLoad;
    int    medEdgesLoad;
    int    minEdgesLoad;
    int    maxEdgesLoad;
    double aveEdgesByte;
    float  medEdgesByte;
    float  minEdgesByte;
    float  maxEdgesByte;
    double aveLengthOfRoutes;
    int    medLengthOfRoutes;
    int    minLengthOfRoutes;
    int    maxLengthOfRoutes;
    double aveRoutesByte;
    float  medRoutesByte;
    float  minRoutesByte;
    float  maxRoutesByte;
    double aveRoutesHopbyte;
    float  medRoutesHopbyte;
    float  minRoutesHopbyte;
    float  maxRoutesHopbyte;

    // TODO: think better way later
    QMap< QPair<int, int>, int> sourceTargetToEdgeIndex;
    QMap< QVector<int>, int> coordToVertexIndex;

    void calcAndSetStatisticalInfo();
    void genAndSetSourceTargetToEdgeIndex();
    void genAndSetCoordToVertexIndex();
    void genAndSetMaxEdgeLoadOfRoutes();
    void genAndSetMaxEdgeByteOfRoutes();
    void allocateUnusedCoordsToVertices(QVector<int> shape, QVector<int> order);
};

#endif // GRAPH_H
