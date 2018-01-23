#ifndef ROUTE_H
#define ROUTE_H

#include<QVector>
#include "Edge.h"

class Route
{
public:
    Route(int index = -1, int src = 1, int dest = -1, QVector<int> arcs = QVector<int>(), float byte = 0.0, int load = 0, int maxEdgeLoad = 0, float maxEdgeByte = 0);

    void setIndex(int index);
    void setSrc(int src);
    void setDest(int dest);
    void setArcs(QVector<int> arcs);
    void setByte(float byte);
    void setHopbyte(float hopbyte);
    void setLoad(int load);
    void setMaxEdgeLoad(int maxEdgeLoad);
    void setMaxEdgeByte(float maxEdgeByte);
    void setMetric(QString metric, float val);
    int getIndex();
    int getSrc();
    int getDest();
    QVector<int> getArcs();
    int getLength();
    float getByte();
    float getHopbyte();
    int getLoad();
    int getMaxEdgeLoad();
    float getMaxEdgeByte();
    float getMetric(QString metric);
    bool includesEdge(Edge e, bool orderSensitive = false);

private:
    int index;
    int src;
    int dest;
    QVector<int> arcs;
    float byte;
    float hopbyte;
    int load;
    int maxEdgeLoad;
    float maxEdgeByte;
};

#endif // ROUTE_H
