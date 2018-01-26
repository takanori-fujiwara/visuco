#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ///// For Theta, need to change below
    // MainWindow.cpp: aggregatingDims = {3} or {3, 4}; // For Theta
    // AdjMatrix.cpp: change unit_adj
    // Graph.cpp: allocatingUnusedCoords = false; // For Theta

    ui->setupUi(this);
    setWindowTitle("Communication View & Detailed Route View");
    thresAggregation = 256;
    aggregatingDims = {3,4};
//    aggregatingDims = {3}; // For Theta (previous one)
    qSort(aggregatingDims.begin(), aggregatingDims.end(), qGreater<int>());

    pythonPath = "/usr/local/bin/python2";
    rscriptPath = "/usr/local/bin/Rscript";
    loadCurrentSettings();

    // This can be used for generating analysis data automatically
    if (false) {
        QStringList apps = {"miniAMR"};
        QStringList nodes = {"64"};
        QStringList cores = {"1"};
        QStringList options = {"_s2"};
        int thresForGraphPartition = 256;
        foreach(QString a, apps) {
            foreach(QString n, nodes) {
                foreach(QString c, cores) {
                    foreach(QString o, options) {
                        QString name_full = a + "_n" + n + "_c" + c  + o;
                        QString name_short = a + "_n" + n;
                        QString dataDir = qApp->applicationDirPath() + "/../../../../../../data/";
                        QString analysisDirPath = dataDir + "graph/" + name_full + "/";
                        QString routeFilePath =  dataDir + "route/" + name_full + "_routes.txt";
                        //QString routeFilePath =  dataDir + "route/old_ver2/" + name + "_routes.txt";
                        QString mappingFilePath =  dataDir + "mapping/" + name_short + "_mapping.csv";
                        QString hopByteFilePath = dataDir + "hopbyte/" + name_full + "_hopbytes.txt";//"";
                        int numCores = c.toInt();
                        loadNewData(analysisDirPath, routeFilePath, mappingFilePath, hopByteFilePath, numCores, thresForGraphPartition);
                    }
                }
            }
        }
    }

    setupComboBox();
    color = new Color;
    graph = new Graph;
    aggregatedGraph = new Graph;

    adjMat = new AdjMatrix;
    aggregatedAdjMat = new AdjMatrix;
    setupColor();

    setupGraphView();
    setupMatrixView();
    statViewCurrentComboIndex = 0;

    connect(ui->comboGraphViewLineColor, SIGNAL(currentTextChanged(QString)), this, SLOT(changeGraphViewLineColor(QString)));
    connect(ui->comboGraphViewMouseMode, SIGNAL(currentTextChanged(QString)), ui->graphView, SLOT(setMouseMode(QString)));
    connect(ui->comboMatrixViewLine, SIGNAL(currentTextChanged(QString)), this, SLOT(changeMatrixViewLine(QString)));
    connect(ui->comboMatrixViewMouseMode, SIGNAL(currentTextChanged(QString)), ui->matrixView, SLOT(setMouseMode(QString)));
    connect(ui->actionOutput_Current_Displayed_Analysis_Data, SIGNAL(triggered()), this, SLOT(outputResults()));

    connect(ui->btnShowAltRoute, SIGNAL(released()), ui->matrixView, SLOT(suggestAltRoutesForDisplayedRoutes()));
    connect(ui->btnShowAltMapping, SIGNAL(released()), this, SLOT(setMatrixViewGraphDir()));
    connect(ui->btnShowAltMapping, SIGNAL(released()), ui->matrixView, SLOT(suggestAltMapping()));
    connect(ui->matrixView, SIGNAL(suggestAltRoutesForDisplayedRoutesCalled(int, int, float)), this, SLOT(updateBtnsAndInfoForAltRoute(int, int, float)));
    connect(ui->btnApplyAltRoute, SIGNAL(released()), ui->matrixView, SLOT(applyAltRoutes()));
    connect(ui->radioLengthFirst, SIGNAL(released()), this, SLOT(radioLengthFirstSelected()));
    connect(ui->radioLoadFirst, SIGNAL(released()), this, SLOT(radioLoadFirstSelected()));
    connect(ui->radioMappingAll, SIGNAL(released()), this, SLOT(radioMappingAllSelected()));
    connect(ui->radioMappingSelectedRoutesRandomized, SIGNAL(released()), this, SLOT(radioMappingSelectedRoutesRandomizedSelected()));
    connect(ui->radioMappingSelectedRoutesOpt, SIGNAL(released()), this, SLOT(radioMappingSelectedRoutesOptSelected()));
    connect(ui->btnAutoUpdate, SIGNAL(released()), this, SLOT(switchAutoUpdate()));
    connect(this, SIGNAL(runAutoUpdateCalled()), ui->matrixView, SLOT(turnOnAutoUpdate()));
    connect(this, SIGNAL(stopAutoUpdateCalled()), ui->matrixView, SLOT(turnOffAutoUpdate()));

    connect(ui->graphView, SIGNAL(displaySelectedElementsInMatrixViewCalled(QString,QVector<int>)),
            ui->matrixView, SLOT(updateDisplayedElementsIndicies(QString,QVector<int>)));
    connect(ui->graphView, SIGNAL(resetSettingsCalled()), ui->matrixView, SLOT(resetSettings()));
    connect(ui->matrixView, SIGNAL(applyAltRoutesCalled()), this, SLOT(resetupViews()));
    connect(ui->graphView, SIGNAL(routeColorSettingsChanged(QString,float,float)),
            ui->matrixView, SLOT(changeRouteColorMapSettings(QString,float,float)));
    connect(ui->graphView, SIGNAL(switchGraphCalled()), ui->matrixView, SLOT(switchGraphAndAdjMat()));

    // This can be used for generating qap result automatically
//    QStringList apps = {"miniAMR"};
//    QStringList nodes = {"2048"};
//    QStringList cores = {"2"};
//    QStringList options = {"_s2"};
//    foreach(QString a, apps) {
//        foreach(QString n, nodes) {
//            foreach(QString c, cores) {
//                foreach(QString o, options) {
//                    QString name = a + "_n" + n + "_c" + c  + o;
//                    QString dataDir = qApp->applicationDirPath() + "/../../../../../../data/";
//                    QString analysisDirPath = dataDir + "graph/" + name + "/";
//                    resetupAll(analysisDirPath);
//                    ui->matrixView->setGraphDir(analysisDirPath);
//                    ui->matrixView->suggestAltMapping();
//                    //mappingGuide.suggestMapping(QVector<int>({}), "all-ma", analysisDirPath + "route_weight_mat.csv", analysisDirPath + "dist_mat.csv",  analysisDirPath + "qap_intermed/", analysisDirPath + "qapresult.txt");
//                    outputMappingFileFromQapResult(analysisDirPath + "qapresult.txt", "/Users/fujiwara/Desktop/mapping/mapping_n" + n + "_c" + c + o + ".txt");
//                }
//            }
//        }
//    }

    ///
    /// This can be used for generating qap result automatically and output
    ///
    if (false) {
        QString type = "all-ma-gp"; // "all-ma", "all-ma-gp", "selected-optimal", "selected-randomized"
        QString perfOutput = qApp->applicationDirPath() + "/../../../../../../data/perfResults/" + type + "_ours.csv";
        QStringList apps = {"miniMD"};
        QStringList nodes = {"512","1024","2048"};
        QStringList cores = {"1","4","16"};
        QStringList options = {"_w"};
        int repeat = 1;
        QVector<QVector<int> > replacedTargetVertices = {{0},{0,1},{0,1,2},{0,1,2,3},{0,1,2,3,4},{0,1,2,3,4,5}};//,{0,1,2,3,4,5,6},{0,1,2,3,4,5,6,7}};
        foreach(QString a, apps) {
            foreach(QString n, nodes) {
                foreach(QString c, cores) {
                    foreach(QString o, options) {
                        QString name = a + "_n" + n + "_c" + c  + o;
                        QString dataDir = qApp->applicationDirPath() + "/../../../../../../data/";
                        QString analysisDirPath = dataDir + "graph/" + name + "/";
                        resetupAll(analysisDirPath);
                        ui->matrixView->setGraphDir(analysisDirPath);
                        if (type == "all-ma") {
                            for (int i = 0 ; i < repeat; ++i) {
                                ui->matrixView->setMappingGuideType(type);
                                ui->matrixView->suggestAltMapping(perfOutput, false);
                            }
                        } else if (type == "all-ma-gp") {
                            for (int i = 0 ; i < repeat; ++i) {
                                ui->matrixView->setMappingGuideType("all-ma");
                                ui->matrixView->suggestAltMapping(perfOutput, true);
                            }
                        } else if (type == "selected-optimal") {
                            ui->matrixView->setMappingGuideType(type);
                            foreach(QVector<int> vertices, replacedTargetVertices) {
                                for (int i = 0 ; i < repeat; ++i) {
                                    ui->matrixView->setDisplayedVertexIndices(vertices);
                                    ui->matrixView->suggestAltMapping(perfOutput, false);
                                }
                            }
                        } else if (type == "selected-randomized") {
                            ui->matrixView->setMappingGuideType(type);
                            foreach(QVector<int> vertices, replacedTargetVertices) {
                                for (int i = 0 ; i < repeat; ++i) {
                                    ui->matrixView->setDisplayedVertexIndices(vertices);
                                    ui->matrixView->suggestAltMapping(perfOutput, false);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupStatView()
{
    StatView* statView = new StatView(this);
    statView->setCurrentComboBoxIndex(statViewCurrentComboIndex);
    statView->setColor(color);
    statView->setGraph(graph);
    statView->setAggregatedGraph(aggregatedGraph);
    if (graph->getNumVertices() < thresAggregation) {
        statView->setDisplayedGraph(graph);
    } else {
        statView->setDisplayedGraph(aggregatedGraph);
    }
    statView->updateAllDrawingInfo();
    statView->move(0, 0);
    statView->show();
    statView->raise();
    statView->activateWindow();

    connect(statView, SIGNAL(displaySelectedElementsInOtherViewsCalled(QString,QVector<int>)),
            ui->graphView, SLOT(updateDisplayedElementsIndicies(QString,QVector<int>)));
    connect(statView, SIGNAL(currentComboBoxIndexChanged(int)), this, SLOT(setStatViewCurrentComboIndex(int)));
    connect(statView, SIGNAL(colorChanged()), ui->graphView, SLOT(updateColorsFromStatView()));
    connect(statView, SIGNAL(colorChanged()), ui->matrixView, SLOT(updateColorsFromStatView()));
    connect(this, SIGNAL(resetupAllCalled()), statView, SLOT(close()));
    connect(this, SIGNAL(resetupViewsCalled()), statView, SLOT(close()));
    connect(this, SIGNAL(on_actionShow_Stat_View_triggeredCalled()), statView, SLOT(close()));
    connect(ui->graphView, SIGNAL(routeColorSettingsChanged(QString,float,float)), statView, SLOT(changeRouteColorMapSettings(QString,float,float)));
    connect(ui->graphView, SIGNAL(switchGraphCalled()), statView, SLOT(switchGraph()));
}

void MainWindow::on_actionShow_Stat_View_triggered()
{
    emit on_actionShow_Stat_View_triggeredCalled();
    setupStatView();
}

void MainWindow::on_actionChange_Visual_Encoding_Settings_triggered()
{
    visualEncodingDialog = new VisualEncodingDialog;
    visualEncodingDialog->exec();

    if (visualEncodingDialog->isOk) {
        float nodeSizeWeight = visualEncodingDialog->nodeSizeWeight;
        float nodeSizeMin = visualEncodingDialog->nodeSizeMin;
        float edgeWidthWeight = visualEncodingDialog->edgeWidthWeight;
        float routeWidthWeight = visualEncodingDialog->routeWidthWeight;
        float selfLoopRouteRatio = visualEncodingDialog->selfLoopRouteRatio;
        float edgeWidthMatrixView = visualEncodingDialog->edgeWidthMatrixView;
        float routeWidthMatrixView = visualEncodingDialog->routeWidthMatrixView;
        ui->graphView->updateVisualEncodingParams(nodeSizeWeight, nodeSizeMin, edgeWidthWeight, routeWidthWeight, selfLoopRouteRatio);
        ui->matrixView->updateVisualEncodingParams(edgeWidthMatrixView, routeWidthMatrixView);
    }
}

void MainWindow::on_actionGenerate_Analysis_Data_triggered()
{
    dirSelectionDialog = new DataDirectorySelectionDialog;
    dirSelectionDialog->exec();

    if (dirSelectionDialog->isOk) {
        QString routeFilePath = dirSelectionDialog->routeFile;
        QString mappingFilePath = dirSelectionDialog->matrixMappingFile;
        QString hopByteFilePath = dirSelectionDialog->hopByteFile;
        int numCores = dirSelectionDialog->numCores;
        int thresForGraphPartition = dirSelectionDialog->thresForGraphPartition;
        QString analysisDirPath = dirSelectionDialog->outputDir;

        loadNewData(analysisDirPath, routeFilePath, mappingFilePath, hopByteFilePath, numCores, thresForGraphPartition);
    }
}

void MainWindow::on_actionLoad_Analysis_Data_triggered()
{
    QString analysisDirPath = "";

    QSettings settings("ANL_VIDI", "Visuco");
    if (settings.value("mainWindow/analysisDirPath").isValid()) {
        analysisDirPath = settings.value("mainWindow/analysisDirPath").toString();
    }

    QString tempAnalysisDirPath = QFileDialog::getExistingDirectory(this, tr("Open Directory"), analysisDirPath,
                                                                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (!tempAnalysisDirPath.isEmpty() && !tempAnalysisDirPath.isNull()) {
        analysisDirPath = tempAnalysisDirPath + "/";
        settings.setValue("mainWindow/analysisDirPath", analysisDirPath);
        resetupAll(analysisDirPath);
    }
}

void MainWindow::on_actionPreferences_triggered()
{
    settingsDialog = new SettingsDialog;
    settingsDialog->exec();

    if (settingsDialog->isOk) {
        pythonPath = settingsDialog->pythonPath;
        rscriptPath = settingsDialog->rscriptPath;
    }
}

void MainWindow::loadNewData(QString analysisDirPath, QString routesFilePath, QString mappingFilePath, QString hopbytesFilePath, int numCores, int thresForGraphPartition)
{
    bool aLotOfSubGraphs = false;
    bool graphPartitionByAdj = true;

    if (!QDir(analysisDirPath).exists()) QDir().mkdir(analysisDirPath);

    QVector<int> shape;
    QVector<int> order;
    QString topologyType;

    if (mappingFilePath != "") {
        // load from mapping file
        QFile mappingFile(mappingFilePath);
        if (mappingFile.open(QIODevice::ReadOnly)) {
            QTextStream ts(&mappingFile);
            int lineNo = 0;
            while (!ts.atEnd()) {
                QString line = ts.readLine();
                QStringList elements = line.split(" ");
                if (lineNo == 0) {
                    foreach(QString elem, elements) {
                        shape.push_back(elem.toInt());
                    }
                }
                if (lineNo == 1) {
                    foreach(QString elem, elements) {
                        order.push_back(elem.toInt());
                    }
                }
                if (lineNo == 2) {
                    topologyType = elements[0];
                }
                lineNo++;
            }
        }
        if (shape.size() != order.size()) qWarning() << "shape and order size is not same";
        if (QFile::exists(analysisDirPath + "mapping.csv")) QFile::remove(analysisDirPath + "mapping.csv");
        QFile::copy(mappingFilePath, analysisDirPath + "mapping.csv");
    } else {
        // TODO: automtically extracted from route file
    }


    // output graph data
    Graph* tmpGraph = new Graph;
    tmpGraph->loadFromRoutesFile(routesFilePath, hopbytesFilePath, numCores, shape, order, analysisDirPath, topologyType); // this produces graph files
    Graph* tmpAggregatedGraph = new Graph;
    tmpAggregatedGraph->loadFromRoutesFile(routesFilePath, hopbytesFilePath, numCores, shape, order, analysisDirPath, topologyType, aggregatingDims); // this produces graph files

    // output all child indices to calculate positions in aggregated graph
    QVector<QVector<int>> vecChildIndices;
    foreach (Vertex av, tmpAggregatedGraph->getVertices()) {
        QVector<int> childIndices;
        foreach (QVector<int> childCoord, av.getChildCoords()) {
            int childIndex = tmpGraph->getVertexIndexWithCoord(childCoord);
            childIndices.push_back(childIndex);
        }
        vecChildIndices.push_back(childIndices);
    }

    QString childVertexIndicesFile = analysisDirPath + "child_vertex_indices.csv";
    QFile file(childVertexIndicesFile);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) qWarning() << "cannot open file: " << childVertexIndicesFile;
    QTextStream out(&file);
    foreach (QVector<int> childIndices, vecChildIndices) {
        QString outLine;
        foreach (int index, childIndices) {
            outLine += QString::number(index) + ",";
        }
        outLine.chop(1);
        out << outLine << "\n";
    }
    file.close();

    int numOfVertices = tmpGraph->getNumVertices();
    delete tmpGraph;
    delete tmpAggregatedGraph;

    QString scriptFile;
    QString command;

    // output layout
    QString edgeFile = analysisDirPath + "edges.csv";
    QString vertexFile = analysisDirPath + "vertices.csv";
    QString aEdgeFile = analysisDirPath + "aggregated_edges.csv";
    QString aVertexFile = analysisDirPath + "aggregated_vertices.csv";

    scriptFile = qApp->applicationDirPath() + "/../../../../../../src/python/visualization/draw_graph_with_aggregation.py";

    if (numOfVertices < thresAggregation) {
        command = pythonPath + " " + scriptFile + " -ef " + edgeFile + " -vf " + vertexFile +
                " -p true " + " -aef " + aEdgeFile + " -avf " + aVertexFile + " -cf " + childVertexIndicesFile + " -o " + analysisDirPath + " -i false -l sfdpw";
    } else {
        command = pythonPath + " " + scriptFile + " -ef " + edgeFile + " -vf " + vertexFile +
                " -p false " + " -aef " + aEdgeFile + " -avf " + aVertexFile + " -cf " + childVertexIndicesFile + " -o " + analysisDirPath + " -i false -l sfdpw";
    }

    if(!QProcess::startDetached(command)){
        qCritical() << "Cannot start python program";
    }

    // output adjmatrix and distance matrix
    AdjMatrix* tmpAdjMat = new AdjMatrix;
    tmpAdjMat->resetAdjMat(shape, order, topologyType);
    tmpAdjMat->outputAdjMatInCsv(analysisDirPath + "adj_mat.csv");
    delete tmpAdjMat;

    scriptFile = qApp->applicationDirPath() + "/../../../../../../src/r/matrixProcessing/adjMatToDistMat.R";
    command = rscriptPath + " " + scriptFile + " " + analysisDirPath + "adj_mat.csv";

    if (QProcess::execute(command) < 0) {
        qCritical() << "Cannot start R program";
    }

    // output graphs for metis
    FormatConverter fc;
    fc.csvGraphMatToMetisGraph(analysisDirPath + "dist_mat.csv", analysisDirPath + "dist.graph");
    fc.csvGraphMatToMetisGraph(analysisDirPath + "adj_mat.csv", analysisDirPath + "adj.graph");
    fc.csvGraphMatToMetisGraph(analysisDirPath + "route_weight_mat.csv", analysisDirPath + "route_weight.graph");

    int numOfSubGraphs = thresForGraphPartition;
    if (!aLotOfSubGraphs) numOfSubGraphs = numOfVertices / thresForGraphPartition;

    // output partition files by Metis
    if (numOfVertices > thresForGraphPartition) {
        // if (numOfVertices == thresForGraphPartition * 2) thresForGraphPartition /= 2; // when each partition has only 2 nodes, it doesn't work well (rule of thumb).

        ///
        /// THIS method used in existing method is not good. because every distance will be the same.
        /// We partition distance graph based on its base adjacency matrix
        ///
        if (!graphPartitionByAdj) {
            command = qApp->applicationDirPath() + "/../../../../../../src/metis-5.1.0/build/Darwin-x86_64/programs/gpmetis" + " " + analysisDirPath + "dist.graph" + " " + QString::number(numOfSubGraphs);
            if (QProcess::execute(command) < 0) {
                qCritical() << "Cannot start METIS program";
            }
        } else {
            command = qApp->applicationDirPath() + "/../../../../../../src/metis-5.1.0/build/Darwin-x86_64/programs/gpmetis" + " " + analysisDirPath + "adj.graph" + " " + QString::number(numOfSubGraphs);
            if (QProcess::execute(command) < 0) {
                qCritical() << "Cannot start METIS program";
            }
        }

        command = qApp->applicationDirPath() + "/../../../../../../src/metis-5.1.0/build/Darwin-x86_64/programs/gpmetis" + " " + analysisDirPath + "route_weight.graph" + " " + QString::number(numOfSubGraphs);// + " -objtype vol";// -tpwgts " + analysisDirPath + "twgts.txt";
        if (QProcess::execute(command) < 0) {
            qCritical() << "Cannot start METIS program";
        }

        // output resized adjmatrix and distance matrix
        //fc.csvGraphMatToResizedCsvGraphMat(dir + "dist_mat.csv", dir + "qap_intermed/",
        //                                                           thresForGraphPartition, "average",
        //                                                           dir + "dist.graph.part." + QString::number(thresForGraphPartition));
        if (!graphPartitionByAdj) {
            fc.csvGraphMatToResizedCsvGraphMat(analysisDirPath + "dist_mat.csv", analysisDirPath + "qap_intermed/",
                                               numOfVertices / thresForGraphPartition, "average",
                                               analysisDirPath + "dist.graph.part." + QString::number(numOfSubGraphs));
        } else {
            fc.csvGraphMatToResizedCsvGraphMat(analysisDirPath + "dist_mat.csv", analysisDirPath + "qap_intermed/",
                                               numOfSubGraphs, "average",
                                               analysisDirPath + "adj.graph.part." + QString::number(numOfSubGraphs));
        }
        fc.csvGraphMatToResizedCsvGraphMat(analysisDirPath + "route_weight_mat.csv", analysisDirPath + "qap_intermed/",
                                           numOfSubGraphs, "sum",
                                           analysisDirPath + "route_weight.graph.part." + QString::number(numOfSubGraphs));
    }

    ////// these are temporary code for test
    // route list
    graph->outputRouteListWithTCoordFromRoutesFile(routesFilePath, analysisDirPath + "route_list.csv", numCores, shape);

    // load hist
    scriptFile = qApp->applicationDirPath() + "/../../../../../../src/r/stat/outoutHist.R";
    command = "/usr/local/bin/Rscript " + scriptFile + " -ef " + analysisDirPath + "edges.csv" + " -n 100";

    if (QProcess::execute(command) < 0) {
        qCritical() << "Cannot start R program";
    }
}

void MainWindow::setupComboBox()
{
//    ui->comboGraphViewData_1->addItems(QStringList({"", "imb-MPI1", "multisend", "miniMD", "miniMDTheta", "miniAMR", "miniAMRTheta", "ioDefault", "ioOpt"}));
//    ui->comboGraphViewData_1->setCurrentText("");
//    updateGraphViewDataComboBoxes(ui->comboGraphViewData_1->currentText());
//    connect(ui->comboGraphViewData_1, SIGNAL(activated(QString)), this, SLOT(updateGraphViewDataComboBoxes(QString)));
//    connect(ui->comboGraphViewData_1, SIGNAL(activated(QString)), this, SLOT(resetupAll()));
//    connect(ui->comboGraphViewData_2, SIGNAL(activated(QString)), this, SLOT(resetupAll()));
//    connect(ui->comboGraphViewData_3, SIGNAL(activated(QString)), this, SLOT(resetupAll()));
//    connect(ui->comboGraphViewData_4, SIGNAL(activated(QString)), this, SLOT(resetupAll()));
}

void MainWindow::updateGraphViewDataComboBoxes(QString selectedApplication)
{
//    ui->comboGraphViewData_2->clear();
//    ui->comboGraphViewData_3->clear();
//    ui->comboGraphViewData_4->clear();
//    if (selectedApplication == "imb-MPI1") {
//        ui->comboGraphViewData_2->addItems(QStringList({"32","64"}));
//        ui->comboGraphViewData_2->setCurrentText("32");
//        ui->comboGraphViewData_3->addItems(QStringList({"16"}));
//        ui->comboGraphViewData_3->setCurrentText("16");
//        ui->comboGraphViewData_4->addItems(QStringList({""}));
//        ui->comboGraphViewData_4->setCurrentText("");
//    } else if (selectedApplication == "multisend") {
//        ui->comboGraphViewData_2->addItems(QStringList({"256","512","2048"}));
//        ui->comboGraphViewData_2->setCurrentText("256");
//        ui->comboGraphViewData_3->addItems(QStringList({"16"}));
//        ui->comboGraphViewData_3->setCurrentText("16");
//        ui->comboGraphViewData_4->addItems(QStringList({""}));
//        ui->comboGraphViewData_4->setCurrentText("");
//    } else if (selectedApplication == "miniMD") {
//        ui->comboGraphViewData_2->addItems(QStringList({"32", "64", "128", "256", "512", "1024", "2048", "4096", "8192"}));
//        ui->comboGraphViewData_2->setCurrentText("2048");
//        ui->comboGraphViewData_3->addItems(QStringList({"1", "4", "16"}));
//        ui->comboGraphViewData_3->setCurrentText("1");
//        ui->comboGraphViewData_4->addItems(QStringList({"_w", "_wr", "_s1", "_s1r", "_s2", "_s2r"}));
//        ui->comboGraphViewData_4->setCurrentText("_w");
//    } else if (selectedApplication == "miniMDTheta") {
//        ui->comboGraphViewData_2->addItems(QStringList({"32", "64", "128", "256", "512", "1024"}));
//        ui->comboGraphViewData_2->setCurrentText("32");
//        ui->comboGraphViewData_3->addItems(QStringList({"1"}));
//        ui->comboGraphViewData_3->setCurrentText("1");
//        ui->comboGraphViewData_4->addItems(QStringList({"_w"}));
//        ui->comboGraphViewData_4->setCurrentText("_w");
//    } else if (selectedApplication == "miniAMR") {
//        ui->comboGraphViewData_2->addItems(QStringList({"32", "64", "128", "256", "512", "1024", "2048", "4096"}));
//        ui->comboGraphViewData_2->setCurrentText("256");
//        ui->comboGraphViewData_3->addItems(QStringList({"1", "2", "4"}));
//        ui->comboGraphViewData_3->setCurrentText("1");
//        ui->comboGraphViewData_4->addItems(QStringList({"_s1","_s1r","_s2","_s2r"}));
//        ui->comboGraphViewData_4->setCurrentText("_s1");
//    } else if (selectedApplication == "miniAMRTheta") {
//        ui->comboGraphViewData_2->addItems(QStringList({"32","64","128","256","512","1024"}));
//        ui->comboGraphViewData_2->setCurrentText("32");
//        ui->comboGraphViewData_3->addItems(QStringList({"1"}));
//        ui->comboGraphViewData_3->setCurrentText("1");
//        ui->comboGraphViewData_4->addItems(QStringList({"_s2"}));
//        ui->comboGraphViewData_4->setCurrentText("_s2");
//    } else if (selectedApplication == "ioDefault" || selectedApplication == "ioOpt") {
//        ui->comboGraphViewData_2->addItems(QStringList({"512","1024"}));
//        ui->comboGraphViewData_2->setCurrentText("512");
//        ui->comboGraphViewData_3->addItems(QStringList({"16"}));
//        ui->comboGraphViewData_3->setCurrentText("16");
//        ui->comboGraphViewData_4->addItems(QStringList({""}));
//        ui->comboGraphViewData_4->setCurrentText("");
//    }

//    this->selectedApplication = selectedApplication;
}

void MainWindow::setupColor()
{
}

void MainWindow::setupGraph(QString analysisDirPath)
{
//    QString application = ui->comboGraphViewData_1->currentText(); // mimiMD, miniAMR, io
//    QString n = ui->comboGraphViewData_2->currentText();
//    QString c = ui->comboGraphViewData_3->currentText();
//    QString option = ui->comboGraphViewData_4->currentText(); // "" (none) or "_w" (weak scaling), "_s1" (strong scaling: small), "_s2" (strong scaling: large)

//    if (analysisDirPath == "") {
//        analysisDirPath = qApp->applicationDirPath() + "/../../../../../../data/graph/" + application + "_n" + n + "_c" + c + option + "/";
//    }

    QString layout = "sfdpw"; // "arf", "arfw", "sfdp", "sfdpw

    QString graphVerticesFilePath = analysisDirPath  + "vertices.csv";
    QString graphEdgesFilePath = analysisDirPath + "edges.csv";
    QString graphRoutesFilePath = analysisDirPath + "routes.csv";
    QString graphSummaryFilePath = analysisDirPath + "summary.csv";
    QString graphPositionsFilePath = analysisDirPath + "positions_" + layout + ".csv";

    graph->loadFromGraphFiles(graphVerticesFilePath, graphEdgesFilePath, graphRoutesFilePath, graphSummaryFilePath);
    graph->loadFromPosFile(graphPositionsFilePath);
    graph->setLayout(layout);

    graph->setData(analysisDirPath);

    // aggregated graph
    QString aGraphVerticesFilePath = analysisDirPath  + "aggregated_vertices.csv";
    QString aGraphEdgesFilePath = analysisDirPath + "aggregated_edges.csv";
    QString aGraphRoutesFilePath = analysisDirPath + "aggregated_routes.csv";
    QString aGraphSummaryFilePath = analysisDirPath + "aggregated_summary.csv";
    QString aGraphPositionsFilePath = analysisDirPath + "aggregated_positions_" + layout + ".csv";

    aggregatedGraph->loadFromGraphFiles(aGraphVerticesFilePath, aGraphEdgesFilePath, aGraphRoutesFilePath, aGraphSummaryFilePath);
    aggregatedGraph->loadFromPosFile(aGraphPositionsFilePath);
    aggregatedGraph->setLayout(layout);

    aggregatedGraph->setData(analysisDirPath);
}

void MainWindow::setupAdjMatrix(QString analysisDirPath)
{
    QString nodeMappingFilePath = analysisDirPath + "mapping.csv";
//    if (analysisDirPath == "") {
//        QString a = ui->comboGraphViewData_1->currentText(); // mimiMD, miniAMR, io
//        QString n = ui->comboGraphViewData_2->currentText();
//        QString c = ui->comboGraphViewData_3->currentText();
//        QString o = ui->comboGraphViewData_4->currentText();
//        nodeMappingFilePath = qApp->applicationDirPath() + "/../../../../../../data/graph/" + a + "_n" + n + "_c" + c + o + "/mapping.csv";
//    }

    QVector<int> shape;
    QVector<int> order;
    QString topologyType;

    QFile mappingFile(nodeMappingFilePath);
    if (mappingFile.open(QIODevice::ReadOnly)) {
        QTextStream ts(&mappingFile);

        int lineNo = 0;
        while (!ts.atEnd()) {
            QString line = ts.readLine();
            QStringList elements = line.split(" ");
            if (lineNo == 0) {
                foreach(QString elem, elements) {
                    shape.push_back(elem.toInt());
                }
            }
            if (lineNo == 1) {
                foreach(QString elem, elements) {
                    order.push_back(elem.toInt());
                }
            }
            if (lineNo == 2) {
                topologyType = elements[0];
            }
            lineNo++;
        }

        if (shape.size() != order.size()) qWarning() << "shape and order size is not same";
    }

    adjMat->resetAdjMat(shape, order, topologyType);

    QVector<int> shapeAfterAggregation = shape;
    QVector<int> orderWithoutAggregatedDims = order;
    foreach (int dim, aggregatingDims) {
        shapeAfterAggregation.remove(dim);
        orderWithoutAggregatedDims.remove(dim);
    }
    QVector<int> orderAfterAggregation;
    QVector<int> sortedOrderWithoutAggregatedDims = orderWithoutAggregatedDims;
    qSort(sortedOrderWithoutAggregatedDims.begin(), sortedOrderWithoutAggregatedDims.end());
    foreach (int o, orderWithoutAggregatedDims) {
        int rank = sortedOrderWithoutAggregatedDims.indexOf(o);
        orderAfterAggregation.push_back(rank);
    }
    aggregatedAdjMat->resetAdjMat(shapeAfterAggregation, orderAfterAggregation, topologyType);
}

void MainWindow::setupGraphView()
{
    QString edgeDrawingMode = "straight"; // "none", "straight"
    QString routeDrawingMode = "curve"; // "none", "src-dest", "curve", "straight"
    ui->graphView->setColor(color);
    ui->graphView->setGraph(graph);
    ui->graphView->setAggregatedGraph(aggregatedGraph);
    if (graph->getNumVertices() < thresAggregation) {
        ui->graphView->setDisplayedGraph(graph);
    } else {
        ui->graphView->setDisplayedGraph(aggregatedGraph);
    }
    ui->graphView->setEdgeDrawingMode(edgeDrawingMode);
    ui->graphView->setRouteDrawingMode(routeDrawingMode);
    ui->graphView->updateBasicVisualEncodingParams(selectedApplication);
    ui->graphView->resetSettings();
}

void MainWindow::setupMatrixView()
{
    ui->matrixView->setAdjMat(adjMat);
    ui->matrixView->setAggregatedAdjMat(aggregatedAdjMat);
    ui->matrixView->setGraph(graph);
    ui->matrixView->setAggregatedGraph(aggregatedGraph);
    if (graph->getNumVertices() < thresAggregation) {
        ui->matrixView->setDisplayedGraph(graph);
        ui->matrixView->setDisplayedAdjMat(adjMat);
    } else {
        ui->matrixView->setDisplayedGraph(aggregatedGraph);
        ui->matrixView->setDisplayedAdjMat(aggregatedAdjMat);
    }
    ui->matrixView->setColor(color);
    ui->matrixView->setUnitFoldingSize(1);
    ui->matrixView->setRscriptPath(rscriptPath);
    ui->matrixView->setSaQapPath(qApp->applicationDirPath() + "/../../../../../../src/r/qap/saQap.R");
    ui->matrixView->resetSettings();
}

void MainWindow::resetupAll(QString analysisDirPath)
{
    emit resetupAllCalled();

    setupGraph(analysisDirPath);
    setupAdjMatrix(analysisDirPath);
    setupGraphView();
    setupMatrixView();
    setupStatView();
    ui->graphView->update();
    ui->matrixView->update();
    updateBtnsAndInfoForAltRoute(0, 0, 0);
}

void MainWindow::resetupViews()
{
    emit resetupViewsCalled();

    setupGraphView();
    setupMatrixView();
    setupStatView();
    ui->graphView->update();
    ui->matrixView->update();
    updateBtnsAndInfoForAltRoute(0, 0, 0);
}

void MainWindow::updateBtnsAndInfoForAltRoute(int numSuggestedRoutes, int totalChangeOfLength, float totalChangeOfMaxLoad)
{
    if (numSuggestedRoutes == 0) {
        ui->btnShowAltRoute->setText("Alt Routing");
        ui->btnApplyAltRoute->setDisabled(true);
        ui->labelInfoAltRoute->setText("");
    } else {
        ui->btnShowAltRoute->setText("Clear");
        ui->btnApplyAltRoute->setEnabled(true);

        QString totalChangeOfLengthStr;
        if (totalChangeOfLength <= 0) {
            totalChangeOfLengthStr = QString::number(-totalChangeOfLength);
        } else {
            totalChangeOfLengthStr = QString::number(-totalChangeOfLength);
        }

        QString totalChangeOfMaxLoadStr;
        if (totalChangeOfMaxLoad <= 0) {
            totalChangeOfMaxLoadStr = QString::number(-totalChangeOfMaxLoad, 'g', 3);
        } else {
            totalChangeOfMaxLoadStr = QString::number(-totalChangeOfMaxLoad, 'g', 3);
        }
        ui->labelInfoAltRoute->setText("length:" + totalChangeOfLengthStr + ", highest load:" + totalChangeOfMaxLoadStr + "");
    }
}

void MainWindow::setMatrixViewGraphDir()
{
    // TODO: this code is temp one
//    QString graphDir = qApp->applicationDirPath() + "/../../../../../../data/graph/" +
//            ui->comboGraphViewData_1->currentText() +
//            "_n" + ui->comboGraphViewData_2->currentText() +
//            "_c" + ui->comboGraphViewData_3->currentText() +
//            ui->comboGraphViewData_4->currentText() + "/";
//    ui->matrixView->setGraphDir(graphDir);
}

void MainWindow::updateBtnsAndInfoForAltMapping(int numSuggestedRoutes, int totalChangeOfLength, int totalChangeOfMaxLoad)
{
    if (numSuggestedRoutes == 0) {
        ui->btnShowAltMapping->setText("Alt Mapping");
        ui->btnApplyAltMapping->setDisabled(true);
        ui->labelInfoAltRoute->setText("");
    } else {
        ui->btnShowAltMapping->setText("Clear");
        ui->btnApplyAltMapping->setEnabled(true);

        QString totalChangeOfLengthStr;
        if (totalChangeOfLength <= 0) {
            totalChangeOfLengthStr = QString::number(totalChangeOfLength);
        } else {
            totalChangeOfLengthStr = "+" + QString::number(totalChangeOfLength);
        }

        QString totalChangeOfMaxLoadStr;
        if (totalChangeOfMaxLoad <= 0) {
            totalChangeOfMaxLoadStr = QString::number(totalChangeOfMaxLoad);
        } else {
            totalChangeOfMaxLoadStr = "+" + QString::number(totalChangeOfMaxLoad);
        }
        ui->labelInfoAltRoute->setText("(length:" + totalChangeOfLengthStr + ", heaviest loads:" + totalChangeOfMaxLoadStr + ")");
    }
}

void MainWindow::radioLengthFirstSelected()
{
    ui->matrixView->setRoutingGuidePriority("lengthFirst");
    ui->matrixView->setRoutingGuideLengthTolerance(0);
    ui->matrixView->setRoutingGuideLoadTolerance(0);
}

void MainWindow::radioLoadFirstSelected()
{
    ui->matrixView->setRoutingGuidePriority("loadFirst");
    ui->matrixView->setRoutingGuideLengthTolerance(0);
    ui->matrixView->setRoutingGuideLoadTolerance(0);
}

void MainWindow::radioMappingAllSelected()
{
    ui->matrixView->setMappingGuideType("all-ma");
}

void MainWindow::radioMappingSelectedRoutesOptSelected()
{
    ui->matrixView->setMappingGuideType("selected-optimal");
}

void MainWindow::radioMappingSelectedRoutesRandomizedSelected()
{
    ui->matrixView->setMappingGuideType("selected-randomized");
}

void MainWindow::checkBoxAllowRemapRelased()
{
    //if (ui->checkBoxAllowRemap->isChecked()) ui->matrixView->setSuggestionType("src-fixed");
    //else ui->matrixView->setSuggestionType("src-dest-fixed");
}

void MainWindow::switchAutoUpdate()
{
    // TODO: need to think how to stop the update (qApp->processEvents(); etc)
    if (ui->btnAutoUpdate->text() == "Auto Update") {
        emit runAutoUpdateCalled();
        ui->btnAutoUpdate->setText("Stop Update");
        ui->matrixView->suggestAltRoutesForDisplayedRoutes();
    } else {
        emit stopAutoUpdateCalled();
        ui->btnAutoUpdate->setText("Auto Update");
    }
}

void MainWindow::setStatViewCurrentComboIndex(int statViewCurrentComboIndex) { this->statViewCurrentComboIndex = statViewCurrentComboIndex; }

void MainWindow::changeGraphLayout(QString layout)
{
//    QString application = ui->comboGraphViewData_1->currentText(); // mimiMD, miniAMR, io
//    QString n = ui->comboGraphViewData_2->currentText();
//    QString c = ui->comboGraphViewData_3->currentText();
//    QString option = ui->comboGraphViewData_4->currentText(); // "" (none) or "_w" (weak scaling), "_s1" (strong scaling: small), "_s2" (strong scaling: large)

//    QString graphPositionsFilePath = qApp->applicationDirPath() + "/../../../../../../data/graph/" + application + "_n" + n + "_c" + c + option + "/positions_" + layout + ".csv";
//    graph->loadFromPosFile(graphPositionsFilePath);
//    graph->setLayout(layout);
    resetupViews();
}

void MainWindow::changeGraphViewLine(QString routeDrawingMode)
{
    if (routeDrawingMode == "Catmull-Rom spline") {
        ui->graphView->setRouteDrawingMode("catmull-rom");
        ui->graphView->updateRoutesCurveVbos();
        ui->graphView->updateSelectedRoutesCurveVbos();
    } else if (routeDrawingMode == "Cubic B-spline") {
        ui->graphView->setRouteDrawingMode("bspline");
        ui->graphView->updateRoutesCurveVbos();
        ui->graphView->updateSelectedRoutesCurveVbos();
    } else if (routeDrawingMode == "Straight line") {
        ui->graphView->setRouteDrawingMode("straight");
        ui->graphView->updateRoutesCurveVbos();
        ui->graphView->updateSelectedRoutesCurveVbos();
    } else if (routeDrawingMode == "Metro style line") {
        ui->graphView->setRouteDrawingMode("metro");
        //ui->graphView->updateRoutePositionsMap();
        //ui->graphView->updateRoutesCurveVbos();
        //ui->graphView->updateSelectedRoutesCurveVbos();
    }
    ui->graphView->update();
}

void MainWindow::changeGraphViewLineColor(QString routeColorMetric)
{
    if (routeColorMetric == "Message Size") {
        ui->graphView->setRouteColorMetric("byte");
        ui->graphView->updateRoutesCurveVbos();
        ui->graphView->updateRoutesCbo();
        ui->matrixView->changeRouteColorMapSettings("byte", 0.0, 0.0);
    } else if (routeColorMetric == "Length") {
        ui->graphView->setRouteColorMetric("length");
        ui->graphView->updateRoutesCurveVbos();
        ui->graphView->updateRoutesCbo();
        ui->matrixView->changeRouteColorMapSettings("length", 0.0, 0.0);
    } else if (routeColorMetric == "Hop-Byte") {
        ui->graphView->setRouteColorMetric("hopbyte");
        ui->graphView->updateRoutesCurveVbos();
        ui->graphView->updateRoutesCbo();
        ui->matrixView->changeRouteColorMapSettings("hopbyte", 0.0, 0.0);
    } else if (routeColorMetric == "Distinguishable") {
        ui->graphView->setRouteColorMetric("distinguishable");
        ui->graphView->updateRoutesCurveVbos();
        ui->graphView->updateRoutesCbo();
        ui->matrixView->changeRouteColorMapSettings("distinguishable", 0.0, 0.0);
    }
    ui->graphView->update();
}

void MainWindow::changeGraphViewViewMode(QString viewMode)
{
    if (viewMode == "2D") {
        ui->graphView->resetSettings();
        ui->graphViewStackedWidget->setCurrentIndex(0);
    } else if (viewMode == "3D") {
        ui->graphViewStackedWidget->setCurrentIndex(1);
    }
}

void MainWindow::changeMatrixViewLine(QString routeDrawingMode)
{
    if (routeDrawingMode == "Catmull-Rom spline") {
        ui->matrixView->setRouteDrawingMode("catmull-rom");
        ui->matrixView->updateAllDrawingInfo();
        ui->matrixView->updateAllDrawingInfoForLens();
    } else if (routeDrawingMode == "Straight line") {
        ui->matrixView->setRouteDrawingMode("straight");
        ui->matrixView->updateAllDrawingInfo();
        ui->matrixView->updateAllDrawingInfoForLens();
    }
    ui->matrixView->update();
}

bool MainWindow::outputMappingFileFromQapResult(QString qapResultFilePath, QString outFilePath)
{
    // load qap result in vertexIndicesInQapResult
    QVector<int> indicesInQapResult;

    QFile qapResultFile(qapResultFilePath);
    if (qapResultFile.open(QIODevice::ReadOnly)) {
        QTextStream ts(&qapResultFile);

        while (!ts.atEnd()) {
            QString line = ts.readLine();
            indicesInQapResult.push_back(line.toInt());
        }

        qapResultFile.close();
    } else {
        qWarning() << "Couldn't open output order file. file path: " << qapResultFilePath;
        return false;
    }

    // generate coordinates based on qap result
    int totalNumOfRanks = graph->getNumVertices() * graph->getVertexAt(0).getRanks().size();

    QVector<QVector<int> > mappings(totalNumOfRanks, QVector<int>());

    for (int i = 0; i < indicesInQapResult.size(); ++i) {
        QVector<int> ranks = graph->getVertexAt(i).getRanks();
        QVector<int> coordWithoutT = adjMat->colNumToCoord(indicesInQapResult.at(i));

        for (int j = 0; j < ranks.size(); ++j) {
            QVector<int> coord = coordWithoutT;
            coord.push_back(j); // add T value
            mappings[ranks.at(j)] = coord;
        }
    }

    // output mapping file
    QFile outFile(outFilePath);
    if (!outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "cannot open file: " << outFilePath;
        return false;
    }
    QTextStream out(&outFile);

    for (int i = 0; i < mappings.size(); ++i) {
        for (int j = 0; j < mappings.at(i).size(); ++j) {
            if (j == 0) out << mappings.at(i).at(j);
            else out << " " << mappings.at(i).at(j);
        }
        out << "\n";
    }

    return true;
}

void MainWindow::outputResults()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"),
                                                    qApp->applicationDirPath() + "/../../../../../../data/",
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (dir != "") {
        //graph->outputVerticesInCsv(dir + "/vertices.csv");
        //graph->outputEdgesInCsv(dir + "/edges.csv");
        //graph->outputRoutesInCsv(dir + "/routes.csv");
        //graph->outputSummaryInCsv(dir + "/summary.csv");
        //graph->outputMappingFile(dir + "/mapping.txt");
        //graph->outputVerticesSimilarityInCsv(dir + "/vertices_similarity.csv");
        // temp here
        //graph->outputMappingFile(dir + "/mapping_2.txt", QVector<int>({14,46,35,19,16,3,49,61,18,31,30,62,15,2,29,12,47,51,63,34,6,54,55,48,27,50,26,60,58,28,17,59,7,42,11,13,37,43,57,10,1,45,9,33,41,44,40,39,0,23,53,52,56,36,24,38,8,32,20,22,4,5,21,25}));

        // these are temporary code for test
        outputMappingFileFromQapResult(dir + "/qapresult.txt", dir + "/mapping.txt");
    }
}

void MainWindow::loadCurrentSettings()
{
    QSettings settings("ANL_VIDI", "Visuco");

//    if (settings.value("settingsDialog/pythonPath").isValid())
//        pythonPath = settings.value("settingsDialog/pythonPath").toString();
//    if (settings.value("settingsDialog/rscriptPath").isValid())
//        rscriptPath = settings.value("settingsDialog/rscriptPath").toString();
}
