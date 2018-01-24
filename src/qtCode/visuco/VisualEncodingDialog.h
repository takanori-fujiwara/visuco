#ifndef VISUALENCODINGDIALOG_H
#define VISUALENCODINGDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QSpacerItem>
#include <QDebug>
#include <QSettings>

class VisualEncodingDialog : public QDialog
{
    Q_OBJECT
public:
    explicit VisualEncodingDialog(QWidget *parent = 0);
    ~VisualEncodingDialog();

    float nodeSizeWeight;
    float nodeSizeMin;
    float edgeWidthWeight;
    float routeWidthWeight;
    float selfLoopRouteRatio;
    float edgeWidthMatrixView;
    float routeWidthMatrixView;
    bool isOk;

private:
    QLineEdit *nodeSizeWeightLineEdit = new QLineEdit;
    QLineEdit *nodeSizeMinLineEdit = new QLineEdit;
    QLineEdit *edgeWidthWeightLineEdit = new QLineEdit;
    QLineEdit *routeWidthWeightLineEdit = new QLineEdit;
    QLineEdit *selfLoopRouteRatioLineEdit = new QLineEdit;
    QLineEdit *showOnlySelectedElemsLineEdit = new QLineEdit;
    QLineEdit *filterOutAlphaLineEdit = new QLineEdit;
    QLineEdit *filterOutDarkerLineEdit = new QLineEdit;
    QLineEdit *edgeWidthMatrixViewLineEdit = new QLineEdit;
    QLineEdit *routeWidthMatrixViewLineEdit = new QLineEdit;
    void setCurrentSettings();
    void handleOk();
    void saveSettings();
};

#endif // VISUALENCODINGDIALOG_H
