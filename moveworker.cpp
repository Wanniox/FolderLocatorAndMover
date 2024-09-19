#include "MoveWorker.h"

MoveWorker::MoveWorker(QListWidgetItem *selectedItem, const QString &destinationDirectory, const QString &sourceDirectory, QObject *parent)
    : QObject(parent), selectedItem(selectedItem), destinationDirectory(destinationDirectory), sourceDirectory(sourceDirectory) {}

void MoveWorker::moveFolders() {

    QString folder = selectedItem->text();
    QDir sourceDir(sourceDirectory);
    QDir destDir(destinationDirectory);

    QString sourceDrive = sourceDir.absolutePath().left(2);
    QString destDrive = destDir.absolutePath().left(2);

    if (sourceDir.exists() && (sourceDrive == destDrive)) {
        if (sourceDir.rename(sourceDir.absolutePath() + "/" + folder, destDir.absolutePath() + "/" + folder)) {
            emit progressChanged(100);
        } else {
            emit progressChanged(0);
            emit errorMessage("Failed to move folder.");
            emit finished(false);
            return;
        }
    } else if (sourceDir.exists()) {
        QString sourceFolderPath = sourceDir.absolutePath() + "/" + folder;
        QString destFolderPath = destDir.absolutePath() + "/" + folder;

        QDir sourceFolder(sourceFolderPath);

        qint64 currentSize = 0;
        qint64 size = dirSize(sourceFolderPath);
        qDebug() << "Directory size: " << size;
        if (copyFolder(sourceFolder, destFolderPath, size, currentSize)) {
            qDebug() << "copy folder positive!";
            if (! deleteFolder(sourceFolderPath)) {
                emit progressChanged(0);
                emit errorMessage("Failed to delete the source folder.");
                emit finished(false);
                return;
            }
        } else {
            qDebug() << "copy folder negative!";
            if (quit) {
                qDebug() << "quit positive!";
                emit errorMessage("Transfer cancelled.");
            } else {
                emit progressChanged(0);
                emit errorMessage("Failed to copy folder.");
                emit finished(false);
                return;
            }
        }
    }
    emit finished(true);
}

qint64 MoveWorker::dirSize(QString source) {
    qint64 size = 0;
    QDir dir(source);
    QDir::Filters fileFilters = QDir::Files|QDir::System|QDir::Hidden;
    for(QString filePath : dir.entryList(fileFilters)) {
        QFileInfo fi(dir, filePath);
        size+= fi.size();
    }
    QDir::Filters dirFilters = QDir::Dirs|QDir::NoDotAndDotDot|QDir::System|QDir::Hidden;
    for(QString childDirPath : dir.entryList(dirFilters))
        size+= dirSize(source + QDir::separator() + childDirPath);
    return size;
}

bool MoveWorker::copyFolder(const QDir &source, const QString &destination, const qint64 &size, qint64 &currentSize) {
    QDir destDir(destination);
    if (!destDir.exists()) {
        if (!destDir.mkpath(".")) {
            return false;
        }
    }

    foreach (QString fileName, source.entryList(QDir::Files)) {
        if (quit) {
            qDebug() << "quit returning false!";
            return false;
        }
        QString srcFilePath = source.absoluteFilePath(fileName);
        QFile file(srcFilePath);
        qint64 fileSize = file.size();
        file.close();
        QString destFilePath = destDir.absoluteFilePath(fileName);
        if (!QFile::copy(srcFilePath, destFilePath)) {
            return false;
        } else {
            currentSize += fileSize;
            float currentSizeInGB = static_cast<float>(currentSize) / 1000000000.3f;
            float sizeInGB = static_cast<float>(size) / 1000000000.3f;
            QString currentSizeStr = QString::number(currentSizeInGB, 'f', 3);
            QString sizeStr = QString::number(sizeInGB, 'f', 3);
            qDebug().noquote() << QString("Progress: %1% (%2 GB of %3 GB)").arg((qint64)(currentSize * 100 / size)).arg(currentSizeStr).arg(sizeStr);
            emit progressChanged(((qint64)currentSize*100/size));
        }
    }

    foreach (QString dirName, source.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        QString srcDirPath = source.absoluteFilePath(dirName);
        QString destDirPath = destDir.absoluteFilePath(dirName);
        if (!copyFolder(QDir(srcDirPath), destDirPath, size, currentSize)) {
            return false;
        }
    }
    return true;
}

bool MoveWorker::deleteFolder(const QString &folderPath) {
    QDir dir(folderPath);

    foreach (QString fileName, dir.entryList(QDir::Files)) {
        QString filePath = dir.absoluteFilePath(fileName);
        QFile::remove(filePath);
    }

    foreach (QString dirName, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        QString childDirPath = dir.absoluteFilePath(dirName);
        if (!deleteFolder(childDirPath)) {
            return false;
        }
    }

    return dir.rmdir(dir.absolutePath());
}

void MoveWorker::stop() {
    quit = true;
}
