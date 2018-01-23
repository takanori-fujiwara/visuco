#ifndef FORMATCONVERTER_H
#define FORMATCONVERTER_H

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

class FormatConverter
{
public:
    FormatConverter();
    bool csvGraphMatToMetisGraph(QString csvFilePath, QString metisGraphFilePath);
    bool csvGraphMatToResizedCsvGraphMat(QString inCsvFilePath, QString outputDir, int numPartitions, QString margeMethod, QString partitionFilePath = ""); // method: sum or average
};

#endif // FORMATCONVERTER_H
