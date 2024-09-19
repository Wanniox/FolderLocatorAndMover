#ifndef MOVEDIALOG_H
#define MOVEDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <algorithm>

class ParentFolder;

class MoveDialog : public QDialog
{
    Q_OBJECT

public:
    MoveDialog(const QList<ParentFolder*>& folders, QWidget *parent = nullptr);
    QString selectedDirectory() const;
    void sortListWidgetItems(QListWidget*);
private:
    QListWidget *listWidget;
    QPushButton *okButton;
    QPushButton *cancelButton;
};

#endif // MOVEDIALOG_H
