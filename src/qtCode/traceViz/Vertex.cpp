#include "Vertex.h"

Vertex::Vertex(int index, QVector<int> ranks, QVector<int> coord,  QVector<QVector<int>> childCoords)
{
    this->index = index;
    this->ranks = ranks;
    this->coord = coord;
    this->childCoords = childCoords;

    graphPos = QPointF(-100.0, -100.0);
    inDegOfEdges = 0;
    inDegOfEdgesLoad = 0;
    inDegOfEdgesByte = 0.0;
    outDegOfEdges = 0;
    outDegOfEdgesLoad = 0;
    outDegOfEdgesByte = 0.0;
    inDegOfRoutes = 0;
    inDegOfRoutesLoad = 0;
    outDegOfRoutes = 0;
    outDegOfRoutesLoad = 0;
}

void Vertex::setIndex(int index) { this->index = index; }
void Vertex::setChildCoords(QVector<QVector<int>> childCoords) { this->childCoords = childCoords; }
void Vertex::setRanks(QVector<int> ranks) { this->ranks = ranks; }
void Vertex::setCoord(QVector<int> coord) { this->coord = coord; }
void Vertex::setGraphPos(QPointF graphPos) { this->graphPos = graphPos; }
void Vertex::setInDegOfEdges(int deg) { this->inDegOfEdges = deg; }
void Vertex::setInDegOfEdgesLoad(int wDeg) { this->inDegOfEdgesLoad = wDeg; }
void Vertex::setInDegOfEdgesByte(float wDeg) { this->inDegOfEdgesByte = wDeg; }
void Vertex::setOutDegOfEdges(int deg) { this->outDegOfEdges = deg; }
void Vertex::setOutDegOfEdgesLoad(int wDeg) { this->outDegOfEdgesLoad = wDeg; }
void Vertex::setOutDegOfEdgesByte(float wDeg) { this->outDegOfEdgesByte = wDeg; }
void Vertex::setInDegOfRoutes(int deg) { this->inDegOfRoutes = deg; }
void Vertex::setInDegOfRoutesLoad(int deg) { this->inDegOfRoutesLoad = deg; }
void Vertex::setOutDegOfRoutes(int deg) { this->outDegOfRoutes = deg; }
void Vertex::setOutDegOfRoutesLoad(int deg) { this->outDegOfRoutesLoad = deg; }

int Vertex::getIndex() { return index; }
QVector<QVector<int>> Vertex::getChildCoords() { return childCoords; }
QVector<int> Vertex::getRanks() { return ranks; }
QVector<int> Vertex::getCoord() { return coord; }
QPointF Vertex::getGraphPos() { return graphPos; }
int Vertex::getInDegOfEdges() { return inDegOfEdges; }
int Vertex::getInDegOfEdgesLoad() { return inDegOfEdgesLoad; }
float Vertex::getInDegOfEdgesByte() { return inDegOfEdgesByte; }
int Vertex::getOutDegOfEdges() { return outDegOfEdges; }
int Vertex::getOutDegOfEdgesLoad() { return outDegOfEdgesLoad; }
float Vertex::getOutDegOfEdgesByte() { return outDegOfEdgesByte; }
int Vertex::getInDegOfRoutes() { return inDegOfRoutes; }
int Vertex::getInDegOfRoutesLoad() { return inDegOfRoutesLoad; }
int Vertex::getOutDegOfRoutes() { return outDegOfRoutes; }
int Vertex::getOutDegOfRoutesLoad() { return outDegOfRoutesLoad; }
int Vertex::getInMinusOutDegOfEdgesLoad() { return (inDegOfEdgesLoad - outDegOfEdgesLoad); }
int Vertex::getOutMinusInDegOfEdgesLoad() { return (outDegOfEdgesLoad - inDegOfEdgesLoad); }
float Vertex::getInMinusOutDegOfEdgesByte() { return (inDegOfEdgesByte - outDegOfEdgesByte); }
float Vertex::getOutMinusInDegOfEdgesByte() { return (outDegOfEdgesByte - inDegOfEdgesByte); }
