#ifndef EDGE_H
#define EDGE_H

#include <QString>
#include <QDebug>

class Edge
{
public:
    Edge(int index = -1, int source = -1, int target = -1, int numOfPassages = -1, float byte = -1.0);

    void setIndex(int index);
    void setSource(int source);
    void setTarget(int target);
    void setLoad(int load);
    void incLoad(int inc = 1);
    void setByte(float byte);
    void incByte(float inc);
    void setMetric(QString metric, float val);
    int getIndex();
    int getSource();
    int getTarget();
    int getLoad();
    float getByte();
    float getMetric(QString metric);

private:
    int index;
    int source;
    int target;
    int load;
    float byte;
};

#endif // EDGE_H
