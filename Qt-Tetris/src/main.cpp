#include <QApplication>
#include <QStyleHints>
#include "ui/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // 深色
    QGuiApplication::styleHints()->setColorScheme(Qt::ColorScheme::Dark);

    MainWindow w;
    w.show();

    return a.exec();
}
