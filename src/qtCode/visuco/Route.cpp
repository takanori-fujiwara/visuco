#include "Route.h"

Route::Route(int index, int src, int dest, QVector<int> arcs, float byte, int load, int maxEdgeLoad, float maxEdgeByte)
{
    this->index = index;
    this->src = src;
    this->dest = dest;
    this->arcs = arcs;
    this->byte = byte;
    this->hopbyte = (arcs.size() - 1) * byte;
    this->load = load;
    this->maxEdgeLoad = maxEdgeLoad;
    this->maxEdgeByte = maxEdgeByte;
}

void Route::setIndex(int index) { this->index = index; }
void Route::setSrc(int src) { this->src = src; }
void Route::setDest(int dest) { this->dest = dest; }
void Route::setArcs(QVector<int> arcs) { this->arcs = arcs; }
void Route::setByte(float byte) { this->byte = byte; }
void Route::setHopbyte(float hopbyte) { this->hopbyte = hopbyte; }
void Route::setLoad(int load) { this->load = load; }
void Route::setMaxEdgeLoad(int maxEdgeLoad) { this->maxEdgeLoad = maxEdgeLoad; }
void Route::setMaxEdgeByte(float maxEdgeByte) { this->maxEdgeByte = maxEdgeByte; }
void Route::setMetric(QString metric, float val)
{
    if (metric == "byte") byte = val;
    else if (metric == "hopbyte") hopbyte = val;
    else qWarning() << "undefined: check setMetric in Route class";
}

int Route::getIndex() { return index; }
int Route::getSrc() { return src; }
int Route::getDest() { return dest; }
QVector<int> Route::getArcs() { return arcs; }
int Route::getLength() { return arcs.size() - 1; }
float Route::getByte() { return byte; }
float Route::getHopbyte() { return hopbyte; }
int Route::getLoad() { return load; }
int Route::getMaxEdgeLoad() { return maxEdgeLoad; }
float Route::getMaxEdgeByte() { return maxEdgeByte; }
float Route::getMetric(QString metric)
{
    float result = 0.0;

    if (metric == "length") result = (float)getLength();
    else if (metric == "byte") result = getByte();
    else if (metric == "hopbyte") result = getHopbyte();
    else qWarning() << "undefined: check getMetric in Route class. Metic: " << metric;

    return result;
}

bool Route::includesEdge(Edge e, bool orderSensitive)
{
    for (int i = 0; i < arcs.size() - 1; ++i) {
        if (e.getSource() == arcs.at(i) && e.getTarget() == arcs.at(i + 1)) return true;
        if (!orderSensitive) {
            if (e.getTarget() == arcs.at(i) && e.getSource() == arcs.at(i + 1)) return true;
        }
    }

    return false;
}
