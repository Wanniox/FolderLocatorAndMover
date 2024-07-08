
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("FLAM");
    ui->centralwidget->setLayout(mainLayout);
    connect(ui->actionNew_folder,&QAction::triggered,this,&MainWindow::newParentFolder);
    connect(ui->actionSave,&QAction::triggered,this,&MainWindow::save);
    connect(ui->actionLoad,&QAction::triggered,this,&MainWindow::load);
    connect(ui->actionDelete,&QAction::triggered,this,&MainWindow::deleteParentFolder);

    load();
}

MainWindow::~MainWindow()
{
    delete ui;
}

QList<ParentFolder*>* MainWindow::getFolders() {
    return folderList;
}

void MainWindow::newParentFolder() {
    QString directory = QFileDialog::getExistingDirectory(this,tr("Open Directory"),"",QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
    if (!directory.isEmpty()) {
        if (directory != "") {
            for (int i = 0; i<folderList->count();++i) {
                if (directory == folderList->at(i)->getLabelText()){
                    QMessageBox::critical(this,tr("Error"),tr("This folder is already used!"));
                    return;
                }
            }
            addNewParentFolder(directory);
        }
    }
}

void MainWindow::addNewParentFolder(QString directory) {
    folder = new ParentFolder(this, directory);
    connect(folder,&ParentFolder::labelSelected,this,&MainWindow::labelClickedSlot);
    connect(folder,&ParentFolder::refresh,this,&MainWindow::refreshDirectories);
    folderList->append(folder);
    mainLayout->addWidget(folder);
    refreshDirectories();
}

void MainWindow::refreshDirectories() {
    int size = 0;
    for (int i = 0; i<folderList->count(); ++i) {
        folderList->at(i)->fillList();
        int currentSize = folderList->at(i)->getLargestWidth();
        if (size < currentSize) {
            size = currentSize;
        }
    }
    for (int i = 0; i<folderList->count(); ++i) {
        folderList->at(i)->setMinimumWidth(size);
    }
}

void MainWindow::labelClickedSlot(QObject * firstSender) {
    for (int i = 0; i<folderList->count(); ++i) {
        if (folderList->at(i)->getLabel()!=firstSender) {
            folderList->at(i)->getLabel()->setSelectedNoSignal(false);
        } else {
            if (folderList->at(i)->getLabel()->isSelected()) {
                ui->actionDelete->setEnabled(false);
            } else {
                ui->actionDelete->setEnabled(true);
            }
        }
    }
}

void MainWindow::deleteParentFolder() {
    for (int i = 0; i<folderList->count(); ++i){
        if (folderList->at(i)->getLabel()->isSelected()) {
            ParentFolder * temp = folderList->takeAt(i);
            mainLayout->removeWidget(temp);
            temp->deleteLater();
            refreshDirectories();
            break;
        }
    }
    ui->actionDelete->setEnabled(false);
}

void MainWindow::save() {
    QStringList homePath = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
    QString filename = homePath.first().split(QDir::separator()).last() + "/Documents/default.ini";
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly)){
        QTextStream stream (&file);
        for(int i = 0; i<folderList->count();++i){
            stream << folderList->at(i)->getLabelText() << Qt::endl;
        }
    }
    file.close();
}

void MainWindow::load() {
    QStringList homePath = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
    QString filename = homePath.first().split(QDir::separator()).last() + "/Documents/default.ini";
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly)){
        QTextStream stream(&file);
        while (!stream.atEnd()) {
            addNewParentFolder(stream.readLine());
        }
        file.close();
        refreshDirectories();
    }
}
