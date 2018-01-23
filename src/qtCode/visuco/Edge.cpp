#include "Edge.h"

Edge::Edge(int index, int source, int target, int load, float byte)
{
    this->index = index;
    this->source = source;
    this->target = target;
    this->load = load;
    this->byte = byte;
}

void Edge::setIndex(int index) { this->index = index; }
void Edge::setSource(int source) { this->source = source; }
void Edge::setTarget(int target) { this->target = target; }
void Edge::setLoad(int load) { this->load = load; }
void Edge::incLoad(int inc) { load += inc; }
void Edge::setByte(float byte) { this->byte = byte; }
void Edge::incByte(float inc) { byte += inc; }
void Edge::setMetric(QString metric, float val)
{
    if (metric == "load") setLoad((int)val);
    else if (metric == "byte") setByte(val);
    else qWarning() << "undefined: check setMetric in Edge class";
}

int Edge::getIndex() { return index; }
int Edge::getSource() { return source; }
int Edge::getTarget() { return target; }
int Edge::getLoad() { return load; }
float Edge::getByte() { return byte; }
float Edge::getMetric(QString metric)
{
    float result = 0.0;

    if (metric == "load") result = (float)getLoad();
    else if (metric == "byte") result = getByte();
    else qWarning() << "undefined: check getMetric in Edge class";

    return result;
}
