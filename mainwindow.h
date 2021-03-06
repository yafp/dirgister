#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QTextEdit;
QT_END_NAMESPACE

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

private slots:
    void setSourceFolder();
    void setTargetFolder();
    void initValues();
    void resetLogUI();
    void checkingRequirements();
    void checkSrc();
    void resetSrc();
    void checkTarget();
    void resetTarget();
    void userTriggeredGeneration();
    void createSingleHTMLIndex(QString currentPath,QString targetFolder);
    void checkSubDirs(QString currentSubPath, QString currentTarget);

private:
    Ui::MainWindow *ui;
    void createActions();
    void createMenus();
    void createToolBars();
    void updateStatusBar(QString statusMessage);
    void readSettings();
    void writeSettings();
    QString generateTimestampString();


    QTextEdit *textEdit;

    QString appVersion;
    QString appName;

    QString srcFolder;
    QString targetFolder;
    QString oldTargetFolder;

    QMenu *fileMenu;
    QMenu *helpMenu;

    QAction *exitAct;
    QAction *aboutQtAct;

    QString newTimestampString;

    bool logFileCreationEnabled;
    bool srcFolderExists;
    bool targetFolderExists;
};

#endif // MAINWINDOW_H
