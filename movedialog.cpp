#include "movedialog.h"
#include "parentfolder.h"

MoveDialog::MoveDialog(const QList<ParentFolder*>& folders, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Select Destination Folder");

    listWidget = new QListWidget(this);
    okButton = new QPushButton("OK", this);
    cancelButton = new QPushButton("Cancel", this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->addWidget(okButton);
    hlayout->addWidget(cancelButton);
    layout->addWidget(listWidget);
    layout->addLayout(hlayout);

    connect(okButton, &QPushButton::clicked, this, &MoveDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &MoveDialog::reject);

    for (auto folder : folders) {
        if (folder != dynamic_cast<ParentFolder*>(parent)) {
            QListWidgetItem *item = new QListWidgetItem(folder->getLabelText(), listWidget);
            listWidget->addItem(item);
        }
    }
    //sortListWidgetItems(listWidget);

    int maxWidth = 0;
    QFontMetrics fm(font());
    for (int i = 0; i<listWidget->count();++i) {
        int textWidth = fm.horizontalAdvance(listWidget->item(i)->text());
        maxWidth = std::max(maxWidth, textWidth);
    }

    int minWidth = qMax(400, maxWidth + 100);

    setMinimumSize(minWidth, 200);
}

void MoveDialog::sortListWidgetItems(QListWidget* listWidget) {
    QList<QListWidgetItem*> items;
    while (listWidget->count() > 0) {
        items.append(listWidget->takeItem(0));
    }

    std::sort(items.begin(), items.end(), [](QListWidgetItem* a, QListWidgetItem* b) {
        return a->text() < b->text();
    });

    listWidget->clear();

    for (QListWidgetItem* item : items) {
        listWidget->addItem(item);
    }
}

QString MoveDialog::selectedDirectory() const {
    QListWidgetItem *item = listWidget->currentItem();
    if (item) {
        return item->text();
    }
    return QString();
}
