#ifndef NEWPARENTFOLDERDIALOG_H
#define NEWPARENTFOLDERDIALOG_H

#include <QWidget>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>

class NewParentFolderDialog : public QDialog
{
    Q_OBJECT
public:
    explicit NewParentFolderDialog(QWidget *parent = nullptr);
public slots:
    void okClicked();
    void openDirectory();
signals:
    void title(QString);
private:
    QLineEdit * input;
};

#endif // NEWPARENTFOLDERDIALOG_H
