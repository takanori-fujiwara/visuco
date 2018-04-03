#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QFileDialog>
#include <QSettings>

#include "Graph.h"
#include "Color.h"
#include "StatView.h"
#include "AdjMatrix.h"
#include "FormatConverter.h"
#include "MappingGuide.h"
#include "DataDirectorySelectionDialog.h"
#include "VisualEncodingDialog.h"
#include "SettingsDialog.h"
#include "VisualEncodingDialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    Color* color;
    Graph* graph;
    Graph* aggregatedGraph;
    AdjMatrix* adjMat;
    AdjMatrix* aggregatedAdjMat;
    DataDirectorySelectionDialog* dirSelectionDialog;
    SettingsDialog* settingsDialog;
    VisualEncodingDialog* visualEncodingDialog;

    int thresAggregation;
    QVector<int> aggregatingDims;

    int statViewCurrentComboIndex;

    QString baseDir;
    QString pythonPath;
    QString rscriptPath;

    QString selectedApplication;

    void genGraphFiles(QStringList applications, QStringList nodes, QStringList cores, QStringList options, QString edgeFileName = "edges.csv");
    void loadNewData(QString analysisDirPath, QString routesFilePath, QString mappingFilePath, QString hopbytesFilePath, int numCores, int thresForGraphPartition);
    void setupComboBox();
    void setupGraph(QString analysisDirPath = "");
    void setupColor();
    void setupAdjMatrix(QString nodeMappingFilePath = "");
    void setupGraphView();
    void setupMatrixView();
    void setupStatView();
    void loadCurrentSettings();

private slots:
    void updateGraphViewDataComboBoxes(QString selectedApplication);
    void resetupAll(QString analysisDirPath = "");
    void resetupViews();
    void updateBtnsAndInfoForAltRoute(int numSuggestedRoutes, int totalChangeOfLength, float totalChangeOfMaxLoad);
    void radioLengthFirstSelected();
    void radioLoadFirstSelected();
    void updateBtnsAndInfoForAltMapping(int numSuggestedRoutes, int totalChangeOfLength, int totalChangeOfMaxLoad);
    void radioMappingAllSelected();
    void radioMappingSelectedRoutesRandomizedSelected();
    void radioMappingSelectedRoutesOptSelected();
    void checkBoxAllowRemapRelased();
    void setStatViewCurrentComboIndex(int statViewCurrentComboIndex);
    void switchAutoUpdate();
    void changeGraphLayout(QString layout);
    void changeGraphViewLine(QString routeDrawingMode);
    void changeGraphViewLineColor(QString routeColorMetric);
    void changeGraphViewViewMode(QString viewMode);
    void changeMatrixViewLine(QString routeDrawingMode);
    void outputResults();
    bool outputMappingFileFromQapResult(QString qapResultFilePath, QString outMappingFilePath);
    void setMatrixViewGraphDir();
    void on_actionGenerate_Analysis_Data_triggered();
    void on_actionLoad_Analysis_Data_triggered();
    void on_actionShow_Stat_View_triggered();
    void on_actionChange_Visual_Encoding_Settings_triggered();
    void on_actionPreferences_triggered();

signals:
    void resetupAllCalled();
    void resetupViewsCalled();
    void switchSuggestionPriorityTypeCalled(QString radioBtnText);
    void runAutoUpdateCalled();
    void stopAutoUpdateCalled();
    void on_actionShow_Stat_View_triggeredCalled();
};

#endif // MAINWINDOW_H
