#include "mainwindow.h"
#include <QApplication>
#include <QPixmap>
#include <QSplashScreen>
#include <QMessageBox>
#include <QTimer>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Splashscreen
    QPixmap pixmap(":/images/splash.png"); //Insert your splash page image here
    if(pixmap.isNull())
    {
        QMessageBox::warning(0, "Error", "Failed to load Splash Screen image!");
    }
    QSplashScreen splash(pixmap, Qt::WindowStaysOnTopHint);
    splash.setEnabled(false);           //Prevent user from closing the splash
    splash.show();

    a.processEvents();                  //Make sure splash screen gets drawn ASAP
    a.setOrganizationName("QtProject");
    a.setApplicationName("DirGister");
    a.setWindowIcon(QIcon(":/images/fa-list-alt_256_0_000000_none.png"));

    MainWindow w;


    QTimer::singleShot(1000, &splash, SLOT(close()));
    QTimer::singleShot(1000, &w, SLOT(show()));

    //splash.close();
    //w.show();

    return a.exec();
}
