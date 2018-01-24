#include "VisualEncodingDialog.h"

VisualEncodingDialog::VisualEncodingDialog(QWidget *parent)
{
    // for graphview
    /*
    nodeSizeWeight = 20.0;
    nodeSizeMin = 1.0;
    edgeWidthWeight = 5.0;
    routeWidthWeight = 0.01;
    selfLoopRouteRatio = 1.0;// 0.4
    showOnlySelectedElems = false;
    filterOutAlpha = 0.35;
    filterOutDarker = 300;
    */
    // for matrixview
    /*
    routeWidth = 3.0;
    edgeWidth = 4.0;
    */

    isOk = false;

    QVBoxLayout *vBoxMain = new QVBoxLayout;
    QGroupBox *graphViewGroupBox = new QGroupBox;
    QVBoxLayout *vBoxGraphView = new QVBoxLayout;
    QGroupBox *matrixViewGroupBox = new QGroupBox;
    QVBoxLayout *vBoxMatrixView = new QVBoxLayout;
    QHBoxLayout *hBoxCancelOk = new QHBoxLayout;
    graphViewGroupBox->setTitle("Communication View");
    matrixViewGroupBox->setTitle("Detailed Route View");
    graphViewGroupBox->setLayout(vBoxGraphView);
    matrixViewGroupBox->setLayout(vBoxMatrixView);

    this->setLayout(vBoxMain);
    vBoxMain->addWidget(graphViewGroupBox);
    vBoxMain->addWidget(matrixViewGroupBox);
    vBoxMain->addLayout(hBoxCancelOk);

    // graphview
    QHBoxLayout *hBoxNodeSizeWeight = new QHBoxLayout;
    QHBoxLayout *hBoxNodeSizeMin = new QHBoxLayout;
    QHBoxLayout *hBoxEdgeWidthWeight = new QHBoxLayout;
    QHBoxLayout *hBoxRouteWidthWeight = new QHBoxLayout;
    QHBoxLayout *hBoxSelfLoopRouteRatio = new QHBoxLayout;
    QHBoxLayout *hBoxShowOnlySelectedElems = new QHBoxLayout;
    QHBoxLayout *hBoxFilterOutAlpha = new QHBoxLayout;
    QHBoxLayout *hBoxFilterOutDarker = new QHBoxLayout;
    QLabel *nodeSizeWeightLabel = new QLabel;
    QLabel *nodeSizeMinLabel = new QLabel;
    QLabel *edgeWidthWeightLabel = new QLabel;
    QLabel *routeWidthWeightLabel = new QLabel;
    QLabel *selfLoopRouteRatioLabel = new QLabel;
    QLabel *showOnlySelectedElemsLabel = new QLabel;
    QLabel *filterOutAlphaLabel = new QLabel;
    QLabel *filterOutDarkerLabel = new QLabel;
    // matrix view
    QHBoxLayout *hBoxEdgeWidthMatrixView = new QHBoxLayout;
    QHBoxLayout *hBoxRouteWidthMatrixView = new QHBoxLayout;
    QLabel *edgeWidthMatrixViewLabel = new QLabel;
    QLabel *routeWidthMatrixViewLabel = new QLabel;

    QPushButton *cancelBtn = new QPushButton;
    QPushButton *okBtn = new QPushButton;
    QSpacerItem *hSpacer = new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hBoxNodeSizeWeight->addWidget(nodeSizeWeightLabel);
    hBoxNodeSizeWeight->addWidget(nodeSizeWeightLineEdit);
    hBoxNodeSizeMin->addWidget(nodeSizeMinLabel);
    hBoxNodeSizeMin->addWidget(nodeSizeMinLineEdit);
    hBoxEdgeWidthWeight->addWidget(edgeWidthWeightLabel);
    hBoxEdgeWidthWeight->addWidget(edgeWidthWeightLineEdit);
    hBoxRouteWidthWeight->addWidget(routeWidthWeightLabel);
    hBoxRouteWidthWeight->addWidget(routeWidthWeightLineEdit);
    hBoxSelfLoopRouteRatio->addWidget(selfLoopRouteRatioLabel);
    hBoxSelfLoopRouteRatio->addWidget(selfLoopRouteRatioLineEdit);
    hBoxShowOnlySelectedElems->addWidget(showOnlySelectedElemsLabel);
    hBoxShowOnlySelectedElems->addWidget(showOnlySelectedElemsLineEdit);
    hBoxFilterOutAlpha->addWidget(filterOutAlphaLabel);
    hBoxFilterOutAlpha->addWidget(filterOutAlphaLineEdit);
    hBoxFilterOutDarker->addWidget(filterOutDarkerLabel);
    hBoxFilterOutDarker->addWidget(filterOutDarkerLineEdit);

    hBoxEdgeWidthMatrixView->addWidget(edgeWidthMatrixViewLabel);
    hBoxEdgeWidthMatrixView->addWidget(edgeWidthMatrixViewLineEdit);
    hBoxRouteWidthMatrixView->addWidget(routeWidthMatrixViewLabel);
    hBoxRouteWidthMatrixView->addWidget(routeWidthMatrixViewLineEdit);

    hBoxCancelOk->addSpacerItem(hSpacer);
    hBoxCancelOk->addWidget(cancelBtn);
    hBoxCancelOk->addWidget(okBtn);

    vBoxGraphView->addLayout(hBoxNodeSizeWeight);
    vBoxGraphView->addLayout(hBoxNodeSizeMin);
    vBoxGraphView->addLayout(hBoxEdgeWidthWeight);
    vBoxGraphView->addLayout(hBoxRouteWidthWeight);
    vBoxGraphView->addLayout(hBoxSelfLoopRouteRatio);
    //vBoxGraphView->addLayout(hBoxShowOnlySelectedElems);
    //vBoxGraphView->addLayout(hBoxFilterOutAlpha);
    //vBoxGraphView->addLayout(hBoxFilterOutDarker);

    vBoxMatrixView->addLayout(hBoxEdgeWidthMatrixView);
    vBoxMatrixView->addLayout(hBoxRouteWidthMatrixView);

    nodeSizeWeightLabel->setText("Node Size");
    nodeSizeMinLabel->setText("Minimum Node Size");
    edgeWidthWeightLabel->setText("Edge Width");
    routeWidthWeightLabel->setText("Route Width");
    selfLoopRouteRatioLabel->setText("Self-loop Route Width");
    showOnlySelectedElemsLabel->setText("Show Only Selected Elements");
    filterOutAlphaLabel->setText("Opacitiy for Filtered-out Elements");
    filterOutDarkerLabel->setText("Darker for Filtered-out Elements");
    edgeWidthMatrixViewLabel->setText("Edge Width");
    routeWidthMatrixViewLabel->setText("Route Width");
    cancelBtn->setText("Cancel");
    okBtn->setText("OK");
    okBtn->setDefault(true);

    nodeSizeWeightLabel->setAlignment(Qt::AlignLeft);
    nodeSizeMinLabel->setAlignment(Qt::AlignLeft);
    edgeWidthWeightLabel->setAlignment(Qt::AlignLeft);
    routeWidthWeightLabel->setAlignment(Qt::AlignLeft);
    selfLoopRouteRatioLabel->setAlignment(Qt::AlignLeft);
    showOnlySelectedElemsLabel->setAlignment(Qt::AlignLeft);
    filterOutAlphaLabel->setAlignment(Qt::AlignLeft);
    filterOutDarkerLabel->setAlignment(Qt::AlignLeft);
    edgeWidthMatrixViewLabel->setAlignment(Qt::AlignLeft);
    routeWidthMatrixViewLabel->setAlignment(Qt::AlignLeft);

    nodeSizeWeightLineEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
    nodeSizeMinLineEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
    edgeWidthWeightLineEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
    routeWidthWeightLineEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
    selfLoopRouteRatioLineEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
    showOnlySelectedElemsLineEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
    filterOutAlphaLineEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
    filterOutDarkerLineEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
    edgeWidthMatrixViewLineEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
    routeWidthMatrixViewLineEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
    nodeSizeWeightLineEdit->setMinimumWidth(100);
    nodeSizeMinLineEdit->setMinimumWidth(100);
    edgeWidthWeightLineEdit->setMinimumWidth(100);
    routeWidthWeightLineEdit->setMinimumWidth(100);
    selfLoopRouteRatioLineEdit->setMinimumWidth(100);
    showOnlySelectedElemsLineEdit->setMinimumWidth(100);
    filterOutAlphaLineEdit->setMinimumWidth(100);
    filterOutDarkerLineEdit->setMinimumWidth(100);
    edgeWidthMatrixViewLineEdit->setMinimumWidth(100);
    routeWidthMatrixViewLineEdit->setMinimumWidth(100);

    nodeSizeWeightLineEdit->setValidator( new QDoubleValidator(0, 100, 5, this) );
    nodeSizeMinLineEdit->setValidator( new QDoubleValidator(0, 100, 5, this) );
    edgeWidthWeightLineEdit->setValidator( new QDoubleValidator(0, 100, 5, this) );
    routeWidthWeightLineEdit->setValidator( new QDoubleValidator(0, 100, 5, this) );
    selfLoopRouteRatioLineEdit->setValidator( new QDoubleValidator(0, 100, 5, this) );
    showOnlySelectedElemsLineEdit->setValidator( new QDoubleValidator(0, 100, 5, this) );
    filterOutAlphaLineEdit->setValidator( new QDoubleValidator(0, 100, 5, this) );
    filterOutDarkerLineEdit->setValidator( new QDoubleValidator(0, 100, 5, this) );
    edgeWidthMatrixViewLineEdit->setValidator( new QDoubleValidator(0, 100, 5, this) );
    routeWidthMatrixViewLineEdit->setValidator( new QDoubleValidator(0, 5, 5, this) );


    setCurrentSettings();

    connect(cancelBtn, &QPushButton::clicked, this, &VisualEncodingDialog::close);
    connect(okBtn, &QPushButton::clicked, this, &VisualEncodingDialog::handleOk);
}

VisualEncodingDialog::~VisualEncodingDialog()
{

}

void VisualEncodingDialog::setCurrentSettings()
{
    QSettings settings("ANL_VIDI", "Visuco");

    if (settings.value("dataDirectorySelectionDialog/nodeSizeWeight").isValid()) {
        nodeSizeWeightLineEdit->setText(QString::number(settings.value("dataDirectorySelectionDialog/nodeSizeWeight").toFloat(), 'g', 5));
    } else {
        nodeSizeWeightLineEdit->setText("20.0");
    }
    nodeSizeWeight = nodeSizeWeightLineEdit->text().toFloat();

    if (settings.value("dataDirectorySelectionDialog/nodeSizeMin").isValid()) {
        nodeSizeMinLineEdit->setText(QString::number(settings.value("dataDirectorySelectionDialog/nodeSizeMin").toFloat(), 'g', 5));
    } else {
        nodeSizeMinLineEdit->setText("1.0");
    }
    nodeSizeMin = nodeSizeMinLineEdit->text().toFloat();

    if (settings.value("dataDirectorySelectionDialog/edgeWidthWeight").isValid()) {
        edgeWidthWeightLineEdit->setText(QString::number(settings.value("dataDirectorySelectionDialog/edgeWidthWeight").toFloat(), 'g', 5));
    } else {
        edgeWidthWeightLineEdit->setText("5.0");
    }
    edgeWidthWeight = edgeWidthWeightLineEdit->text().toFloat();

    if (settings.value("dataDirectorySelectionDialog/routeWidthWeight").isValid()) {
        routeWidthWeightLineEdit->setText(QString::number(settings.value("dataDirectorySelectionDialog/routeWidthWeight").toFloat(), 'g', 5));
    } else {
        routeWidthWeightLineEdit->setText("0.01");
    }
    routeWidthWeight = routeWidthWeightLineEdit->text().toFloat();

    if (settings.value("dataDirectorySelectionDialog/selfLoopRouteRatio").isValid()) {
        selfLoopRouteRatioLineEdit->setText(QString::number(settings.value("dataDirectorySelectionDialog/selfLoopRouteRatio").toFloat(), 'g', 5));
    } else {
        selfLoopRouteRatioLineEdit->setText("1.0");
    }
    selfLoopRouteRatio = selfLoopRouteRatioLineEdit->text().toFloat();

    if (settings.value("dataDirectorySelectionDialog/edgeWidthMatrixView").isValid()) {
        edgeWidthMatrixViewLineEdit->setText(QString::number(settings.value("dataDirectorySelectionDialog/edgeWidthMatrixView").toFloat(), 'g', 5));
    } else {
        edgeWidthMatrixViewLineEdit->setText("4.0");
    }
    edgeWidthMatrixView = edgeWidthMatrixViewLineEdit->text().toFloat();

    if (settings.value("dataDirectorySelectionDialog/routeWidthMatrixView").isValid()) {
        routeWidthMatrixViewLineEdit->setText(QString::number(settings.value("dataDirectorySelectionDialog/routeWidthMatrixView").toFloat(), 'g', 5));
    } else {
        routeWidthMatrixViewLineEdit->setText("3.0");
    }
    routeWidthMatrixView = routeWidthMatrixViewLineEdit->text().toFloat();
}

void VisualEncodingDialog::saveSettings()
{
    QSettings settings("ANL_VIDI", "Visuco");
    settings.setValue("dataDirectorySelectionDialog/nodeSizeWeight", nodeSizeWeight);
    settings.setValue("dataDirectorySelectionDialog/nodeSizeMin", nodeSizeMin);
    settings.setValue("dataDirectorySelectionDialog/edgeWidthWeight", edgeWidthWeight);
    settings.setValue("dataDirectorySelectionDialog/routeWidthWeight", routeWidthWeight);
    settings.setValue("dataDirectorySelectionDialog/selfLoopRouteRatio", selfLoopRouteRatio);
    settings.setValue("dataDirectorySelectionDialog/edgeWidthMatrixView", edgeWidthMatrixView);
    settings.setValue("dataDirectorySelectionDialog/routeWidthMatrixView", routeWidthMatrixView);
}


void VisualEncodingDialog::handleOk()
{
    nodeSizeWeight = nodeSizeWeightLineEdit->text().toFloat();
    nodeSizeMin = nodeSizeMinLineEdit->text().toFloat();
    edgeWidthWeight = edgeWidthWeightLineEdit->text().toFloat();
    routeWidthWeight = routeWidthWeightLineEdit->text().toFloat();
    selfLoopRouteRatio = selfLoopRouteRatioLineEdit->text().toFloat();
    edgeWidthMatrixView = edgeWidthMatrixViewLineEdit->text().toFloat();
    routeWidthMatrixView = routeWidthMatrixViewLineEdit->text().toFloat();

    saveSettings();
    isOk = true;
    close();
}
