#ifndef ROUTEGUIDE_H
#define ROUTEGUIDE_H

#include "Graph.h"
#include "AdjMatrix.h"

// Time
#include <chrono>
#include <ctime>
#include <ratio>

class RouteGuide
{
public:
    RouteGuide();
    RouteGuide(Graph* graph, AdjMatrix *adjMat);

    void initilize(Graph* graph, AdjMatrix *adjMat);
    QVector<Route> findAltRoutes(Route route, QString type = "src-dest-fixed", int lengthTolerance = 0, float loadTolerance = 0.0); // type: src-dest-fixed, src-fixed, non-fixed
    QVector<Route> findAltRoutesForRemapping(Route route, int altSrc, int altDest, int thresLength, int thresMaxLoad, QVector<QVector<int> > &edgeLoadIncInfoForRemapping);
    static bool lengthLessThan(Route route1, Route route2);
    static bool maxEdgeLoadLessThan(Route route1, Route route2);
    static bool lengthLessThanThenMaxEdgeLoadLessThan(Route route1, Route route2);
    static bool maxEdgeLoadLessThanThenLengthLessThan(Route route1, Route route2);
    void reorderAltRoutesByLength(QVector<Route> &routes);
    void reorderAltRoutesByMaxEdgeLoad(QVector<Route> &routes);
    void reorderAltRoutesByLengthThenMaxEdgeLoad(QVector<Route> &routes);
    void reorderAltRoutesByMaxEdgeLoadThenLength(QVector<Route> &routes);
    Route suggestRoute(QString priority, Route route, QString type = "src-dest-fixed", int lengthTolerance = 0, float loadTolerance = 0.0); // priority: lengthFirst, loadCapacityFirst
    Route suggestRouteSrcDestFixed(QString priority, Route route, int lengthTolerance = 0, float loadTolerance = 0.0); // priority: lengthFirst, loadCapacityFirst
    Route suggestRouteSrcFixed(QString priority, Route route, int lengthTolerance = 0, float loadTolerance = 0.0); // priority: lengthFirst, loadCapacityFirst
    void clearReplacableVertexMat();
    void updateGraphForUpdate(Route originalRoute, Route altRoute);
    void updateGraph();

private:
    Graph *graph;
    AdjMatrix *adjMat;
    Graph graphForUpdate;
    QVector<QVector<int> > replacableVertexMat;
    QVector<QVector<int> > edgeLoadIncInfoForRemapping;
    QVector<Route> routesWillBeUpdatedForRemapping;

    void filterRoutesByPassedVertex(QVector<Route> &routes, int vertexIndex);
    void filterRoutesBySrcOrDest(QVector<Route> &routes, int vertexIndex);
    void filterRoutesBySrc(QVector<Route> &routes, int srcVertexIndex);
    void filterRoutesByDest(QVector<Route> &routes, int destVertexIndex);
    void reconstructRoutesBasedOnReplacedDests(QVector<Route> &routes, int destBefore, int destAfter);
    void replaceVerticesInMiddleOfRoutes(QVector<Route> &routes, int destBefore, int destAfter);
    bool isConnectedAllVerticesRelatedWithReplacingInOneRoute(Route &route, int originalVertexIndex, int altVertexIndex, QVector< QVector<int> > &adjacencyMatrix);
    QVector<Route> candidateRoutesWhoseVerticesRelatedWithReplacingAreConnected(Route originalRoute, QVector<Route> candidateRoutes);
};

#endif // ROUTEGUIDE_H
