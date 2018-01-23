#ifndef MAPPINGGUIDE_H
#define MAPPINGGUIDE_H

#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QProcess>
#include <QtCore/qmath.h>
#include <QtGlobal>
#include <QTime>

#include <algorithm>
#include <Eigen/Core>

#include "combination.h"
#include "Graph.h"
#include "AdjMatrix.h"
#include "memetic.h"

class MappingGuide
{
public:
    MappingGuide();
    MappingGuide(Graph* graph, AdjMatrix* adjMat);
    void initilize(Graph* graph, AdjMatrix* adjMat);
    QVector<int> suggestMapping(QVector<int> refinementTargetIndices, QString type, QString weightMatFile, QString distMatFile, QString dirForIntermidFiles, QString outputFile, int rep = 100, QString perfOutput = "");
    QVector<int> memeticQap(QString weightMatFile, QString distMatFile, QString outputFile, bool forSubGraph = false);
    QVector<int> memeticQapWithPartition(int numVertices, int numPartitions, QString dirForIntermidFiles, QString outputFilePath);
    QVector<int> refineMappingOptimal(QVector<int> refinementTargetIndices, QVector<int> initBijection, QVector<QVector<float> > &weightMat, QVector<QVector<float> > &distMat);
    QVector<int> refineMappingRandomized(QVector<int> refinementTargetIndices, QVector<int> initBijection, int rep, QVector<QVector<float> > &weightMat, QVector<QVector<float> > &distMat);
    float evalQap(QVector<int> &bijection, QVector<QVector<float> > &weightMat, QVector<QVector<float> > &distMat);
    float evalQap(Eigen::VectorXf &bijection, Eigen::MatrixXf &W, Eigen::MatrixXf &D);
    QVector<QPair<int,float> > findCandidatesReplaced(int indexToBeReplaced, QVector<int> &currentBijection, QVector<QVector<float> > &weightMat, QVector<QVector<float> > &distMat);
    static bool evalLessThan(QPair<int,float> candidate1, QPair<int,float> candidate2);
    int selectIndexFromCandidates(QVector<QPair<int,float> > candidatesAndEvals);
    void test(QString qapResultFilePath, QString weightMatFilePath, QString distMatFilePath);
    QVector<int> genCurrentBijection();
    double calcNumOfPermutation(int n, int m);
    void setUseMaGraphPartition(bool useMaGraphPartition);

private:
    Graph* graph;
    AdjMatrix* adjMat;
    int thresForMaPartition;

    // memetic params
    int maMaxEvaluations;
    int maPopulationSize;
    double maProbabilityLS;
    bool maInitRandom;
    int maMaxEvaluationsForSubGraph;
    int maPopulationSizeForSubGraph;
    double maProbabilityLSForSubGraph;
    bool maInitRandomForSubGraph;
    bool useMaGraphPartition;
    double time_;
};

#endif // MAPPINGGUIDE_H
