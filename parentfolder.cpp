#include "parentfolder.h"
#include "mainwindow.h"
#include "movedialog.h"
#include "moveworker.h"

ParentFolder::ParentFolder(QWidget *parent, QString folderName)
    : QWidget{parent}
{
    list->setSelectionMode(QAbstractItemView::ExtendedSelection);
    mainWindow = parent;
    label->setText(folderName);
    QPushButton *newButton = new QPushButton(this);
    newButton->setText("New");
    newButton->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    deleteButton = new QPushButton(this);
    deleteButton->setText("Delete");
    deleteButton->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    deleteButton->setEnabled(false);
    moveButton = new QPushButton(this);
    moveButton->setText("Move");
    moveButton->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    moveButton->setEnabled(false);
    QVBoxLayout* layout = new QVBoxLayout(this);
    QHBoxLayout * hlayout = new QHBoxLayout();
    hlayout->addWidget(newButton);
    hlayout->addWidget(moveButton);
    hlayout->addWidget(deleteButton);
    hlayout->setAlignment(Qt::AlignLeft);
    layout->addWidget(label);
    layout->addLayout(hlayout);
    layout->addWidget(list);
    connect(newButton,&QPushButton::clicked,this,&ParentFolder::newButtonClicked);
    connect(moveButton,&QPushButton::clicked,this,&ParentFolder::moveButtonClicked);
    connect(deleteButton,&QPushButton::clicked,this,&ParentFolder::deleteButtonClicked);
    connect(label,&ClickableLabel::clicked,this,&ParentFolder::labelClicked);
    connect(list,&QListWidget::itemSelectionChanged,this,&ParentFolder::updateButtons);

    fillList();

    QFontMetrics fm(font());
    int totalHeight = list->count() * fm.height() + 150;

    setMinimumHeight(totalHeight);
}

int ParentFolder::getLargestWidth() {

    QFont boldFont = font();
    boldFont.setBold(true);
    int width = 0;
    QFontMetrics fm(boldFont);
    if (list->count() != 0) {
        for (int i = 0; i<list->count();++i) {
            int textWidth = fm.horizontalAdvance(list->item(i)->text());
            width = std::max(250, std::max(textWidth+20, fm.horizontalAdvance(label->text())+20));
        }
    } else {
        width = std::max(250, fm.horizontalAdvance(label->text())+20);
    }
    return width;
}

QString ParentFolder::getLabelText() {
    return label->text();
}

ClickableLabel * ParentFolder::getLabel(){
    return label;
}

void ParentFolder::setLabel(QString text) {
    label->setText(text);
}

void ParentFolder::addItem(QListWidgetItem item) {
    list->addItem(&item);
}

void ParentFolder::removeItem(int index) {
    list->takeItem(index);
}

QList<QListWidgetItem> ParentFolder::getList(){
    QList<QListWidgetItem> itemsList;
    for (int i = 0; i < list->count(); ++i) {
        itemsList.append(*list->item(i));
    }
    return itemsList;
}

void ParentFolder::updateButtons() {
    bool itemsSelected = !list->selectedItems().isEmpty();
    deleteButton->setEnabled(itemsSelected);
    moveButton->setEnabled(itemsSelected);
}

void ParentFolder::fillList() {
    list->clear();
    QDir directory(label->text());
    QFileInfoList files = directory.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    foreach(QFileInfo file, files) {
        QListWidgetItem *item = new QListWidgetItem(file.fileName(), list);
        list->addItem(item);
    }
}

void ParentFolder::labelClicked() {
    emit labelSelected(QObject::sender());
}

void ParentFolder::newButtonClicked() {
    bool ok;
    QString newFolderName = QInputDialog::getText(this, tr("New Folder"),tr("Folder name:"), QLineEdit::Normal, QString(), &ok);
    if (ok && !newFolderName.isEmpty()) {
        QDir dir(label->text());
        if (dir.exists(newFolderName)) {
            QMessageBox::warning(this, tr("Error"), tr("Folder already exists!"));
        } else {
            if (dir.mkdir(newFolderName)) {
                QListWidgetItem *item = new QListWidgetItem(newFolderName, list);
                list->addItem(item);
                emit refresh();
            } else {
                QMessageBox::critical(this, tr("Error"), tr("Failed to create folder!"));
            }
        }
    }
}

void ParentFolder::deleteButtonClicked() {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this,tr("Are you sure?"), tr("Are you sure you want to delete the folder(s) and all its contents?"));
    if (reply == QMessageBox::Yes){
        QList<QListWidgetItem *> selectedItems = list->selectedItems();
        foreach(QListWidgetItem * item, selectedItems) {
            QString folderName = item->text();
            QDir dir(label->text() + "/" + folderName);
            if (dir.exists()) {
                if (dir.removeRecursively()) {
                    delete list->takeItem(list->row(item));
                    QMessageBox::information(this, tr("Completed"), tr("Folder %1 deleted!").arg(folderName));
                } else {
                    QMessageBox::warning(this, tr("Error"), tr("Failed to delete folder: %1").arg(folderName));
                }
            }
        }
    }
}

void ParentFolder::moveButtonClicked() {
    QList<QListWidgetItem *> selectedItems = list->selectedItems();
    MoveDialog dialog(*dynamic_cast<MainWindow*>(mainWindow)->getFolders(), this);
    if (dialog.exec() == QDialog::Accepted) {
        destinationDirectory = dialog.selectedDirectory();
        sourceDirectory = label->text();

        foreach(QListWidgetItem* item, selectedItems) {
            itemQueue.enqueue(item);
        }
        processNextItem();
    }
}

void ParentFolder::processNextItem() {
    if (itemQueue.isEmpty()) {
        return;
    }
    value = 0;
    QListWidgetItem* item = itemQueue.dequeue();
    QString folderName = item->text();
    qDebug() << "Starting move for folder:" << folderName;
    progressDialog = new QProgressDialog("Moving " + item->text(), "Cancel", 0, 100, this);
    progressDialog->setWindowModality(Qt::ApplicationModal);
    progressDialog->setMinimumDuration(0);
    progressDialog->show();

    MoveWorker* worker = new MoveWorker(item, destinationDirectory, sourceDirectory);
    QThread* thread = new QThread;
    worker->moveToThread(thread);

    connect(worker, &MoveWorker::finished, this, [=] {moveFinished(folderName, value);});
    connect(worker, &MoveWorker::finished, thread, &QThread::quit);
    connect(worker, &MoveWorker::finished, worker, &MoveWorker::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    connect(progressDialog, &QProgressDialog::canceled, this, [=] {ParentFolder::cancelled(folderName,worker,thread);});
    connect(worker, &MoveWorker::progressChanged, progressDialog, &QProgressDialog::setValue);
    connect(worker, &MoveWorker::progressChanged, this, &ParentFolder::setValue);

    thread->start();
    QMetaObject::invokeMethod(worker, "moveFolders", Qt::QueuedConnection);
}

void ParentFolder::setValue(int workerValue) {
    value = workerValue;
}

void ParentFolder::moveFinished(QString folder, int value) {
    qDebug() << "Value:" << value;
    if (value == 100) {
        progressDialog->reject();
        QMessageBox::information(this,tr("Folder transferred successfully"), tr("%1 has been transferred successfully!").arg(folder));
    }
    if (!itemQueue.isEmpty()) {
        processNextItem();
    } else {
        emit refresh();
    }
}

void ParentFolder::cancelled(QString name,MoveWorker*worker,QThread*thread) {
    //THIS IS SO BAD D:
    thread->terminate();
    thread->wait();
    thread->deleteLater();
    emit worker->finished();
    QMessageBox::warning(this,tr("Cancelled!"),tr("Transferring %1 has been cancelled! Note that duplicate folders might be visible!").arg(name));
}
