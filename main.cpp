#include "mainwindow.h"
#include <QApplication>
#include <QPixmap>
#include <QSplashScreen>
#include <QMessageBox>
#include <QTimer>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QPixmap pixmap(":/images/splash.png");                      // Insert your splash page image here
    QSplashScreen splash(pixmap, Qt::WindowStaysOnTopHint);
    splash.setEnabled(false);                                   // Prevent user from closing the splash
    splash.show();

    app.processEvents();                                          //Make sure splash screen gets drawn ASAP
    app.setOrganizationName("QtProject");
    app.setApplicationName("DirGister");
    app.setWindowIcon(QIcon(":/images/fa-list-alt_256_0_000000_none.png"));

    MainWindow w;

    QTimer::singleShot(1000, &splash, SLOT(close()));
    QTimer::singleShot(1000, &w, SLOT(show()));

    //splash.close();
    //w.show();

    return app.exec();
}
