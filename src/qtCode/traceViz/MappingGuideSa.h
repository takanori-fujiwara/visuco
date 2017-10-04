#ifndef MAPPINGGUIDESA_H
#define MAPPINGGUIDESA_H

#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QProcess>
#include <QtCore/qmath.h>
#include <QtGlobal>
#include <QTime>

#include <algorithm>
#include "combination.h"
#include "Graph.h"
#include "AdjMatrix.h"

class MappingGuideSA
{
public:
    MappingGuideSA();
    MappingGuideSA(Graph* graph, AdjMatrix* adjMat, QString rScript, QString saQapScriptFile);
    void initilize(Graph* graph, AdjMatrix* adjMat, QString rScript, QString saQapScriptFile);
    QVector<int> suggestMapping(QVector<int> refinementTargetIndices, QString type, QString weightMatFile, QString distMatFile, QString dirForIntermidFiles, QString outputFile, int rep = 100);
    QVector<int> saQap(QString weightMatFile, QString distMatFile, QString outputFile, int rep = 100);
    QVector<int> saQapWithPartition(int numVertices, int numPartitions, QString dirForIntermidFiles, QString outputFilePath);
    QVector<int> refineMappingOptimal(QVector<int> refinementTargetIndices, QVector<int> initBijection, QVector<QVector<int> > &weightMat, QVector<QVector<int> > &distMat);
    QVector<int> refineMappingRandomized(QVector<int> refinementTargetIndices, QVector<int> initBijection, int rep, QVector<QVector<int> > &weightMat, QVector<QVector<int> > &distMat);
    int evalQap(QVector<int> &bijection, QVector<QVector<int> > &weightMat, QVector<QVector<int> > &distMat);
    QVector<QPair<int,int> > findCandidatesReplaced(int indexToBeReplaced, QVector<int> &currentBijection, QVector<QVector<int> > &weightMat, QVector<QVector<int> > &distMat);
    static bool evalLessThan(QPair<int,int> candidate1, QPair<int,int> candidate2);
    int selectIndexFromCandidates(QVector<QPair<int,int> > candidatesAndEvals);
    void test(QString qapResultFilePath, QString weightMatFilePath, QString distMatFilePath);

    QVector<int> genCurrentBijection();

private:
    Graph* graph;
    AdjMatrix* adjMat;
    QString rScript;
    QString saQapScriptFile;
    int thresForSaPartition;
};

#endif // MAPPINGGUIDESA_H
