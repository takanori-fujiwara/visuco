#include "FormatConverter.h"

FormatConverter::FormatConverter()
{

}

bool FormatConverter::csvGraphMatToMetisGraph(QString csvFilePath, QString metisGraphFilePath)
{
    int numVertices = 0;
    int numEdges = 0;
    QString type = "001";

    QVector<QVector<QString> > indexWeightPairsForAllVertices;

    QFile csvFile(csvFilePath);
    if (csvFile.open(QIODevice::ReadOnly)) {

        // store data in indexWeightPairsForAllVertices
        QTextStream ts(&csvFile);
        while (!ts.atEnd()) {
            QString line = ts.readLine();
            QStringList weights = line.split(",");
            QVector<QString> indexWeightPairs;

            for (int i = 0; i < weights.size(); ++i) {
                QString w = weights.at(i);
                if (w != "0") {
                    indexWeightPairs.push_back(QString::number(i + 1) + " " + QString::number((long)w.toFloat())); // metis graph index starts from 1
                    numEdges++;
                }
            }

            numVertices++;
            indexWeightPairsForAllVertices.push_back(indexWeightPairs);
        }

        // output indexWeightPairsForAllVertices
        QFile graphFile(metisGraphFilePath);
        if (!graphFile.open(QIODevice::WriteOnly | QIODevice::Text)) qWarning() << "cannot open file: " << metisGraphFilePath;
        QTextStream out(&graphFile);

        out << numVertices << " " << numEdges / 2 << " " << type << "\n"; // dividing 2 because metis graph is undirected graph

        int count = 0;
        for (int i = 0; i < indexWeightPairsForAllVertices.size(); ++i) {
            for (int j = 0; j < indexWeightPairsForAllVertices.at(i).size(); ++j) {
                if (j == 0) out << indexWeightPairsForAllVertices.at(i).at(j);
                else out << " " << indexWeightPairsForAllVertices.at(i).at(j);
                count++;
            }
            out << "\n";
        }
        csvFile.close();
    } else {
        return false;
    }

    return true;
}

bool FormatConverter::csvGraphMatToResizedCsvGraphMat(QString inCsvFilePath, QString outputDir, int numPartitions, QString margeMethod, QString partitionFilePath)
{

    if(!QDir(outputDir).exists()) QDir().mkdir(outputDir);
    QFileInfo fi(inCsvFilePath);

    // read original graph
    QFile inCsvFile(inCsvFilePath);
    if (!inCsvFile.open(QIODevice::ReadOnly)) {
        qWarning() << "cannot open file: " << inCsvFilePath;
        return false;
    }

    QVector<QVector<float> > originalGraphMat;

    QTextStream ts2(&inCsvFile);
    while (!ts2.atEnd()) {
        QString line = ts2.readLine();
        QStringList strWeights = line.split(",");
        QVector<float> weights;

        for (int i = 0; i < strWeights.size(); ++i) {
            float w = strWeights.at(i).toFloat();
            weights.push_back(w);
        }

        originalGraphMat.push_back(weights);
    }
    inCsvFile.close();

    int colSizeInResizedMat = originalGraphMat.size() / numPartitions;

    // read partition file
    // original index to resized graph idex
    QVector<int> originalIndexToResizedGraphIndex;
    // resizd graph index to original Indices
    QVector<QVector<int> > resizedGraphIndexToOriginalIndices(numPartitions);

    if (partitionFilePath != "") {
        QFile partitionFile(partitionFilePath);
        if (!partitionFile.open(QIODevice::ReadOnly)) {
            qWarning() << "cannot open file: " << partitionFilePath;
            return false;
        }

        QTextStream ts(&partitionFile);
        int numLines = 0;
        int maxVertexIndex = 0;
        while (!ts.atEnd()) {
            QString line = ts.readLine();
            int vertexIndex = line.toInt(); // index in metis partition starts from 0??? why?
            originalIndexToResizedGraphIndex.push_back(vertexIndex);
            resizedGraphIndexToOriginalIndices[vertexIndex].push_back(numLines);

            if (vertexIndex > maxVertexIndex) maxVertexIndex = vertexIndex;
            ++numLines;
        }
        partitionFile.close();
        maxVertexIndex += 1;

        // ***
        // *** modify partitions to make the completeley same numeber of vertices for each subgraph
        // ***
        int desiredNumVertices = numLines / maxVertexIndex;

        // pop vertices from larger graph than desired
        QVector<int> popedVertices;
        for (int i = 0; i < maxVertexIndex; ++i) {
            while (resizedGraphIndexToOriginalIndices[i].size() > desiredNumVertices) {
                int popedVertex = resizedGraphIndexToOriginalIndices[i].takeLast();
                popedVertices.push_back(popedVertex);
            }
        }

        // push vertices for smaller graph than desired
        for (int i = 0; i < maxVertexIndex; ++i) {
            while (resizedGraphIndexToOriginalIndices[i].size() < desiredNumVertices) {
                int pushingVertex = popedVertices.takeLast();
                resizedGraphIndexToOriginalIndices[i].push_back(pushingVertex);
            }
        }
    } else {
        for (int i = 0; i < numPartitions; ++i) {
            for (int j = 0; j < colSizeInResizedMat; ++j) {
                originalIndexToResizedGraphIndex.push_back(i);
            }
        }
    }

    // output resized graph
    if (margeMethod == "sum") {
        // generate resized graph
        QVector<QVector<float> > resizedGraphMat(numPartitions, QVector<float>(numPartitions, 0.0));

        for (int i = 0; i < originalGraphMat.size(); ++i) {
            for (int j = 0; j < originalGraphMat.at(i).size(); ++j) {
                int i_ = originalIndexToResizedGraphIndex.at(i);
                int j_ = originalIndexToResizedGraphIndex.at(j);

                if (i_ != j_) resizedGraphMat[i_][j_] += originalGraphMat.at(i).at(j);
            }
        }

        // output resized graph
        QFile outCsvFile(outputDir + fi.baseName() + "_resized.csv");
        if (!outCsvFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qWarning() << "cannot open file: " << outputDir + fi.baseName() + "_resized.csv";
            return false;
        }

        QTextStream out(&outCsvFile);

        for (int i = 0; i < resizedGraphMat.size(); ++i) {
            for (int j = 0; j < resizedGraphMat.at(i).size(); ++j) {
                if (j == 0) out << resizedGraphMat.at(i).at(j);
                else out << "," << resizedGraphMat.at(i).at(j);
            }
            out << "\n";
        }
        outCsvFile.close();

    } else if (margeMethod == "average") {
        // generate resized graph
        QVector<QVector<float> > resizedGraphMat(numPartitions, QVector<float>(numPartitions, 0.0));

        for (int i = 0; i < originalGraphMat.size(); ++i) {
            for (int j = 0; j < originalGraphMat.at(i).size(); ++j) {
                int i_ = originalIndexToResizedGraphIndex.at(i);
                int j_ = originalIndexToResizedGraphIndex.at(j);

                if (i_ != j_) resizedGraphMat[i_][j_] += originalGraphMat.at(i).at(j);
            }
        }

        int elementsPerPartition = colSizeInResizedMat * colSizeInResizedMat;

        for (int i = 0; i < resizedGraphMat.size(); ++i) {
            for (int j = 0; j < resizedGraphMat.at(i).size(); ++j) {
                resizedGraphMat[i][j] /= (float)elementsPerPartition;
            }
        }

        // output resized graph
        QFile outCsvFile(outputDir + fi.baseName() + "_resized.csv");
        if (!outCsvFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qWarning() << "cannot open file: " << outputDir + fi.baseName() + "_resized.csv";
            return false;
        }

        QTextStream out(&outCsvFile);

        for (int i = 0; i < resizedGraphMat.size(); ++i) {
            for (int j = 0; j < resizedGraphMat.at(i).size(); ++j) {
                if (j == 0) out << resizedGraphMat.at(i).at(j);
                else out << "," << resizedGraphMat.at(i).at(j);
            }
            out << "\n";
        }
        outCsvFile.close();
    }

    // generate graph within each partition
    QVector<QVector<QVector<int> > > partitionedGraphs;
    for (int i = 0; i < resizedGraphIndexToOriginalIndices.size(); ++i) {
        QVector<int> vIndices = resizedGraphIndexToOriginalIndices.at(i);
        QVector<QVector<int> > partitionedGraph;
        for (int j = 0; j < vIndices.size(); ++j) {
            QVector<int> partitionedGraphRow;
            for (int k = 0; k < vIndices.size(); ++k) {
                int src = vIndices.at(j);
                int target = vIndices.at(k);
                partitionedGraphRow.push_back(originalGraphMat.at(src).at(target));
            }
            partitionedGraph.push_back(partitionedGraphRow);
        }
        partitionedGraphs.push_back(partitionedGraph);
    }

    // output all partitioned graphs
    for (int i = 0; i < partitionedGraphs.size(); ++i) {
        QFile outCsvFile(outputDir + fi.baseName() + "_partitioned" + QString::number(i) + ".csv");
        if (!outCsvFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qWarning() << "cannot open file: " << outputDir + fi.baseName() + "_partitioned" + QString::number(i) + ".csv";
            return false;
        }

        QTextStream out(&outCsvFile);

        for (int j = 0; j < partitionedGraphs.at(i).size(); ++j) {
            for (int k = 0; k < partitionedGraphs.at(i).at(j).size(); ++k) {
                if (k == 0) out << partitionedGraphs.at(i).at(j).at(k);
                else out << "," << partitionedGraphs.at(i).at(j).at(k);
            }
            out << "\n";
        }
        outCsvFile.close();
    }

    // output resizedGraphIndexToOriginalIndices
    QFile outCsvFile(outputDir + fi.baseName() + "_resizedGraphIndexToOriginalIndices.csv");
    if (!outCsvFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "cannot open file: " << outputDir + fi.baseName() + "_resizedGraphIndexToOriginalIndices.csv";
        return false;
    }

    QTextStream out(&outCsvFile);
    foreach(QVector<int> originalIndices, resizedGraphIndexToOriginalIndices) {
        for (int i = 0; i < originalIndices.size(); ++i) {
            if (i == 0) out << originalIndices.at(i);
            else out << "," << originalIndices.at(i);
        }
        out << "\n";
    }
    outCsvFile.close();

    return true;
}
