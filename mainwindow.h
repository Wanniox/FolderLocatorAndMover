#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHBoxLayout>
#include <QDebug>
#include <QFileDialog>
#include "parentfolder.h"
#include <QStandardPaths>
#include <QStringList>
#include <QDir>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QList<ParentFolder*>* getFolders();
    void addNewParentFolder(QString);
public slots:
    void newParentFolder();
    void deleteParentFolder();
    void labelClickedSlot(QObject *);
    void refreshDirectories();
    void save();
    void load();

private:
    Ui::MainWindow *ui;
    QHBoxLayout * mainLayout = new QHBoxLayout();
    QList<ParentFolder*> * folderList = new QList<ParentFolder*>;
    ParentFolder * folder;
};
#endif // MAINWINDOW_H
