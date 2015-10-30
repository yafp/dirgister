#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QPlainTextEdit;
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
    void about();
    void initValues();
    void resetLogUI();

    void checkingRequirements();
    void checkSrc();
    void checkTarget();
    void userTriggeredGeneration();
    void createSingleHTMLIndex(QString currentPath,QString targetFolder);
    void checkSubDirs(QString currentSubPath, QString currentTarget);



private:
    Ui::MainWindow *ui;
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    QString generateTimestampString();


    QString strippedName(const QString &fullFileName);
    QPlainTextEdit *textEdit;
    QString curFile;
    QString appVersion;
    QString targetFolder;
    QString srcFolder;
    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *helpMenu;
    QToolBar *fileToolBar;
    QToolBar *editToolBar;
    QAction *setSourceFolderAct;
    QAction *setTargetFolderAct;
    QAction *exitAct;
    QAction *generateHTMLAct;
    QAction *resetLogTextEditAct;
    QAction *aboutQtAct;

    QString newTimestampString;


    bool logFileCreationEnabled;
    bool srcFolderExists;
    bool targetFolderExists;


};

#endif // MAINWINDOW_H
