#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QString>
#include <QLineEdit>
#include <QLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QSettings>

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();
    QString pythonPath;
    QString rscriptPath;
    bool isOk;

private:
    QLineEdit *pythonPathLineEdit;
    QLineEdit *rscriptPathLineEdit;

    void setCurrentSettings();

private slots:
    void handleOk();
    void saveSettings();
};

#endif // SETTINGSDIALOG_H
