#include "newparentfolderdialog.h"

NewParentFolderDialog::NewParentFolderDialog(QWidget *parent)
    : QDialog{parent}
{
    QVBoxLayout * layout = new QVBoxLayout(this);
    QHBoxLayout * inputLayout = new QHBoxLayout();
    QHBoxLayout * buttonLayout = new QHBoxLayout();
    QLabel * label = new QLabel("Folder:");
    input = new QLineEdit(this);
    QPushButton * setDirectory = new QPushButton("Select");
    setDirectory->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    inputLayout->addWidget(label);
    inputLayout->addWidget(input);
    inputLayout->addWidget(setDirectory);
    QPushButton * ok = new QPushButton("Ok");
    ok->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    QPushButton * cancel = new QPushButton("Cancel");
    cancel->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    buttonLayout->addWidget(ok);
    buttonLayout->addWidget(cancel);
    buttonLayout->setAlignment(Qt::AlignRight);
    layout->addLayout(inputLayout);
    layout->addLayout(buttonLayout);
    connect(ok,&QPushButton::clicked,this,&NewParentFolderDialog::okClicked);
    connect(cancel,&QPushButton::clicked,this,&NewParentFolderDialog::reject);
    connect(setDirectory,&QPushButton::clicked,this,&NewParentFolderDialog::openDirectory);
}

void NewParentFolderDialog::okClicked() {
    emit title(input->text());
    accept();
}

void NewParentFolderDialog::openDirectory() {
    QString directory = QFileDialog::getExistingDirectory(this,tr("Open Directory"),"",QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
    if (!directory.isEmpty()) {
        input->setText(directory);
    } else {
        QMessageBox::critical(this, tr("Error"), tr("Could not open directory"));
    }
}
