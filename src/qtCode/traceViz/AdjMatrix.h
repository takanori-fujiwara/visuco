#ifndef ADJMATRIX_H
#define ADJMATRIX_H

#include <QList>
#include <QtMath>
#include <QDebug>
#include <QFile>
#include <QTextStream>

class AdjMatrix
{
public:
    AdjMatrix(QVector<int> shape = {}, QVector<int> order = {});

    void setShape(QVector<int> shape);
    void setOrder(QVector<int> order);
    void setUnitAdjMat_2(QVector< QVector <int> > unitAdjMat_2);
    void setUnitAdjMat_4(QVector< QVector <int> > unitAdjMat_4);
    void setUnitAdjMat_6(QVector< QVector <int> > unitAdjMat_6);
    void setUnitAdjMat_8(QVector< QVector <int> > unitAdjMat_8);
    void setUnitAdjMat_12(QVector< QVector <int> > unitAdjMat_12);
    void setUnitAdjMat_16(QVector< QVector <int> > unitAdjMat_16);
    QVector< QVector <int> > getAdjMat();
    QVector<int> getShape();
    QVector<int> getOrder();
    int getColSize();
    int getValueAt(int row, int col);
    QVector< QVector <int> > getUnitAdjMat_2();
    QVector< QVector <int> > getUnitAdjMat_4();
    QVector< QVector <int> > getUnitAdjMat_6();
    QVector< QVector <int> > getUnitAdjMat_8();
    QVector< QVector <int> > getUnitAdjMat_12();
    QVector< QVector <int> > getUnitAdjMat_16();

    QVector< QVector <int> > genIdentityMat(int n);
    void resetAdjMat(QVector<int> shape, QVector<int> order);
    void printAdjMat();
    int coordToColNum(QVector<int> coord);
    QVector<int> colNumToCoord(int colNum);

    void outputAdjMatInCsv(QString csvFilePath);

private:
    QString topologyMode;
    QVector< QVector <int> > adjMat;
    QVector<int> shape;
    QVector<int> order;
    QVector< QVector <int> > unitAdjMat_2;
    QVector< QVector <int> > unitAdjMat_4;
    QVector< QVector <int> > unitAdjMat_8;
    QVector< QVector <int> > unitAdjMat_16;
    QVector< QVector <int> > unitAdjMat_6; // for theta
    QVector< QVector <int> > unitAdjMat_12; // for theta

    void genAdjMat(QVector<int> shape, QVector<int> order);
    void genAdjMat(QList< QVector< QVector <int> > > unitAdjMatrices);
};

#endif // ADJMATRIX_H
