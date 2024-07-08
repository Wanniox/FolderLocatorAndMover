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
    QHBoxLayout * hlayout = new QHBoxLayout(); //No parent is okay here because I will set the parent layout with addLayout() method. B I G  B R A I N
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

    int maxWidth = 0;
    QFontMetrics fm(font());
    for (int i = 0; i<list->count();++i) {
        int textWidth = fm.horizontalAdvance(list->item(i)->text());
        maxWidth = std::max(maxWidth, std::max(textWidth, fm.horizontalAdvance(label->text())+20));
    }

    int minWidth = qMax(label->width(), maxWidth + 10);
    return minWidth;
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

    QListWidgetItem* item = itemQueue.dequeue();
    QString folderName = item->text();
    qDebug() << "Starting move for folder: " << folderName;
    progressDialog = new QProgressDialog("Moving " + item->text(), "Cancel", 0, 100, this);
    progressDialog->setWindowModality(Qt::NonModal);

    MoveWorker* worker = new MoveWorker(item, destinationDirectory, sourceDirectory);
    QThread* thread = new QThread;
    worker->moveToThread(thread);

    connect(worker, &MoveWorker::finished, this, [=] { moveFinished(folderName); });
    connect(worker, &MoveWorker::finished, thread, &QThread::quit);
    connect(worker, &MoveWorker::finished, worker, &MoveWorker::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    connect(progressDialog, &QProgressDialog::canceled, this, [=] {ParentFolder::quitTransfer(thread);});
    connect(worker, &MoveWorker::progressChanged, progressDialog, &QProgressDialog::setValue);

    thread->start();
    QMetaObject::invokeMethod(worker, "moveFolders", Qt::QueuedConnection);
}

void ParentFolder::quitTransfer(QThread * thread) {
    thread->quit();
}

void ParentFolder::moveFinished(QString folder) {
    progressDialog->reject();
    QMessageBox::information(this,"Folder transferred successfully", folder+" has been transferred successfully!");
    if (!itemQueue.isEmpty()) {
        processNextItem();
    } else {
        emit refresh();
    }
}
