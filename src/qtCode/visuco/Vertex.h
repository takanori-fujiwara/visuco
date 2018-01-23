#ifndef VERTEX_H
#define VERTEX_H

#include<QVector>
#include<QPointF>

class Vertex
{
public:
    Vertex(int index = -1, QVector<int> ranks = QVector<int>(), QVector<int> coord = QVector<int>(),  QVector<QVector<int>> childCoords = QVector<QVector<int>>());

    void setIndex(int index);
    void setRanks(QVector<int> ranks);
    void setCoord(QVector<int> coord);
    void setChildCoords(QVector<QVector<int>> childCoords);
    void setGraphPos(QPointF graphPos);
    void setInDegOfEdges(int deg);
    void setInDegOfEdgesLoad(int wDeg);
    void setInDegOfEdgesByte(float wDeg);
    void setOutDegOfEdges(int deg);
    void setOutDegOfEdgesLoad(int wDeg);
    void setOutDegOfEdgesByte(float wDeg);
    void setInDegOfRoutes(int deg);
    void setInDegOfRoutesLoad(int deg);
    void setOutDegOfRoutes(int deg);
    void setOutDegOfRoutesLoad(int deg);

    int getIndex();
    QVector<int> getRanks();
    QVector<int> getCoord();
    QVector<QVector<int>> getChildCoords();
    QPointF getGraphPos();
    int getInDegOfEdges();
    int getInDegOfEdgesLoad();
    float getInDegOfEdgesByte();
    int getOutDegOfEdges();
    int getOutDegOfEdgesLoad();
    float getOutDegOfEdgesByte();
    int getInDegOfRoutes();
    int getInDegOfRoutesLoad();
    int getOutDegOfRoutes();
    int getOutDegOfRoutesLoad();
    int getInMinusOutDegOfEdgesLoad();
    int getOutMinusInDegOfEdgesLoad();
    float getInMinusOutDegOfEdgesByte();
    float getOutMinusInDegOfEdgesByte();

private:
    int index;
    QVector<int> ranks; // mpi rank. when using multicore, node could have multiple ranks
    QVector<int> coord;
    QVector<QVector<int>> childCoords; // used for only aggregated vertex
    QPointF graphPos; // [-1.0, 1.0]
    int inDegOfEdges;
    int inDegOfEdgesLoad;
    float inDegOfEdgesByte;
    int outDegOfEdges;
    int outDegOfEdgesLoad;
    float outDegOfEdgesByte;
    int inDegOfRoutes;
    int inDegOfRoutesLoad;
    int outDegOfRoutes;
    int outDegOfRoutesLoad;
};

#endif // VERTEX_H
