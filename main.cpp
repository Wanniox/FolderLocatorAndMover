#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}

//TODO: Change shortcuts to new folder.
//TODO: Search (sub)folders.
//TODO: Double clicking a subfolder should open the file explorer in that folder.
//TODO: Refresh button in toolbar.
//TODO: Display source and destination directories in progressDialog.
//TODO: Allow cancelling of transfer.
//TODO: Check for duplicates button. Keep largest folder question dialog?
//TODO: add user setting to omit folders by size.
//MAYBE: Switch to single tablewidget instead of multiple listviews (user setting) <--- THIS IS FUCKED or just create new qtablewidget class
