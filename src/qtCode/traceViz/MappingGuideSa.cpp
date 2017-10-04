#include "MappingGuideSa.h"
#include "MappingGuide.h"

MappingGuideSA::MappingGuideSA()
{
    thresForSaPartition = 256;
}

MappingGuideSA::MappingGuideSA(Graph* graph, AdjMatrix* adjMat, QString rScript, QString saQapScriptFile)
{
    thresForSaPartition = 256;
    this->graph = graph;
    this->adjMat = adjMat;
    this->rScript = rScript;
    this->saQapScriptFile = saQapScriptFile;
}

void MappingGuideSA::initilize(Graph* graph, AdjMatrix* adjMat, QString rScript, QString saQapScriptFile)
{
    thresForSaPartition = 256;
    this->graph = graph;
    this->adjMat = adjMat;
    this->rScript = rScript;
    this->saQapScriptFile = saQapScriptFile;
}

// TODO: change rinside version later
QVector<int> MappingGuideSA::saQap(QString weightMatFile, QString distMatFile, QString outputFile, int rep)
{
    QVector<int> qapResult;
    QString command = rScript + " " + saQapScriptFile + " -wf " + weightMatFile + " -df " + distMatFile + " -of " + outputFile + " -r " + QString::number(rep);// + " -v ";

    qDebug() << "command: " << command;
    if (QProcess::execute(command) < 0) {
        qCritical() << "Cannot start" << saQapScriptFile;
    }

    QFile qapResultFile(outputFile);
    if (qapResultFile.open(QIODevice::ReadOnly)) {
        QTextStream ts(&qapResultFile);

        while (!ts.atEnd()) {
            QString line = ts.readLine();
            qapResult.push_back(line.toInt());
        }
    }
    qapResultFile.close();

    return qapResult;
}

QVector<int> MappingGuideSA::saQapWithPartition(int numVertices, int numPartitions, QString dirForIntermidFiles, QString outputFilePath)
{
    QVector<int> result(numVertices);

    // load resizedGraphIndexToOriginalIndices
    QVector<QVector<int> > wResizedGraphIndexToOriginalIndices;
    QVector<QVector<int> > dResizedGraphIndexToOriginalIndices;
    QFile rFile(dirForIntermidFiles + "route_weight_mat_resizedGraphIndexToOriginalIndices.csv");
    QFile dFile(dirForIntermidFiles + "dist_mat_resizedGraphIndexToOriginalIndices.csv");
    if (rFile.open(QIODevice::ReadOnly)) {
        QTextStream ts(&rFile);
        while (!ts.atEnd()) {
            QString line = ts.readLine();
            QStringList elements = line.split(",");
            QVector<int> originalIndices;
            foreach(QString e, elements) {
                originalIndices.push_back(e.toInt());
            }
            wResizedGraphIndexToOriginalIndices.push_back(originalIndices);
        }
        rFile.close();
    }
    if (dFile.open(QIODevice::ReadOnly)) {
        QTextStream ts(&dFile);
        while (!ts.atEnd()) {
            QString line = ts.readLine();
            QStringList elements = line.split(",");
            QVector<int> originalIndices;
            foreach(QString e, elements) {
                originalIndices.push_back(e.toInt());
            }
            dResizedGraphIndexToOriginalIndices.push_back(originalIndices);
        }
        dFile.close();
    }

    // qap for resized graph
    QVector<int> resizedQapResult = saQap(dirForIntermidFiles + "route_weight_mat_resized.csv", dirForIntermidFiles + "dist_mat_resized.csv", dirForIntermidFiles + "qapresult_resized.txt");

    // calc qap for each subgraph
    // TODO: here we can parallelize
    for (int i = 0; i < resizedQapResult.size(); ++i) {
        int wSubgraphIndex = i;
        int dSubgraphIndex = resizedQapResult.at(i);

        QVector<int> subgraphQapResult =
                saQap(dirForIntermidFiles + "route_weight_mat_partitioned" + QString::number(wSubgraphIndex) + ".csv",
                      dirForIntermidFiles + "dist_mat_partitioned" + QString::number(dSubgraphIndex) + ".csv",
                      dirForIntermidFiles + "qapresult_partitioned" + QString::number(i) + ".txt",
                      10);

        for (int j = 0; j < subgraphQapResult.size(); ++j) {
            int wIndex = wResizedGraphIndexToOriginalIndices.at(wSubgraphIndex).at(j);
            int dIndex = dResizedGraphIndexToOriginalIndices.at(dSubgraphIndex).at(subgraphQapResult.at(j));
            result[wIndex] = dIndex;
        }
    }

    QFile outFile(outputFilePath);
    if (!outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "cannot open file: " << outputFilePath;
    }

    QTextStream out(&outFile);
    foreach(int dIndex, result) {
        out << dIndex << "\n";
    }
    outFile.close();

    return result;
}

void MappingGuideSA::test(QString qapResultFilePath, QString weightMatFilePath, QString distMatFilePath)
{
    QVector<int> bijection;
    QVector<QVector<int> > weightMat;
    QVector<QVector<int> > distMat;

    QFile bFile(qapResultFilePath);
    QFile wFile(weightMatFilePath);
    QFile dFile(distMatFilePath);

    if (bFile.open(QIODevice::ReadOnly)) {
        QTextStream ts(&bFile);
        while (!ts.atEnd()) {
            QString line = ts.readLine();
            bijection.push_back(line.toInt());
        }
        bFile.close();
    }
    if (wFile.open(QIODevice::ReadOnly)) {
        QTextStream ts(&wFile);
        while (!ts.atEnd()) {
            QString line = ts.readLine();
            QStringList elements = line.split(",");
            QVector<int> wMatRow;
            foreach(QString e, elements) {
                wMatRow.push_back(e.toInt());
            }
            weightMat.push_back(wMatRow);
        }
        wFile.close();
    }
    if (dFile.open(QIODevice::ReadOnly)) {
        QTextStream ts(&dFile);
        while (!ts.atEnd()) {
            QString line = ts.readLine();
            QStringList elements = line.split(",");
            QVector<int> dMatRow;
            foreach(QString e, elements) {
                dMatRow.push_back(e.toInt());
            }
            distMat.push_back(dMatRow);
        }
        dFile.close();
    }

    refineMappingOptimal(QVector<int>({0,10}), bijection, weightMat, distMat);
    refineMappingRandomized(QVector<int>({0,10}), bijection, 10, weightMat, distMat);
}

QVector<int> MappingGuideSA::refineMappingOptimal(QVector<int> refinementTargetIndices, QVector<int> initBijection, QVector<QVector<int> > &weightMat, QVector<QVector<int> > &distMat)
{
    // find optimal solution by evaluating all possible patterns
    // calc time: nCm * m! * (calculation time for eval)
    QVector<int> result = initBijection;
    int bestE = evalQap(initBijection, weightMat, distMat);

    const int n = initBijection.size();
    const int r = refinementTargetIndices.size();

    QVector<int> dataForComb;
    for(int i=0; i<n; ++i){
        dataForComb.push_back(i);
    }

    // generate all possible combination of indices replaced with selected indices
    QVector<QVector<int> > combinations;
    do {
        QVector<int> comb;
        for (int i = 0; i < r; ++i){
            comb.push_back(dataForComb[i]);
        }
        combinations.push_back(comb);
    } while (next_combination(dataForComb.begin(), dataForComb.begin()+r, dataForComb.end()));

    // repeat eval and find best one all permulation for each combination
    foreach(QVector<int> comb, combinations) {
        const int m = comb.size();
        QVector<int> dataForPerm;
        for(int i = 0; i < m; ++i){
            dataForPerm.push_back(comb.at(i));
        }

        do{
            // generate bijection to be evaluated
            QVector<int> bijection = initBijection;
            for (int i = 0; i < refinementTargetIndices.size(); ++i) {
                bijection[refinementTargetIndices.at(i)] = dataForPerm.at(i);
                bijection[dataForPerm.at(i)] = refinementTargetIndices.at(i);
            }

            int e = evalQap(bijection, weightMat, distMat);

            if (e < bestE) {
                bestE = e;
                result = bijection;
            }
        } while (std::next_permutation(dataForPerm.begin(), dataForPerm.end()));
    }

    qDebug() << "result val" << bestE;
    return result;
}

QVector<int> MappingGuideSA::refineMappingRandomized(QVector<int> refinementTargetIndices, QVector<int> initBijection, int rep, QVector<QVector<int> > &weightMat, QVector<QVector<int> > &distMat)
{
    QVector<int> result;
    int bestE = evalQap(initBijection, weightMat, distMat);

    QVector<int> targets = refinementTargetIndices;

    for (int i = 0; i < rep; ++i) {
        // shuffle the evaluation order
        std::random_shuffle(targets.begin(), targets.end());

        QVector<int> bijection = initBijection;

        for (int j = 0; j < targets.size(); ++j) {
            QVector<QPair<int,int> > candidatesAndEvals = findCandidatesReplaced(targets.at(j), bijection, weightMat, distMat);
            // select index to be replaced weighted randomly
            int index = selectIndexFromCandidates(candidatesAndEvals);
            bijection[targets.at(j)] = index;
            bijection[index] = targets.at(j);
        }

        int e = evalQap(bijection, weightMat, distMat);

        if (e < bestE) {
            bestE = e;
            result = bijection;
        }
    }

    qDebug() << "result val" << bestE;
    return result;
}

int MappingGuideSA::evalQap(QVector<int> &bijection, QVector<QVector<int> > &weightMat, QVector<QVector<int> > &distMat)
{
    // TODO: should use map reduce here
    int result = 0;

    for (int i = 0; i < weightMat.size(); ++i) {
        for (int j = 0; j < weightMat.at(i).size(); ++j) {
            int i_ = bijection.at(i);
            int j_ = bijection.at(j);

            int w = weightMat.at(i).at(j);
            int d = distMat.at(i_).at(j_);

            result += w * d;
        }
    }

    return result;
}

QVector<QPair<int,int> > MappingGuideSA::findCandidatesReplaced(int indexToBeReplaced, QVector<int> &currentBijection, QVector<QVector<int> > &weightMat, QVector<QVector<int> > &distMat)
{
    QVector<QPair<int,int> > candidateIndicesAndImprovements;

    int currentE = evalQap(currentBijection, weightMat, distMat);

    // TODO: should parallelize here
    for (int i = 0; i < currentBijection.size(); ++i) {
        if (i != indexToBeReplaced) {
            QVector<int> tmpBijection = currentBijection;
            tmpBijection[i] = indexToBeReplaced;
            tmpBijection[indexToBeReplaced] = i;

            int e = evalQap(tmpBijection, weightMat, distMat);

            if (e < currentE) {
                candidateIndicesAndImprovements.push_back(qMakePair(i, (currentE - e)));
            }
        }
    }

    if (candidateIndicesAndImprovements.size() == 0) { // when not found
        candidateIndicesAndImprovements.push_back(qMakePair(indexToBeReplaced, 1));
    }
    return candidateIndicesAndImprovements;
}

bool MappingGuideSA::evalLessThan(QPair<int,int> candidate1, QPair<int,int> candidate2)
{
    return candidate1.second < candidate2.second;
}

int MappingGuideSA::selectIndexFromCandidates(QVector<QPair<int,int> > candidatesAndEvals)
{
    // select index randomly reflected their evaluated values
    int retIndex = 0;

    qsrand(QTime::currentTime().msec());

    qSort(candidatesAndEvals.begin(), candidatesAndEvals.end(), evalLessThan);
    //qDebug() << candidatesAndEvals;

    QVector<int> weightTable;
    int totalWeight = 0;
    for (int i = 0; i < candidatesAndEvals.size(); ++i) {
        weightTable.push_back(candidatesAndEvals.at(i).second);
        totalWeight += candidatesAndEvals.at(i).second;
    }

    int thr = qrand() % (totalWeight + 1);

    int sum = 0;

    for (int i = 0; i < weightTable.size(); ++i) {
        sum += weightTable.at(i);
        if (thr < sum) {
            retIndex = i;
            break;
        }
    }

    //qDebug() << candidatesAndEvals.at(retIndex).first;
    return candidatesAndEvals.at(retIndex).first;
}

// TODO: change args later after finising change these to RInside version
QVector<int> MappingGuideSA::suggestMapping(QVector<int> refinementTargetIndices, QString type, QString weightMatFile, QString distMatFile, QString dirForIntermidFiles, QString outputFile, int rep)
{
    QVector<int> result;
    QVector<int> currentBijection = genCurrentBijection();
    int numVertices = currentBijection.size();

    QVector<QVector<int> > weightMat;
    QVector<QVector<int> > distMat;

    QFile wFile(weightMatFile);
    QFile dFile(distMatFile);
    if (wFile.open(QIODevice::ReadOnly)) {
        QTextStream ts(&wFile);
        while (!ts.atEnd()) {
            QString line = ts.readLine();
            QStringList elements = line.split(",");
            QVector<int> wMatRow;
            foreach(QString e, elements) {
                wMatRow.push_back(e.toInt());
            }
            weightMat.push_back(wMatRow);
        }
        wFile.close();
    }
    if (dFile.open(QIODevice::ReadOnly)) {
        QTextStream ts(&dFile);
        while (!ts.atEnd()) {
            QString line = ts.readLine();
            QStringList elements = line.split(",");
            QVector<int> dMatRow;
            foreach(QString e, elements) {
                dMatRow.push_back(e.toInt());
            }
            distMat.push_back(dMatRow);
        }
        dFile.close();
    }

    if (type == "all-ma") {
        qDebug() << "all-ma";
        if (numVertices <= thresForSaPartition) {
            qDebug() << "Current Obj" << evalQap(currentBijection, weightMat, distMat);
            result = saQap(weightMatFile, distMatFile, outputFile, rep);
        } else {
            if (numVertices == thresForSaPartition * 2) thresForSaPartition /= 2; // when each partition has only 2 nodes, it doesn't work well (rule of thumb).
            result = saQapWithPartition(numVertices, thresForSaPartition, dirForIntermidFiles, outputFile);
        }
    } else if (type == "selected-optimal") {
        qDebug() << "selected-optimal";
        result = refineMappingOptimal(refinementTargetIndices, currentBijection, weightMat, distMat);
    } else if (type == "selected-randomized") {
        qDebug() << "selected-randomized";
        result = refineMappingRandomized(refinementTargetIndices, currentBijection, rep, weightMat, distMat);
    }

    return result;
}

QVector<int> MappingGuideSA::genCurrentBijection()
{
    QVector<int> result;

    foreach(Vertex v, graph->getVertices()) {
        int matColNum = adjMat->coordToColNum( v.getCoord() );
        result.push_back(matColNum);
    }

    return result;
}
