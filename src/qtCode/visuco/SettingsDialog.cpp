#include "SettingsDialog.h"

SettingsDialog::SettingsDialog(QWidget *parent)
{
    pythonPath = "/usr/local/bin/python2";
    rscriptPath = "/usr/local/bin/RScript";
    isOk = false;

    pythonPathLineEdit = new QLineEdit;
    rscriptPathLineEdit = new QLineEdit;
    pythonPathLineEdit->setText(pythonPath);
    rscriptPathLineEdit->setText(rscriptPath);

    QVBoxLayout *vBoxLayout = new QVBoxLayout;
    QHBoxLayout *pythonPathHBoxLayout = new QHBoxLayout;
    QHBoxLayout *rscriptPathHBoxLayout = new QHBoxLayout;
    QHBoxLayout *cancelOkHBoxLayout = new QHBoxLayout;

    QLabel *pythonPathLabel = new QLabel;
    QLabel *rscriptPathLabel = new QLabel;

    QPushButton *cancelBtn = new QPushButton;
    QPushButton *okBtn = new QPushButton;

    QSpacerItem *hSpacer = new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum);

    this->setWindowTitle("Environment Settings");
    pythonPathLabel->setText("Python path");
    rscriptPathLabel->setText("RScript path");
    cancelBtn->setText("Cancel");
    okBtn->setText("OK");

    pythonPathLabel->setMinimumWidth(200);
    pythonPathLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
    rscriptPathLabel->setMinimumWidth(200);
    rscriptPathLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
    pythonPathLineEdit->setMinimumWidth(400);
    rscriptPathLineEdit->setMinimumWidth(400);

    this->setLayout(vBoxLayout);
    vBoxLayout->addLayout(pythonPathHBoxLayout);
    pythonPathHBoxLayout->addWidget(pythonPathLabel);
    pythonPathHBoxLayout->addWidget(pythonPathLineEdit);
    vBoxLayout->addLayout(rscriptPathHBoxLayout);
    rscriptPathHBoxLayout->addWidget(rscriptPathLabel);
    rscriptPathHBoxLayout->addWidget(rscriptPathLineEdit);
    vBoxLayout->addLayout(cancelOkHBoxLayout);
    cancelOkHBoxLayout->addSpacerItem(hSpacer);
    cancelOkHBoxLayout->addWidget(cancelBtn);
    cancelOkHBoxLayout->addWidget(okBtn);

    setCurrentSettings();

    connect(cancelBtn, &QPushButton::clicked, this, &SettingsDialog::close);
    connect(okBtn, &QPushButton::clicked, this, &SettingsDialog::handleOk);
}

SettingsDialog::~SettingsDialog()
{

}

void SettingsDialog::handleOk()
{
    if (pythonPathLineEdit->text() != "" && rscriptPathLineEdit->text() != "") {
        pythonPath = pythonPathLineEdit->text();
        rscriptPath = rscriptPathLineEdit->text();
        saveSettings();
        isOk = true;
        this->close();
    } else {
        QMessageBox msgBox;
        if (pythonPathLineEdit->text() == "") {
            msgBox.setText("python path is not set.");
            msgBox.exec();
        } else if (rscriptPathLineEdit->text() == "") {
            msgBox.setText("rscript path is not set.");
            msgBox.exec();
        }
    }
}

void SettingsDialog::saveSettings()
{
    QSettings settings("ANL_VIDI", "Visuco");

    settings.setValue("settingsDialog/pythonPath", pythonPath);
    settings.setValue("settingsDialog/rscriptPath", rscriptPath);
}

void SettingsDialog::setCurrentSettings()
{
    QSettings settings("ANL_VIDI", "Visuco");

    if (settings.value("settingsDialog/pythonPath").isValid()) {
        pythonPathLineEdit->setText(settings.value("settingsDialog/pythonPath").toString());
        pythonPath = pythonPathLineEdit->text();
    }
    if (settings.value("settingsDialog/rscriptPath").isValid()) {
        rscriptPathLineEdit->setText(settings.value("settingsDialog/rscriptPath").toString());
        rscriptPath = rscriptPathLineEdit->text();
    }
}
