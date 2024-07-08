#ifndef MOVEWORKER_H
#define MOVEWORKER_H

#include <QObject>
#include <QDir>
#include <QFile>
#include <QListWidgetItem>
#include <QProgressDialog>

class MoveWorker : public QObject {
    Q_OBJECT

public:
    explicit MoveWorker(QListWidgetItem * selectedItem, const QString &destinationDirectory, const QString &sourceDirectory, QObject *parent = nullptr);
    qint64 dirSize(QString);
signals:
    void progressChanged(int);
    void finished();
    void errorMessage(const QString&);
public slots:
    void moveFolders();

private:
    QListWidgetItem * selectedItem;
    QString destinationDirectory;
    QString sourceDirectory;

    bool copyFolder(const QDir &, const QString &, const qint64 &, qint64 &);
    bool deleteFolder(const QString &);
};

#endif // MOVEWORKER_H
