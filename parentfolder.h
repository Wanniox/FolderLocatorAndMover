#ifndef PARENTFOLDER_H
#define PARENTFOLDER_H

#include "qlistwidget.h"
#include "clickablelabel.h"
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QListWidget>
#include <QSizePolicy>
#include <QPushButton>
#include <QDir>
#include <QFileInfoList>
#include <QLineEdit>
#include <QInputDialog>
#include <QMessageBox>
#include <windows.h>
#include <QThread>
#include <QProgressDialog>
#include <QQueue>
#include "moveworker.h"

class ParentFolder : public QWidget
{
    Q_OBJECT
public:
    explicit ParentFolder(QWidget *parent = nullptr, QString folderName = nullptr);
    ClickableLabel * getLabel();
    QString getLabelText();
    void setLabel(QString);
    void addItem(QListWidgetItem);
    void removeItem(int);
    QList<QListWidgetItem> getList();
    void fillList();
    int getLargestWidth();
public slots:
    void newButtonClicked();
    void deleteButtonClicked();
    void labelClicked();
    void updateButtons();
    void moveFinished(QString,bool);
    void moveButtonClicked();
    void processNextItem();
    void cancelled(QString,MoveWorker*,QThread*);
    void setValue(int);
signals:
    void deleteClicked();
    void labelSelected(QObject *);
    void refresh();
private:
    QQueue<QListWidgetItem*> itemQueue;
    QWidget * mainWindow;
    QListWidget* list = new QListWidget(this);
    ClickableLabel* label = new ClickableLabel(this);
    QPushButton *deleteButton;
    QPushButton *moveButton;
    QProgressDialog* progressDialog;
    QString destinationDirectory;
    QString sourceDirectory;
    int value = 0;
};

#endif // PARENTFOLDER_H
