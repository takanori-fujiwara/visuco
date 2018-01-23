#ifndef DATADIRECTORYSELECTIONDIALOG_H
#define DATADIRECTORYSELECTIONDIALOG_H

#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QSpacerItem>
#include <QIntValidator>
#include <QDebug>
#include <QSettings>

class DataDirectorySelectionDialog : public QDialog
{
    Q_OBJECT
public:
    explicit DataDirectorySelectionDialog(QWidget *parent = 0);
    ~DataDirectorySelectionDialog();

    QString routeFile;
    QString matrixMappingFile;
    QString hopByteFile;
    int numNodes;
    int numCores;
    int thresForGraphPartition;
    QString outputDir;
    bool isOk;

private:
    QLineEdit *routeFileLineEdit;
    QLineEdit *matrixMappingFileLineEdit;
    QLineEdit *hopByteFileLineEdit;
    QComboBox *topologyTypeComboBox;
    QLineEdit *numNodesLineEdit;
    QLineEdit *numCoresLineEdit;
    QLineEdit *thresForGraphPartitionLineEdit;
    QLineEdit *outputDirLineEdit;

    void setCurrentSettings();

private slots:
    void setOutputDir();
    void setRouteFile();
    void setMatrixMappingFile();
    void setHopByteFile();
    void handleOk();
    void saveSettings();

signals:
};

#endif // DATADIRECTORYSELECTIONDIALOG_H
