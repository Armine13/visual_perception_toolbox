#include "GUI/mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

//    a.sendEvent(&w, QEvent::Paint);
//    connect(w, SIGNAL(updateImage()), a, SLOT(sendEvent(mainWindow, &event)));
//    connect(&w, SIGNAL(updateImage()), a, SLOT
//     QCoreApplication::sendEvent(QObject * receiver, QEvent * event)
    w.show();

    return a.exec();
}
