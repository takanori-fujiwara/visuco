#include "AdjMatrix.h"

AdjMatrix::AdjMatrix(QVector<int> shape, QVector<int> order)
{
    topologyMode = "theta"; // 5dtorus or theta

    setUnitAdjMatrices(topologyMode);
    genAdjMat(shape, order);
    this->shape = shape;
    this->order = order;
}

void AdjMatrix::setUnitAdjMatrices(QString topologyMode)
{
    if (topologyMode == "5dtorus") {
        unitAdjMat_2.clear();
        unitAdjMat_2
                << QVector<int>{0, 1}
                << QVector<int>{1, 0};
        unitAdjMat_4.clear();
        unitAdjMat_4
                << QVector<int>{0, 1, 0, 1}
                << QVector<int>{1, 0, 1, 0}
                << QVector<int>{0, 1, 0, 1}
                << QVector<int>{1, 0, 1, 0};
        unitAdjMat_8.clear();
        unitAdjMat_8
                << QVector<int>{0, 1, 0, 0, 0, 0, 0, 1}
                << QVector<int>{1, 0, 1, 0, 0, 0, 0, 0}
                << QVector<int>{0, 1, 0, 1, 0, 0, 0, 0}
                << QVector<int>{0, 0, 1, 0, 1, 0, 0, 0}
                << QVector<int>{0, 0, 0, 1, 0, 1, 0 ,0}
                << QVector<int>{0, 0, 0, 0, 1, 0, 1, 0}
                << QVector<int>{0, 0, 0, 0, 0, 1, 0, 1}
                << QVector<int>{1, 0, 0, 0, 0, 0, 1, 0};
        unitAdjMat_16.clear();
        unitAdjMat_16
                << QVector<int>{0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}
                << QVector<int>{1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
                << QVector<int>{0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
                << QVector<int>{0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
                << QVector<int>{0, 0, 0, 1, 0, 1, 0 ,0, 0, 0, 0, 0, 0, 0, 0, 0}
                << QVector<int>{0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}
                << QVector<int>{0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}
                << QVector<int>{0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0}
                << QVector<int>{0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0}
                << QVector<int>{0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0}
                << QVector<int>{0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0}
                << QVector<int>{0, 0, 0, 0, 0, 0, 0 ,0, 0, 0, 1, 0, 1, 0, 0, 0}
                << QVector<int>{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0}
                << QVector<int>{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0}
                << QVector<int>{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1}
                << QVector<int>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0};
    } else if (topologyMode == "theta") {
    // For Theta
        unitAdjMat_2.clear();
        unitAdjMat_2
                << QVector<int>{0, 1}
                << QVector<int>{1, 0};
        unitAdjMat_3.clear();
        unitAdjMat_3
                << QVector<int>{0, 1, 1}
                << QVector<int>{1, 0, 1}
                << QVector<int>{1, 1, 0};
        unitAdjMat_4.clear();
        unitAdjMat_4
                << QVector<int>{0, 1, 1, 1}
                << QVector<int>{1, 0, 1, 1}
                << QVector<int>{1, 1, 0, 1}
                << QVector<int>{1, 1, 1, 0};
        unitAdjMat_16.clear();
        unitAdjMat_16
                << QVector<int>{0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
                << QVector<int>{1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
                << QVector<int>{1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
                << QVector<int>{1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
                << QVector<int>{1, 1, 1, 1, 0, 1, 1 ,1, 1, 1, 1, 1, 1, 1, 1, 1}
                << QVector<int>{1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
                << QVector<int>{1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1}
                << QVector<int>{1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1}
                << QVector<int>{1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1}
                << QVector<int>{1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1}
                << QVector<int>{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1}
                << QVector<int>{1, 1, 1, 1, 1, 1, 1 ,1, 1, 1, 1, 0, 1, 1, 1, 1}
                << QVector<int>{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1}
                << QVector<int>{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1}
                << QVector<int>{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1}
                << QVector<int>{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0};
        unitAdjMat_6.clear();
        unitAdjMat_6
                << QVector<int>{0, 1, 1, 1, 1, 1}
                << QVector<int>{1, 0, 1, 1, 1, 1}
                << QVector<int>{1, 1, 0, 1, 1, 1}
                << QVector<int>{1, 1, 1, 0, 1, 1}
                << QVector<int>{1, 1, 1, 1, 0, 1}
                << QVector<int>{1, 1, 1, 1, 1, 0};
        unitAdjMat_8.clear();
        unitAdjMat_8
                << QVector<int>{0, 1, 1, 1, 1, 1, 1, 1}
                << QVector<int>{1, 0, 1, 1, 1, 1, 1, 1}
                << QVector<int>{1, 1, 0, 1, 1, 1, 1, 1}
                << QVector<int>{1, 1, 1, 0, 1, 1, 1, 1}
                << QVector<int>{1, 1, 1, 1, 0, 1, 1 ,1}
                << QVector<int>{1, 1, 1, 1, 1, 0, 1, 1}
                << QVector<int>{1, 1, 1, 1, 1, 1, 0, 1}
                << QVector<int>{1, 1, 1, 1, 1, 1, 1, 0};
        unitAdjMat_12.clear();
        unitAdjMat_12
                << QVector<int>{0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
                << QVector<int>{1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
                << QVector<int>{1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1}
                << QVector<int>{1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1}
                << QVector<int>{1, 1, 1, 1, 0, 1, 1 ,1, 1, 1, 1, 1}
                << QVector<int>{1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1}
                << QVector<int>{1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1}
                << QVector<int>{1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1}
                << QVector<int>{1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1}
                << QVector<int>{1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1}
                << QVector<int>{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1}
                << QVector<int>{1, 1, 1, 1, 1, 1, 1 ,1, 1, 1, 1, 0};
    }
}

void AdjMatrix::resetAdjMat(QVector<int> shape, QVector<int> order, QString topologyMode)
{
    setUnitAdjMatrices(topologyMode);
    genAdjMat(shape, order);
    this->shape = shape;
    this->order = order;
    this->topologyMode = topologyMode;
}

QVector< QVector <int> > AdjMatrix::genIdentityMat(int n)
{
    QVector< QVector <int> > result(n, QVector<int>(n));

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i == j) result[i][j] = 1;
            else result[i][j] = 0;
        }
    }

    return result;
}

void AdjMatrix::genAdjMat(QVector<int> shape, QVector<int> order)
{
    QList< QVector< QVector <int> > > unitAdjMatrices;
    QVector<int> dimensions;
    foreach(int i, order) {
        dimensions.push_back(shape.at(i));
    }

    for (int i = 0; i < dimensions.size(); ++i) {
        switch (dimensions.at(i)) {
        case 2:
            unitAdjMatrices.push_back(unitAdjMat_2);
            break;
        case 3:
            unitAdjMatrices.push_back(unitAdjMat_3);
            break;
        case 4:
            unitAdjMatrices.push_back(unitAdjMat_4);
            break;
        case 6:
            unitAdjMatrices.push_back(unitAdjMat_6);
            break;
        case 8:
            unitAdjMatrices.push_back(unitAdjMat_8);
            break;
        case 12:
            unitAdjMatrices.push_back(unitAdjMat_12);
            break;
        case 16:
            unitAdjMatrices.push_back(unitAdjMat_16);
            break;
        default:
            qWarning() << "indicated dimension" << dimensions.at(i) << " for genAdj is not supported";
        }
    }
    genAdjMat(unitAdjMatrices);
}

// TODO: need to change later for fast version
void AdjMatrix::genAdjMat(QList< QVector< QVector <int> > > unitAdjMatrices)
{
    if (unitAdjMatrices.size() == 0) {
        qWarning() << "cannot generate adj matrix because of unit adj matrices size is 0";
    } else if (unitAdjMatrices.size() > 1) {
        QVector< QVector <int> > firstMat = unitAdjMatrices.at(0);
        QVector< QVector <int> > secondMat = unitAdjMatrices.at(1);
        int newMatSize = firstMat.size() * secondMat.size();
        QVector< QVector <int> > tmpMat(newMatSize, QVector<int>(newMatSize, 0));

        for (int i = 0; i < secondMat.size(); ++i) {
            for (int j = 0; j < secondMat.at(i).size(); ++j) {
                if (i == j) {
                    // fill with unit mat
                    for (int k = 0; k < firstMat.size(); ++k) {
                        for (int l = 0; l < firstMat.at(k).size(); ++l) {
                            int x = i * firstMat.size() + k;
                            int y = j * firstMat.size() + l;
                            tmpMat[x][y] = firstMat[k][l];
                        }
                    }
                } else if (secondMat.at(i).at(j) == 1){
                    // fill with identity mat
                    QVector< QVector<int> > idenMat = genIdentityMat(firstMat.size());
                    for (int k = 0; k < idenMat.size(); ++k) {
                        for (int l = 0; l < idenMat.at(k).size(); ++l) {
                            int x = i * idenMat.size() + k;
                            int y = j * idenMat.size() + l;
                            tmpMat[x][y] = idenMat[k][l];
                        }
                    }
                } else {
                    // filled with 0
                    // when initializing tmpMat, all elements were set with 0.
                }
            }
        }

        // to recursive call
        unitAdjMatrices[1] = tmpMat;
        unitAdjMatrices.pop_front();
        genAdjMat(unitAdjMatrices);

    } else {
        // TODO: not clear why this process is needed to copy the qvector properly
        adjMat.clear();
        QVector< QVector <int> > firstMat = unitAdjMatrices.at(0);

        for (int i = 0; i < firstMat.size(); ++i) {
            QVector<int> oneRowMat;
            for (int j = 0; j < firstMat.size(); ++j) {
                oneRowMat.push_back(firstMat.at(i).at(j));
            }
            adjMat.push_back(oneRowMat);
        }
    }
}

void AdjMatrix::printAdjMat()
{
    QDebug deb = qDebug();
    deb.nospace();
    for (int i = 0; i < adjMat.size(); ++i) {
        for (int j = 0; j < adjMat.at(i).size(); ++j) {
            if(j > 0) deb << ", ";
            deb << adjMat.at(i).at(j);
        }
        deb << "\n";
    }
}

int AdjMatrix::coordToColNum(QVector<int> coord)
{
    int result = 0;
    int inc = 1;
    foreach(int i, order) {
        result += inc * coord.at(i);
        inc *= shape.at(i);
    }

    return result;
}

QVector<int> AdjMatrix::colNumToCoord(int colNum)
{
    QVector<int> result(shape.size(), 0);
    int divNum = 1;
    for (int i = 0; i < order.size() - 1; ++i) {
        divNum *= shape.at(order.at(i));
    }

    for(int i = 0; i < order.size() - 1; ++i) {
        int o1 = order.at(order.size() - 1 - i);
        int o2 = order.at(order.size() - 2 - i);
        int quot = colNum / divNum;
        result[o1] = quot;

        colNum -= quot * divNum;
        divNum /= shape.at(o2);
    }
    result[order.at(0)] = colNum;

    return result;
}


void AdjMatrix::outputAdjMatInCsv(QString csvFilePath)
{
    QFile file(csvFilePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) qWarning() << "cannot open file: " << csvFilePath;
    QTextStream out(&file);

    for (int i = 0; i < adjMat.size(); ++i) {
        for (int j = 0; j < adjMat.at(i).size(); ++j) {
            if (j == 0) out << adjMat.at(i).at(j);
            else out << "," << adjMat.at(i).at(j);
        }
        out << "\n";
    }
}

void AdjMatrix::setShape(QVector<int> shape) { this->shape = shape; }
void AdjMatrix::setOrder(QVector<int> order) { this->order = order; }
void AdjMatrix::setUnitAdjMat_2(QVector< QVector <int> > unitAdjMat_2) { this->unitAdjMat_2 = unitAdjMat_2; }
void AdjMatrix::setUnitAdjMat_3(QVector< QVector <int> > unitAdjMat_3) { this->unitAdjMat_3 = unitAdjMat_3; }
void AdjMatrix::setUnitAdjMat_4(QVector< QVector <int> > unitAdjMat_4) { this->unitAdjMat_4 = unitAdjMat_4; }
void AdjMatrix::setUnitAdjMat_6(QVector< QVector <int> > unitAdjMat_6) { this->unitAdjMat_6 = unitAdjMat_6; }
void AdjMatrix::setUnitAdjMat_8(QVector< QVector <int> > unitAdjMat_8) { this->unitAdjMat_8 = unitAdjMat_8; }
void AdjMatrix::setUnitAdjMat_12(QVector< QVector <int> > unitAdjMat_12) { this->unitAdjMat_12 = unitAdjMat_12; }
void AdjMatrix::setUnitAdjMat_16(QVector< QVector <int> > unitAdjMat_16) { this->unitAdjMat_16 = unitAdjMat_16; }

QVector< QVector <int> > AdjMatrix::getAdjMat() { return adjMat; }
QVector<int> AdjMatrix::getShape() { return shape; }
QVector<int> AdjMatrix::getOrder() { return order; }
int AdjMatrix::getColSize() { return adjMat.size(); }
int AdjMatrix::getValueAt(int row, int col) { return adjMat.at(row).at(col); }
QVector< QVector <int> > AdjMatrix::getUnitAdjMat_2() { return unitAdjMat_2; }
QVector< QVector <int> > AdjMatrix::getUnitAdjMat_3() { return unitAdjMat_3; }
QVector< QVector <int> > AdjMatrix::getUnitAdjMat_4() { return unitAdjMat_4; }
QVector< QVector <int> > AdjMatrix::getUnitAdjMat_6() { return unitAdjMat_6; }
QVector< QVector <int> > AdjMatrix::getUnitAdjMat_8() { return unitAdjMat_8; }
QVector< QVector <int> > AdjMatrix::getUnitAdjMat_12() { return unitAdjMat_12; }
QVector< QVector <int> > AdjMatrix::getUnitAdjMat_16() { return unitAdjMat_16; }

