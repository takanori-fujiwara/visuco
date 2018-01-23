#include "MappingGuide.h"
#include <thread>

MappingGuide::MappingGuide()
{
    thresForMaPartition = 256;
    maMaxEvaluations = 50000;
    maPopulationSize = 18;
    maProbabilityLS = 0.2;
    maInitRandom = true;
    maMaxEvaluationsForSubGraph = 50000;
    maPopulationSizeForSubGraph = 18;
    maProbabilityLSForSubGraph = 0.2;
    maInitRandomForSubGraph = true;
}

MappingGuide::MappingGuide(Graph* graph, AdjMatrix* adjMat)
{
    thresForMaPartition = 256;
    maMaxEvaluations = 50000;
    maPopulationSize = 18;
    maProbabilityLS = 0.2;
    maInitRandom = true;
    maMaxEvaluationsForSubGraph = 50000;
    maPopulationSizeForSubGraph = 18;
    maProbabilityLSForSubGraph = 0.2;
    maInitRandomForSubGraph = true;
}

void MappingGuide::initilize(Graph* graph, AdjMatrix* adjMat)
{
    thresForMaPartition = 256;
    this->graph = graph;
    this->adjMat = adjMat;
    maMaxEvaluations = 50000;
    maPopulationSize = 18;
    maProbabilityLS = 0.2;
    maInitRandom = true;
    maMaxEvaluationsForSubGraph = 50000;
    maPopulationSizeForSubGraph = 18;
    maProbabilityLSForSubGraph = 0.2;
    maInitRandomForSubGraph = true;
}

// TODO: change rinside version later
QVector<int> MappingGuide::memeticQap(QString weightMatFile, QString distMatFile, QString outputFile, bool forSubGraph)
{
    QVector<int> qapResult;

     // Set seed
    int seed = 123456;
    Input input(weightMatFile, distMatFile);

    // This exception handle is required to solve the bug in METIS (it sometimes does not produce correct csv)
    if (input.getDistances().size() != input.getFlow().size()) {
        qWarning() << "sizes of distace and flow matrix is different. returned the originial order.";
        int dim = qMax(input.getDistances().size(), input.getFlow().size());
        for (int i = 0; i < dim; ++i) {
            qapResult.push_back(i);
        }
        return qapResult;
    }
    // Stationary Genetic Algorythm with position based crossings with 100000 evaluations
    srand(seed);

    if (!forSubGraph) {
        Memetic m(input, maMaxEvaluations, maPopulationSize, maProbabilityLS, maInitRandom);
        qapResult = QVector<int>::fromStdVector(m.getSolution());

        QFile qapCostFile(outputFile.left(outputFile.size() - 4) + "_cost.txt");
        int improveRate = (1.0 - m.getCost() / m.getCostWithOriginalOrder()) * 100;
        if (qapCostFile.open(QIODevice::WriteOnly)) {
            QTextStream ts(&qapCostFile);
            ts << m.getCostWithOriginalOrder() << "\n";
            ts << m.getCost() << "\n";

            ts << improveRate << "%\n";
        }
        qapCostFile.close();
    } else {
        Memetic m(input, maMaxEvaluationsForSubGraph, maPopulationSizeForSubGraph,
                  maProbabilityLSForSubGraph, maInitRandomForSubGraph);
        qapResult = QVector<int>::fromStdVector(m.getSolution());

        QFile qapCostFile(outputFile.left(outputFile.size() - 4) + "_cost.txt");
        int improveRate = (1.0 - m.getCost() / m.getCostWithOriginalOrder()) * 100;
        if (qapCostFile.open(QIODevice::WriteOnly)) {
            QTextStream ts(&qapCostFile);
            ts << m.getCostWithOriginalOrder() << "\n";
            ts << m.getCost() << "\n";

            ts << improveRate << "%\n";
        }
        qapCostFile.close();

    }

    QFile qapResultFile(outputFile);
    if (!qapResultFile.open(QIODevice::WriteOnly | QIODevice::Text)) qWarning() << "cannot open file: " << outputFile;
    QTextStream out(&qapResultFile);
    foreach (int i, qapResult) {
        out << i << "\n";
    }
    qapResultFile.close();

    return qapResult;
}

QVector<int> MappingGuide::memeticQapWithPartition(int numVertices, int numPartitions, QString dirForIntermidFiles, QString outputFilePath)
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
    QVector<int> resizedQapResult = memeticQap(dirForIntermidFiles + "route_weight_mat_resized.csv", dirForIntermidFiles + "dist_mat_resized.csv", dirForIntermidFiles + "qapresult_resized.txt", false);

    // calc qap for each subgraph
    // use multi workers
    int numWorkers = std::max(std::thread::hardware_concurrency(), 1u); // obtain max thread num
    std::vector<std::thread> worker;

    int N = resizedQapResult.size();
    for (int i = 0; i < numWorkers; ++i) {
        worker.emplace_back([&](int id) {
            int r0 = N / numWorkers * id + std::min(N % numWorkers, id);
            int r1 = N / numWorkers * (id + 1) + std::min(N % numWorkers, id + 1);

            for (int j = r0; j < r1; ++j) {
                int wSubgraphIndex = j;
                int dSubgraphIndex = resizedQapResult.at(j);

                QVector<int> subgraphQapResult =
                        memeticQap(dirForIntermidFiles + "route_weight_mat_partitioned" + QString::number(wSubgraphIndex) + ".csv",
                              dirForIntermidFiles + "dist_mat_partitioned" + QString::number(dSubgraphIndex) + ".csv",
                              dirForIntermidFiles + "qapresult_partitioned" + QString::number(j) + ".txt", true);

                for (int k = 0; k < subgraphQapResult.size(); ++k) {
                    int wIndex = wResizedGraphIndexToOriginalIndices.at(wSubgraphIndex).at(k);
                    int dIndex = dResizedGraphIndexToOriginalIndices.at(dSubgraphIndex).at(subgraphQapResult.at(k));

                    result[wIndex] = dIndex;
                }

            }
        }, i);
    }
    for (auto& t : worker) t.join();

    // non multithread version
//    for (int i = 0; i < resizedQapResult.size(); ++i) {
//        int wSubgraphIndex = i;
//        int dSubgraphIndex = resizedQapResult.at(i);

//        QVector<int> subgraphQapResult =
//                memeticQap(dirForIntermidFiles + "route_weight_mat_partitioned" + QString::number(wSubgraphIndex) + ".csv",
//                      dirForIntermidFiles + "dist_mat_partitioned" + QString::number(dSubgraphIndex) + ".csv",
//                      dirForIntermidFiles + "qapresult_partitioned" + QString::number(i) + ".txt", true);

//        for (int j = 0; j < subgraphQapResult.size(); ++j) {
//            int wIndex = wResizedGraphIndexToOriginalIndices.at(wSubgraphIndex).at(j);
//            int dIndex = dResizedGraphIndexToOriginalIndices.at(dSubgraphIndex).at(subgraphQapResult.at(j));

//            result[wIndex] = dIndex;
//        }
//    }

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

void MappingGuide::test(QString qapResultFilePath, QString weightMatFilePath, QString distMatFilePath)
{
    QVector<int> bijection;
    QVector<QVector<float> > weightMat;
    QVector<QVector<float> > distMat;

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
            QVector<float> wMatRow;
            foreach(QString e, elements) {
                wMatRow.push_back(e.toFloat());
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
            QVector<float> dMatRow;
            foreach(QString e, elements) {
                dMatRow.push_back(e.toFloat());
            }
            distMat.push_back(dMatRow);
        }
        dFile.close();
    }

    refineMappingOptimal(QVector<int>({0,10}), bijection, weightMat, distMat);
    refineMappingRandomized(QVector<int>({0,10}), bijection, 10, weightMat, distMat);
}

QVector<int> MappingGuide::refineMappingOptimal(QVector<int> refinementTargetIndices, QVector<int> initBijection, QVector<QVector<float> > &weightMat, QVector<QVector<float> > &distMat)
{
    // find optimal solution by evaluating all possible patterns
    // calc time: nCm * m! * (calculation time for eval)
    QVector<int> result = initBijection;
    float bestE = evalQap(initBijection, weightMat, distMat) + 100000.0;

    const int n = initBijection.size();
    const int r = refinementTargetIndices.size();

    QVector<int> dataForComb;
    for(int i = 0; i < n; ++i){
        dataForComb.push_back(i);
    }

    // generate all possible combination of indices replaced with selected indices
    //qDebug() << "Start generating combinations";
    //high_resolution_clock::time_point t1 = high_resolution_clock::now();
    QVector<QVector<int> > combinations;
    do {
        QVector<int> comb;
        for (int i = 0; i < r; ++i){
            comb.push_back(dataForComb[i]);
        }
        combinations.push_back(comb);
    } while (next_combination(dataForComb.begin(), dataForComb.begin()+r, dataForComb.end()));

    //high_resolution_clock::time_point t2 = high_resolution_clock::now();
    //duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
    //time_ = time_span.count();
    //qDebug() << "Generate Combination Time: " << time_;
    //qDebug() << "Number of Combinations: " << combinations.size();

    // repeat eval and find best one all permulation for each combination
    // use multi workers
    int numWorkers = 1;//std::max(std::thread::hardware_concurrency(), 1u); // obtain max thread num
    std::vector<std::thread> worker;
    QVector<QVector<int> > tmpResults = QVector<QVector<int>>(numWorkers, initBijection);
    QVector<float> tmpBestEs = QVector<float>(numWorkers, bestE);

    //t1 = high_resolution_clock::now();
    int N = combinations.size();
    for (int i = 0; i < numWorkers; ++i) {
        worker.emplace_back([&](int id) {
            int r0 = N / numWorkers * id + std::min(N % numWorkers, id);
            int r1 = N / numWorkers * (id + 1) + std::min(N % numWorkers, id + 1);

            for (int j = r0; j < r1; ++j) {
                const int m = combinations.at(j).size();
                QVector<int> dataForPerm;
                for(int k = 0; k < m; ++k){
                    dataForPerm.push_back(combinations.at(j).at(k));
                }

                do {
                    // generate bijection to be evaluated
                    QVector<int> bijection = initBijection;

                    for (int k = 0; k < r; ++k) {
                        //bijection[refinementTargetIndices.at(k)] = dataForPerm.at(k);
                        //bijection[dataForPerm.at(k)] = refinementTargetIndices.at(k);
                        int tmp = bijection.at(refinementTargetIndices.at(k)); // for swap
                        bijection[refinementTargetIndices.at(k)] = bijection.at(dataForPerm.at(k));
                        bijection[dataForPerm.at(k)] = tmp;
                    }

                    float e = evalQap(bijection, weightMat, distMat);

                    if (e < tmpBestEs.at(id)) {
                        tmpBestEs[id] = e;
                        tmpResults[id] = bijection;
                    }
                } while (std::next_permutation(dataForPerm.begin(), dataForPerm.end()));

            }
        }, i);
    }
    for (auto& t : worker) t.join();

    // this wastes a time but not big problem (workers are not many)
    for (int i = 0; i < numWorkers; ++i) {
        if (tmpBestEs.at(i) < bestE) {
            bestE = tmpBestEs.at(i);
            result = tmpResults.at(i);
        }
    }
    //t2 = high_resolution_clock::now();
    //time_span = duration_cast<duration<double>>(t2 - t1);
    //time_ = time_span.count();
    //qDebug() << "Calc All Patterns Time: " << time_;

//    // non multithread version
//    t1 = high_resolution_clock::now();
//    foreach(QVector<int> comb, combinations) {
//        const int m = comb.size();
//        QVector<int> dataForPerm;
//        for(int i = 0; i < m; ++i){
//            dataForPerm.push_back(comb.at(i));
//        }

//        do{
//            // generate bijection to be evaluated
//            QVector<int> bijection = initBijection;
//            for (int i = 0; i < refinementTargetIndices.size(); ++i) {
//                bijection[refinementTargetIndices.at(i)] = dataForPerm.at(i);
//                bijection[dataForPerm.at(i)] = refinementTargetIndices.at(i);
//            }

//            float e = evalQap(bijection, weightMat, distMat);

//            if (e < bestE) {
//                bestE = e;
//                result = bijection;
//            }
//        } while (std::next_permutation(dataForPerm.begin(), dataForPerm.end()));
//    }
//    t2 = high_resolution_clock::now();
//    time_span = duration_cast<duration<double>>(t2 - t1);
//    time_ = time_span.count();
//    qDebug() << "Calc All Patterns Time: " << time_;

    qDebug() << "result val" << bestE;
    return result;
}

QVector<int> MappingGuide::refineMappingRandomized(QVector<int> refinementTargetIndices, QVector<int> initBijection, int rep, QVector<QVector<float> > &weightMat, QVector<QVector<float> > &distMat)
{
    QVector<int> result;
    float bestE = evalQap(initBijection, weightMat, distMat) + 100000.0;

    // repeat eval and find best one all permulation for each combination
    // use multi workers
    int numWorkers = std::max(std::thread::hardware_concurrency(), 1u); // obtain max thread num
    std::vector<std::thread> worker;
    QVector<QVector<int> > tmpResults = QVector<QVector<int>>(numWorkers, initBijection);
    QVector<float> tmpBestEs = QVector<float>(numWorkers, bestE);

    //high_resolution_clock::time_point t1 = high_resolution_clock::now();

    int N = rep;
    for (int i = 0; i < numWorkers; ++i) {
        worker.emplace_back([&](int id) {
            int r0 = N / numWorkers * id + std::min(N % numWorkers, id);
            int r1 = N / numWorkers * (id + 1) + std::min(N % numWorkers, id + 1);

            QVector<int> targets = refinementTargetIndices;
            for (int j = r0; j < r1; ++j) {
                // shuffle the evaluation order
                std::random_shuffle(targets.begin(), targets.end());

                QVector<int> bijection = initBijection;

                for (int k = 0; k < targets.size(); ++k) {
                    QVector<QPair<int,float> > candidatesAndEvals = findCandidatesReplaced(targets.at(k), bijection, weightMat, distMat);
                    // select index to be replaced weighted randomly
                    int index = selectIndexFromCandidates(candidatesAndEvals);

                    int tmp = bijection.at(targets.at(k)); // for swap
                    bijection[targets.at(k)] = bijection.at(index);
                    bijection[index] = tmp;
                }

                float e = evalQap(bijection, weightMat, distMat);

                if (e < tmpBestEs.at(id)) {
                    tmpBestEs[id] = e;
                    tmpResults[id] = bijection;
                }
            }
        }, i);
    }
    for (auto& t : worker) t.join();

    // this wastes a time but not big problem (workers are not many)
    for (int i = 0; i < numWorkers; ++i) {
        if (tmpBestEs.at(i) < bestE) {
            bestE = tmpBestEs.at(i);
            result = tmpResults.at(i);
        }
    }
//    high_resolution_clock::time_point t2 = high_resolution_clock::now();
//    duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
//    time_ = time_span.count();
//    qDebug() << "Calc All Random Patterns Time: " << time_;

    // non thread version
//    QVector<int> targets = refinementTargetIndices;
//    for (int i = 0; i < rep; ++i) {
//        qDebug() << "rep " << i << "/" << rep;
//        // shuffle the evaluation order
//        std::random_shuffle(targets.begin(), targets.end());

//        QVector<int> bijection = initBijection;

//        for (int j = 0; j < targets.size(); ++j) {
//            QVector<QPair<int,float> > candidatesAndEvals = findCandidatesReplaced(targets.at(j), bijection, weightMat, distMat);
//            // select index to be replaced weighted randomly
//            int index = selectIndexFromCandidates(candidatesAndEvals);
//            bijection[targets.at(j)] = index;
//            bijection[index] = targets.at(j);
//        }

//        float e = evalQap(bijection, weightMat, distMat);

//        if (e < bestE) {
//            bestE = e;
//            result = bijection;
//        }
//    }

    qDebug() << "result val" << bestE;
    return result;
}

float MappingGuide::evalQap(QVector<int> &bijection, QVector<QVector<float> > &weightMat, QVector<QVector<float> > &distMat)
{
    //high_resolution_clock::time_point t1 = high_resolution_clock::now();

    // TODO: should use map reduce here
    float result = 0;

    for (int i = 0; i < weightMat.size(); ++i) {
        for (int j = 0; j < weightMat.at(i).size(); ++j) {
            int i_ = bijection.at(i);
            int j_ = bijection.at(j);

            float w = weightMat.at(i).at(j);
            float d = distMat.at(i_).at(j_);

            result += w * d;
        }
    }

    //high_resolution_clock::time_point t2 = high_resolution_clock::now();
    //duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
    //time_ = time_span.count();
    //qDebug() << "eval qap time: " << time_;

    return result;
}

float MappingGuide::evalQap(Eigen::VectorXf &bijection, Eigen::MatrixXf &W, Eigen::MatrixXf &D)
{
    return (W * bijection * D * bijection.transpose()).trace();
}

QVector<QPair<int,float> > MappingGuide::findCandidatesReplaced(int indexToBeReplaced, QVector<int> &currentBijection, QVector<QVector<float> > &weightMat, QVector<QVector<float> > &distMat)
{
    QVector<QPair<int,float> > candidateIndicesAndImprovements;

    float currentE = evalQap(currentBijection, weightMat, distMat);

    // TODO: should parallelize here
    for (int i = 0; i < currentBijection.size(); ++i) {
        if (i != indexToBeReplaced) {
            QVector<int> tmpBijection = currentBijection;
            tmpBijection[i] = indexToBeReplaced;
            tmpBijection[indexToBeReplaced] = i;

            float e = evalQap(tmpBijection, weightMat, distMat);

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

bool MappingGuide::evalLessThan(QPair<int,float> candidate1, QPair<int,float> candidate2)
{
    return candidate1.second < candidate2.second;
}

int MappingGuide::selectIndexFromCandidates(QVector<QPair<int,float> > candidatesAndEvals)
{
    // select index randomly reflected their evaluated values
    int retIndex = 0;

    qsrand(QTime::currentTime().msec());

    qSort(candidatesAndEvals.begin(), candidatesAndEvals.end(), evalLessThan);

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

    return candidatesAndEvals.at(retIndex).first;
}

// TODO: change args later after finising change these to RInside version
QVector<int> MappingGuide::suggestMapping(QVector<int> refinementTargetIndices, QString type, QString weightMatFile, QString distMatFile, QString dirForIntermidFiles, QString outputFile, int rep, QString perfOutput)
{
    QVector<int> result;
    QVector<int> currentBijection = genCurrentBijection();
    int numVertices = currentBijection.size();

    QVector<QVector<float> > weightMat;
    QVector<QVector<float> > distMat;

    QFile wFile(weightMatFile);
    QFile dFile(distMatFile);
    if (wFile.open(QIODevice::ReadOnly)) {
        QTextStream ts(&wFile);
        while (!ts.atEnd()) {
            QString line = ts.readLine();
            QStringList elements = line.split(",");
            QVector<float> wMatRow;
            foreach(QString e, elements) {
                wMatRow.push_back(e.toFloat());
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
            QVector<float> dMatRow;
            foreach(QString e, elements) {
                dMatRow.push_back(e.toFloat());
            }
            distMat.push_back(dMatRow);
        }
        dFile.close();
    }

    float curCost = evalQap(currentBijection, weightMat, distMat);
    qDebug() << "Current Cost: " << curCost;
    high_resolution_clock::time_point t1 = high_resolution_clock::now();

    if (type == "all-ma") {
        refinementTargetIndices.clear();
        if (numVertices <= thresForMaPartition || useMaGraphPartition == false) {
            result = memeticQap(weightMatFile, distMatFile, outputFile);     
        } else {
            result = memeticQapWithPartition(numVertices, thresForMaPartition, dirForIntermidFiles, outputFile);
        }
    } else if (type == "selected-optimal") {
        result = refineMappingOptimal(refinementTargetIndices, currentBijection, weightMat, distMat);
    } else if (type == "selected-randomized") {
        result = refineMappingRandomized(refinementTargetIndices, currentBijection, rep, weightMat, distMat);
    }
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
    time_ = time_span.count();

    float resultCost = evalQap(result, weightMat, distMat);
    qDebug() << "selected tyoe: " << type;
    qDebug() << "time for obtain better mapping: " << time_;
    qDebug() << "Result Cost: " << resultCost;

    if (perfOutput != "") {
        QFile outFile(perfOutput);
        bool firstTimeToWrite = false;
        if (!outFile.exists()) firstTimeToWrite = true;

        if (!outFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) qWarning() << "cannot open file: " << perfOutput;
        QTextStream out(&outFile);

        if (firstTimeToWrite) {
            out << "file" << "," << "type" << "," << "n" << "," << "m" << "," << "nPm" << ","
                << "time" << "," << "cost before" << "," << "cost after" << "," << "improve rate" << "\n"; // dividing 2 because metis graph is undirected graph

        }
        out << weightMatFile << "," << type << "," << weightMat.size() << "," << refinementTargetIndices.size() << "," << calcNumOfPermutation(weightMat.size(), refinementTargetIndices.size()) << ","
            << time_ << "," << curCost << "," << resultCost << "," << (1.0 - resultCost / curCost) << "\n"; // dividing 2 because metis graph is undirected graph

        outFile.close();
    }

    return result;
}

QVector<int> MappingGuide::genCurrentBijection()
{
    QVector<int> result;

    foreach(Vertex v, graph->getVertices()) {
        int matColNum = adjMat->coordToColNum( v.getCoord() );
        result.push_back(matColNum);
    }

    return result;
}

double MappingGuide::calcNumOfPermutation(int n, int m)
{
    if (n < m) return 0;

    double result = 1;
    for (int i = 0; i < m; ++i) {
        result *=  n - i;
    }
    return result;
}

void MappingGuide::setUseMaGraphPartition(bool useMaGraphPartition) { this->useMaGraphPartition = useMaGraphPartition;}
