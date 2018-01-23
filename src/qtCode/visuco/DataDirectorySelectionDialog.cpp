#include "DataDirectorySelectionDialog.h"

DataDirectorySelectionDialog::DataDirectorySelectionDialog(QWidget *parent)
{
    routeFile = "";
    matrixMappingFile = "";
    hopByteFile = "";
    numNodes = 1;
    numCores = 1;
    thresForGraphPartition = 512;
    outputDir = "";
    isOk = false;

    routeFileLineEdit = new QLineEdit;
    matrixMappingFileLineEdit = new QLineEdit;
    hopByteFileLineEdit = new QLineEdit;
    topologyTypeComboBox = new QComboBox;
    topologyTypeComboBox->addItems({"theta", "5dtorus (currently, not supported.)"});
    numNodesLineEdit = new QLineEdit;
    numNodesLineEdit->setValidator(new QIntValidator(1, 999999, this));
    numNodesLineEdit->setText("1");
    numCoresLineEdit = new QLineEdit;
    numCoresLineEdit->setValidator(new QIntValidator(1, 999, this));
    numCoresLineEdit->setText("1");
    thresForGraphPartitionLineEdit = new QLineEdit;
    thresForGraphPartitionLineEdit->setValidator(new QIntValidator(1, 999999, this));
    thresForGraphPartitionLineEdit->setText("512");
    outputDirLineEdit = new QLineEdit;

    QVBoxLayout *vBoxLayout = new QVBoxLayout;
    QGroupBox *selectFilesGroupBox = new QGroupBox;
    QVBoxLayout *selectFilesVBoxLayout = new QVBoxLayout;
    QHBoxLayout *routeFileHBoxLayout = new QHBoxLayout;
    QHBoxLayout *matrixMappingFileHBoxLayout = new QHBoxLayout;
    QHBoxLayout *hopByteFileHBoxLayout = new QHBoxLayout;
    QGroupBox *selectParametersGroupBox = new QGroupBox;
    QVBoxLayout *selectParametersVBoxLayout = new QVBoxLayout;
    QHBoxLayout *topologyTypeHBoxLayout = new QHBoxLayout;
    QHBoxLayout *numNodesHBoxLayout = new QHBoxLayout;
    QHBoxLayout *numCoresHBoxLayout = new QHBoxLayout;
    QHBoxLayout *thresForGraphPartitionHBoxLayout = new QHBoxLayout;
    QHBoxLayout *outputDirHBoxLayout = new QHBoxLayout;
    QHBoxLayout *cancelOkHBoxLayout = new QHBoxLayout;

    QLabel *explanationLabel = new QLabel;
    QLabel *fileSelectionExplanationLabel = new QLabel;
    QLabel *routeFileLabel = new QLabel;
    QLabel *matrixMappingFileLabel = new QLabel;
    QLabel *hopByteFileLabel = new QLabel;
    QLabel *parameterSelectionExplanationLabel = new QLabel;
    QLabel *topologyTypeLabel = new QLabel;
    QLabel *numNodesLabel = new QLabel;
    QLabel *numCoresLabel = new QLabel;
    QLabel *thresForGraphPartitionLabel = new QLabel;
    QLabel *outputDirLabel = new QLabel;

    QPushButton *routeFilePushBtn = new QPushButton;
    QPushButton *matrixMappingFilePushBtn = new QPushButton;
    QPushButton *hopByteFilePushBtn = new QPushButton;
    QPushButton *outputDirPushBtn = new QPushButton;
    QPushButton *cancelBtn = new QPushButton;
    QPushButton *okBtn = new QPushButton;

    QSpacerItem *hSpacer = new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum);

    this->setWindowTitle("Generate Analysis Data");
    explanationLabel->setText("Set files and parameters to generate analysis data.");

    explanationLabel->setFont(QFont("QFont::SansSeri", 13, QFont::Bold));
    fileSelectionExplanationLabel->setText("Select files");
    routeFileLabel->setText("Route file");
    matrixMappingFileLabel->setText("Matrix mapping file");
    hopByteFileLabel->setText("Hop-byte file (optional)");
    parameterSelectionExplanationLabel->setText("Set numbers");
    topologyTypeLabel->setText("Topology type");
    numNodesLabel->setText("Number of nodes");
    numCoresLabel->setText("Number of cores");
    thresForGraphPartitionLabel->setText("Thres # of nodes to start aggregation");
    outputDirLabel->setText("Select output directory");

    routeFilePushBtn->setText("Select");
    matrixMappingFilePushBtn->setText("Select");
    hopByteFilePushBtn->setText("Select");
    outputDirLineEdit->setText(outputDir);
    outputDirLineEdit->setReadOnly(true);
    outputDirPushBtn->setText("Select");
    cancelBtn->setText("Cancel");
    okBtn->setText("OK");

    routeFileLabel->setMinimumWidth(200);
    routeFileLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
    matrixMappingFileLabel->setMinimumWidth(200);
    matrixMappingFileLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
    hopByteFileLabel->setMinimumWidth(200);
    hopByteFileLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
    topologyTypeComboBox->setMinimumWidth(100);
    topologyTypeComboBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
    topologyTypeLabel->setMinimumWidth(200);
    topologyTypeLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
    numNodesLabel->setMinimumWidth(200);
    numNodesLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
    numCoresLabel->setMinimumWidth(200);
    numCoresLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
    thresForGraphPartitionLabel->setMinimumWidth(200);
    thresForGraphPartitionLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);

    routeFileLineEdit->setMinimumWidth(400);
    matrixMappingFileLineEdit->setMinimumWidth(400);
    hopByteFileLineEdit->setMinimumWidth(400);
    numNodesLineEdit->setMinimumWidth(100);
    numCoresLineEdit->setMinimumWidth(100);
    thresForGraphPartitionLineEdit->setMinimumWidth(100);
    numNodesLineEdit->setMaximumWidth(100);
    numCoresLineEdit->setMaximumWidth(100);
    thresForGraphPartitionLineEdit->setMaximumWidth(100);
    outputDirLineEdit->setMinimumWidth(400);

    routeFileLineEdit->setFocusPolicy(Qt::NoFocus);
    matrixMappingFileLineEdit->setFocusPolicy(Qt::NoFocus);
    hopByteFileLineEdit->setFocusPolicy(Qt::NoFocus);
    outputDirLineEdit->setFocusPolicy(Qt::NoFocus);
    routeFilePushBtn->setFocusPolicy(Qt::NoFocus);
    matrixMappingFilePushBtn->setFocusPolicy(Qt::NoFocus);
    hopByteFilePushBtn->setFocusPolicy(Qt::NoFocus);
    outputDirPushBtn->setFocusPolicy(Qt::NoFocus);
    cancelBtn->setFocusPolicy(Qt::NoFocus);
    okBtn->setFocusPolicy(Qt::NoFocus);

    this->setLayout(vBoxLayout);
    vBoxLayout->addWidget(explanationLabel);
    vBoxLayout->addWidget(selectFilesGroupBox);
    selectFilesGroupBox->setLayout(selectFilesVBoxLayout);
    selectFilesVBoxLayout->addWidget(fileSelectionExplanationLabel);
    selectFilesVBoxLayout->addLayout(routeFileHBoxLayout);
    selectFilesVBoxLayout->addLayout(matrixMappingFileHBoxLayout);
    selectFilesVBoxLayout->addLayout(hopByteFileHBoxLayout);
    vBoxLayout->addWidget(selectParametersGroupBox);
    selectParametersGroupBox->setLayout(selectParametersVBoxLayout);
    selectParametersVBoxLayout->addWidget(parameterSelectionExplanationLabel);
    selectParametersVBoxLayout->addLayout(topologyTypeHBoxLayout);
    selectParametersVBoxLayout->addLayout(numNodesHBoxLayout);
    selectParametersVBoxLayout->addLayout(numCoresHBoxLayout);
    selectParametersVBoxLayout->addLayout(thresForGraphPartitionHBoxLayout);
    vBoxLayout->addLayout(outputDirHBoxLayout);
    vBoxLayout->addLayout(cancelOkHBoxLayout);

    routeFileHBoxLayout->addWidget(routeFileLabel);
    routeFileHBoxLayout->addWidget(routeFileLineEdit);
    routeFileHBoxLayout->addWidget(routeFilePushBtn);

    matrixMappingFileHBoxLayout->addWidget(matrixMappingFileLabel);
    matrixMappingFileHBoxLayout->addWidget(matrixMappingFileLineEdit);
    matrixMappingFileHBoxLayout->addWidget(matrixMappingFilePushBtn);

    hopByteFileHBoxLayout->addWidget(hopByteFileLabel);
    hopByteFileHBoxLayout->addWidget(hopByteFileLineEdit);
    hopByteFileHBoxLayout->addWidget(hopByteFilePushBtn);

    //topologyTypeHBoxLayout->addWidget(topologyTypeLabel);
    //topologyTypeHBoxLayout->addWidget(topologyTypeComboBox);

    //numNodesHBoxLayout->addWidget(numNodesLabel);
    //numNodesHBoxLayout->addWidget(numNodesLineEdit);

    numCoresHBoxLayout->addWidget(numCoresLabel);
    numCoresHBoxLayout->addWidget(numCoresLineEdit);

    thresForGraphPartitionHBoxLayout->addWidget(thresForGraphPartitionLabel);
    thresForGraphPartitionHBoxLayout->addWidget(thresForGraphPartitionLineEdit);

    outputDirHBoxLayout->addWidget(outputDirLabel);
    outputDirHBoxLayout->addWidget(outputDirLineEdit);
    outputDirHBoxLayout->addWidget(outputDirPushBtn);

    cancelOkHBoxLayout->addSpacerItem(hSpacer);
    cancelOkHBoxLayout->addWidget(cancelBtn);
    cancelOkHBoxLayout->addWidget(okBtn);

    setCurrentSettings();

    connect(outputDirPushBtn, &QPushButton::clicked, this, &DataDirectorySelectionDialog::setOutputDir);
    connect(routeFilePushBtn, &QPushButton::clicked, this, &DataDirectorySelectionDialog::setRouteFile);
    connect(matrixMappingFilePushBtn, &QPushButton::clicked, this, &DataDirectorySelectionDialog::setMatrixMappingFile);
    connect(hopByteFilePushBtn, &QPushButton::clicked, this, &DataDirectorySelectionDialog::setHopByteFile);
    connect(cancelBtn, &QPushButton::clicked, this, &DataDirectorySelectionDialog::close);
    connect(okBtn, &QPushButton::clicked, this, &DataDirectorySelectionDialog::handleOk);
}

DataDirectorySelectionDialog::~DataDirectorySelectionDialog()
{

}

void DataDirectorySelectionDialog::setOutputDir()
{
    outputDir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), outputDir,
                                                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks) + "/";
    outputDirLineEdit->setText(outputDir);
}

void DataDirectorySelectionDialog::setRouteFile()
{
    routeFile = QFileDialog::getOpenFileName(this, tr("Open File"), routeFile, tr("TXT Files (*.txt)"));
    routeFileLineEdit->setText(routeFile);
}

void DataDirectorySelectionDialog::setMatrixMappingFile()
{
    matrixMappingFile = QFileDialog::getOpenFileName(this, tr("Open File"), matrixMappingFile, tr("TXT Files (*.txt)"));
    matrixMappingFileLineEdit->setText(matrixMappingFile);
}

void DataDirectorySelectionDialog::setHopByteFile()
{
    hopByteFile = QFileDialog::getOpenFileName(this, tr("Open File"), hopByteFile, tr("TXT Files (*.txt)"));
    hopByteFileLineEdit->setText(hopByteFile);
}

void DataDirectorySelectionDialog::handleOk()
{
    if (QDir(outputDir).exists() && QFile(routeFile).exists() && QFile(matrixMappingFile).exists() &&
            (QFile(hopByteFile).exists() || hopByteFile == "") &&
            numNodesLineEdit->text() != "" && numCoresLineEdit->text() != "" && thresForGraphPartitionLineEdit->text() != "") {
        numNodes = numNodesLineEdit->text().toInt();
        numCores = numCoresLineEdit->text().toInt();
        thresForGraphPartition = thresForGraphPartitionLineEdit->text().toInt();

        saveSettings();
        isOk = true;
        this->close();
    } else {
        QMessageBox msgBox;
        if (!QFile(routeFile).exists()) {
            msgBox.setText(routeFile + " does not exist.");
            msgBox.exec();
        } else if (!QFile(matrixMappingFile).exists()) {
            msgBox.setText(matrixMappingFile + " does not exist.");
            msgBox.exec();
        } else if (!QFile(hopByteFile).exists()) {
            msgBox.setText(hopByteFile + " does not exist.");
            msgBox.exec();
        } else if (!QDir(outputDir).exists()) {
            msgBox.setText(outputDir + " does not exist.");
            msgBox.exec();
        } else if (numNodesLineEdit->text() == "") {
            msgBox.setText("number of nodes is not set");
            msgBox.exec();
        } else if (numCoresLineEdit->text() == "") {
            msgBox.setText("number of cores is not set");
            msgBox.exec();
        } else if (thresForGraphPartitionLineEdit->text() == "") {
            msgBox.setText("threshold of number of nodes is not set");
            msgBox.exec();
        }
    }
}

void DataDirectorySelectionDialog::saveSettings()
{
    QSettings settings("ANL_VIDI", "Visuco");

    settings.setValue("dataDirectorySelectionDialog/outputDir", outputDir);
    settings.setValue("dataDirectorySelectionDialog/routeFile", routeFile);
    settings.setValue("dataDirectorySelectionDialog/matrixMappingFile", matrixMappingFile);
    settings.setValue("dataDirectorySelectionDialog/hopByteFile", hopByteFile);
    settings.setValue("dataDirectorySelectionDialog/numNodes", numNodes);
    settings.setValue("dataDirectorySelectionDialog/numCores", numCores);
    settings.setValue("dataDirectorySelectionDialog/thresForGraphPartition", thresForGraphPartition);
}

void DataDirectorySelectionDialog::setCurrentSettings()
{
    QSettings settings("ANL_VIDI", "Visuco");

    if (settings.value("dataDirectorySelectionDialog/outputDir").isValid()) {
        outputDirLineEdit->setText(settings.value("dataDirectorySelectionDialog/outputDir").toString());
        outputDir = outputDirLineEdit->text();
    }
    if (settings.value("dataDirectorySelectionDialog/routeFile").isValid()) {
        routeFileLineEdit->setText(settings.value("dataDirectorySelectionDialog/routeFile").toString());
        routeFile = routeFileLineEdit->text();
    }
    if (settings.value("dataDirectorySelectionDialog/matrixMappingFile").isValid()) {
        matrixMappingFileLineEdit->setText(settings.value("dataDirectorySelectionDialog/matrixMappingFile").toString());
        matrixMappingFile = matrixMappingFileLineEdit->text();
    }
    if (settings.value("dataDirectorySelectionDialog/hopByteFile").isValid()) {
        hopByteFileLineEdit->setText(settings.value("dataDirectorySelectionDialog/hopByteFile").toString());
        hopByteFile = hopByteFileLineEdit->text();
    }
    if (settings.value("dataDirectorySelectionDialog/numNodes").isValid()) {
        numNodesLineEdit->setText(settings.value("dataDirectorySelectionDialog/numNodes").toString());
        numNodes = numNodesLineEdit->text().toInt();
    }
    if (settings.value("dataDirectorySelectionDialog/numCores").isValid()) {
        numCoresLineEdit->setText(settings.value("dataDirectorySelectionDialog/numCores").toString());
        numCores = numCoresLineEdit->text().toInt();
    }
    if (settings.value("dataDirectorySelectionDialog/thresForGraphPartition").isValid()) {
        thresForGraphPartitionLineEdit->setText(settings.value("dataDirectorySelectionDialog/thresForGraphPartition").toString());
        thresForGraphPartition = thresForGraphPartitionLineEdit->text().toInt();
    }
}
