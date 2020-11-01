#include "mainwindow.h"
#include <QApplication>
#include <QtCore>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QString appdir("test/LiteWearable");
    if (argc == 2){
        appdir = argv[1];
    }else{
        QDir dir(appdir);
        if (!dir.exists()){
            appdir = "../../test/LiteWearable";
        }
    }
    MainWindow w(appdir);
    w.show();
    return a.exec();
}
