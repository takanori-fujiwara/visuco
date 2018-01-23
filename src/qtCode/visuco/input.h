///
/// This code is modified from source code developed by David Gasquez (https://github.com/davidgasquez/qap)
///

#ifndef INPUT_H
#define INPUT_H

#include "common.h"

#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDebug>

using namespace std;

class Input
{
public:
    Input(const string &filename);
    Input(const QString &flowMatFilePath, const QString &distMatFilePath);
    Input(int dimension, vector< vector<float> > distances, vector< vector<float> > flow);

    bool read();
    bool read(const QString &flowMatFilePath, const QString &distMatFilePath);

    string getFilename() const;
    int getDimension() const;

    vector< vector<float> > getDistances() const;
    vector< vector<float> > getFlow() const;

    void setDimension(int dimension);
    void setDistances(vector< vector<float> > distances);
    void setFlow(vector< vector<float> > flow);

private:
    string filename_;
    int dimension_;
    vector< vector<float> > distances_;
    vector< vector<float> > flow_;
};

#endif // INPUT_H
