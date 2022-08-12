#include "mainwindow.h"
#include <QApplication>
#include <QProcess>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    if (argc == 2 && strcmp(argv[1], "-backround"))
    {
        w.hide();
    }
    else
    {
        w.show();
    }
    return a.exec();
}
