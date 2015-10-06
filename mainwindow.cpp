#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtWidgets>
#include "mainwindow.h"
#include <QDebug>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initValues();
    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    readSettings();



    // main UI
    connect(ui->bt_selectSource, SIGNAL(clicked()), this, SLOT(setSourceFolder()));
    connect(ui->bt_selectTarget, SIGNAL(clicked()), this, SLOT(setTargetFolder()));
    connect(ui->bt_generateIndex, SIGNAL(clicked()), this, SLOT(userTriggeredGeneration()));

    // about UI
    //
    // Issues Link
    ui->l_linkIssues->setText("<a href=\"https://github.com/yafp/dirgister/issues\">Issues</a>");
    ui->l_linkIssues->setTextFormat(Qt::RichText);
    ui->l_linkIssues->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->l_linkIssues->setOpenExternalLinks(true);
    // Wiki Link
    ui->l_linkWiki->setText("<a href=\"https://github.com/yafp/dirgister/wiki\">Wiki</a>");
    ui->l_linkWiki->setTextFormat(Qt::RichText);
    ui->l_linkWiki->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->l_linkWiki->setOpenExternalLinks(true);
    // Source Link
    ui->l_linkSource->setText("<a href=\"https://github.com/yafp/dirgister\">Source</a>");
    ui->l_linkSource->setTextFormat(Qt::RichText);
    ui->l_linkSource->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->l_linkSource->setOpenExternalLinks(true);

}



MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    //event->accept();
}



void MainWindow::initValues()
{
   appVersion = "20151006.01";      // App Version String

   ui->l_appTitle->setText("DirGister");
   ui->l_appVersion->setText(appVersion);
   ui->pte_aboutText->insertPlainText ("DirGister is a multiplattform directory indexer. It scans a source folder and writes a html-based index into a target-folder.\n");
}


void MainWindow::about()
{
  // QMessageBox::about(this, tr("About DirGister"),tr("<h2>DirGister</h2><p>... is a simple directory indexer by Florian Poeck.</p><p>It outputs HTML based index files making the entire content of the source folder browseable.<p><h3>Development</h3><p>Source, issues etc on <a href='https://github.com/yafp/dirgister'>Github</a>.</p>"));
}



void MainWindow::resetLogUI()
{
   ui->textEdit->clear();
   readSettings();
}









// ########################################################################
// USER SETS SOURCE and TARGET
// ########################################################################

// User defines a source folder
void MainWindow::setSourceFolder()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select folder to Index"),"/home",QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(dir != "")
    {
        ui->textEdit->moveCursor (QTextCursor::End);
        ui->textEdit->insertPlainText ("Source folder set to: "+dir+"\n");

        srcFolder = dir;
        ui->le_sourceFolder->setText(dir);

        writeSettings();
        resetLogUI();
    }


}


// User defines a target folder - should be writable
void MainWindow::setTargetFolder()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select target for Index"),"/home",QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    ui->textEdit->moveCursor (QTextCursor::End);
    ui->textEdit->insertPlainText ("Target folder set to: "+dir+"\n");

    targetFolder = dir;
    ui->le_targetFolder->setText(dir);

    writeSettings();
    resetLogUI();
}





// ########################################################################
// PRE - CHECKING REQUIREMENTS
// ########################################################################


// checks if the requirements are set to generate areport
void MainWindow::checkingRequirements()
{
    checkSrc();
    checkTarget();
}


// Check if source exists
void MainWindow::checkSrc()
{
    if(srcFolder != "")
    {
        if(QDir(srcFolder).exists())
        {
            srcFolderExists = true;
        }
        else
        {
            srcFolderExists = false;
        }
    }
    else
    {
        QMessageBox::about(this, tr("Error"),tr("Source folder is not defined yet."));
        srcFolderExists = false;
    }
}



// check if Target folder exists already
void MainWindow::checkTarget()
{
    if(targetFolder != "")
    {
        if(QDir(targetFolder).exists())
        {
             //qWarning() << "... Target folder already exists";
        }
        else
        {
             //QDir().mkdir("/home/foobar");
        }
        targetFolderExists = true;
    }
    else
    {
        QMessageBox::about(this, tr("Error"),tr("Target folder is not defined yet."));
        targetFolderExists = false;
    }

}







// ########################################################################
// GENERATE
// ########################################################################


// Creates an index.html showing all containing files and folders
void MainWindow::createSingleHTMLIndex(QString currentPath, QString targetFolder)
{
    // Generating Timestamp
    QDateTime dateTime = dateTime.currentDateTime();
    QString dateTimeString = dateTime.toString("yyyyMMdd-hhmmss");

    QString filename=targetFolder+"/index.html";
    QFile file( filename );
    if ( file.open(QIODevice::ReadWrite) )
    {
        QDir recoredDir(currentPath);

        QStringList allFolders = recoredDir.entryList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs);//(QDir::Filter::Files,QDir::SortFlag::NoSort)
        QStringList allFiles = recoredDir.entryList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::Files);//(QDir::Filter::Files,QDir::SortFlag::NoSort)

        // Create file
        QTextStream stream( &file );

        // deleting the former content
        file.write("");
        file.resize(file.pos());

        // write the new content
        stream << "<!DOCTYPE html>\n";
        stream << "<head>\n";
        stream << "<meta charset='UTF-8'>";
        stream << "<title>DirGister</title>\n";
        stream << "<script src='//code.jquery.com/jquery-1.11.3.min.js'></script>\n";
        stream << "<link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.3.0/css/font-awesome.min.css'>\n";
        stream << "<style>";
        stream << "body { margin:0px;  color: DimGray;}";
        stream << "h1 { margin-top: 0;}";
        stream << "h2 { color: DimGray; font-size: 20; border-bottom: solid 2px DimGray; padding-bottom: 6px;}";
        stream << "h3 { color: DimGray; font-size: 8;}";
        stream << "a { color: DimGray; text-decoration: none;}";
        stream << "a:visited { color: DimGray;}";
        stream << "#header { background-color: Gainsboro; color: DimGray; padding: 15px; margin-top: 0px; a{ color: DimGray;} }";
        stream << "#content {padding-left: 10px; padding-right: 10px; }";
        stream << "</style>";
        stream << "</head>\n";
        stream << "<body>\n";
        stream << "<div id='header'>";
        stream << "<h1><i class='fa fa-list-alt'></i>&nbsp;DirGister</h1>\n";


        // nur auf den unterseiten
        if(srcFolder != currentPath) // all sub-pages
        {
            stream << "<h3><a href='../index.html'><i class='fa fa-arrow-circle-left'></i></a>&nbsp;"+currentPath+"</h3>";
        }
        else // main-page
        {
            stream << "<h3>"+currentPath+"</h3>";
        }
        stream << "</div>"; // close header div
        stream << "<div id='content'>";


        if(allFolders.count() ==  0) // no folders
        {
            //qWarning() << "no folders found";
        }
        else
        {
            stream << "<h2><i class='fa fa-folder'></i>&nbsp;Folders ("+QString::number(allFolders.count())+")</h2>\n";
            QString newTarget;

            foreach (QString str, allFolders)
            {
                stream << "<a href='";
                stream << QString("%1").arg(str)+"/index.html";
                stream << "'><i class='fa fa-folder-o'></i>&nbsp;";
                stream << QString("%1").arg(str);
                stream << "</a><br>";

                newTarget = targetFolder+"/"+QString("%1").arg(str);

                QDir().mkdir(newTarget);
            }
        }


        if(allFiles.count() ==  0) // no files
        {
            //qWarning() << "no files found";
        }
        else
        {
            stream << "<h2><i class='fa fa-file'></i>&nbsp;Files ("+QString::number(allFiles.count())+")</h2>\n";
            foreach (QString str, allFiles)
            {
                QString icon = "";
                QString currentFile = QString("%1").arg(str);

                // typ: audio
                if((currentFile.contains(".mp3")) | (currentFile.contains(".wav")))
                {
                    icon = "<i class='fa fa-music'></i>";
                }

                // typ: image
                if((currentFile.contains(".jpg")) | (currentFile.contains(".png")) | (currentFile.contains(".gif")) | (currentFile.contains(".jpeg")))
                {
                    icon = "<i class='fa fa-image'></i>";
                }

                // typ: pdf
                if(currentFile.contains(".pdf"))
                {
                    icon = "<i class='fa fa-file-pdf-o'></i>";
                }

                // typ: text
                if((currentFile.contains(".txt")) | (currentFile.contains(".rtf")))
                {
                    icon = "<i class='fa fa-file-text'></i>";
                }

                // typ: movie
                if((currentFile.contains(".mov")) | (currentFile.contains(".mpg")) | (currentFile.contains(".mkv"))| (currentFile.contains(".flv")) | (currentFile.contains(".avi")) | (currentFile.contains(".mpeg")) | (currentFile.contains(".mp4")))
                {
                    icon = "<i class='fa fa-file-video-o'></i>";
                }

                // ELSE: lets check if icon is undefined - if so set a default file icon without specified typ
                if(icon == "")
                {
                    icon =  "<i class='fa fa-file-o'></i>";
                }

                stream << icon+"&nbsp;";
                stream << QString("%1").arg(str);
                stream << "<br>\n";
            }
        }
        stream << "</div>"; // close the content div
        stream << "</body>\n";
        stream << "</html>\n";

        file.close();

        ui->textEdit->moveCursor (QTextCursor::End);
        ui->textEdit->insertPlainText ("Creating: "+filename+"\n");
        ui->textEdit->repaint();


        // Issue on Fedora with qDebug()
        // Fedora: /etc/xdg/QtProject/qtlogging.ini
        // https://forum.qt.io/topic/54820/qt-qdebug-not-working-with-qconsoleapplication-or-qapplication/5

        checkSubDirs(currentPath, targetFolder);
    }
}



//  check subdirectories of current folder
void MainWindow::checkSubDirs(QString currentSubPath, QString currentTargetPath)
{
    ui->textEdit->moveCursor (QTextCursor::End);
    ui->textEdit->insertPlainText ("Checking subdirectories of "+currentSubPath+"\n\n");
    ui->textEdit->repaint();

    QDir recoredDir(currentSubPath);
    QStringList allFolders = recoredDir.entryList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs);
    foreach (QString str, allFolders)
    {
        QString currentSrcPath = currentSubPath+"/"+QString("%1").arg(str);
        QString newTargetPath = currentTargetPath+"/"+QString("%1").arg(str);

        createSingleHTMLIndex(currentSrcPath, newTargetPath );
    }
}



//
void MainWindow::userTriggeredGeneration()
{
    checkingRequirements();

    if((targetFolderExists == true) & (srcFolderExists == true))
    {
        QMessageBox msgBox;
        msgBox.setText("<qt>DirGister is going to create an index for<br><b>"+srcFolder+"</b><br>in<br><b>"+targetFolder+"</b>.</qt>");
        msgBox.setInformativeText("Do you want to proceed?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        int ret = msgBox.exec();

        // handle user-answer
        switch (ret)
        {
          case QMessageBox::Yes:
                ui->textEdit->setPlainText("...started index generation\n\n");

                createSingleHTMLIndex(srcFolder,targetFolder);

                ui->textEdit->moveCursor (QTextCursor::End);
                ui->textEdit->insertPlainText ("\n\nIndex generation finished.\n");

                QMessageBox::StandardButton reply;
                reply = QMessageBox::question(this, "Finished index generation", "Would you like to open and display it?",QMessageBox::Yes|QMessageBox::No);
                if (reply == QMessageBox::Yes)
                {
                    QString link = targetFolder+"/index.html";
                    QDesktopServices::openUrl(QUrl(link));
                }
                break;

          case QMessageBox::No:
                QMessageBox::about(this, tr("Aborted"),tr("You aborted the Index generation"));
                break;

          default:
              // should never be reached
              break;
        }
    }
    else
    {
        qWarning() << "Error: stopped report generation cause of missing requirements";
        QMessageBox::about(this, tr("Error"),tr("Requirements failed. Aborting."));
    }
}






// ########################################################################
// MENU, ACTIONS, TOOLBARS and STATUSBAR
// ########################################################################

// Creating Menu Actions
void MainWindow::createActions()
{
    setSourceFolderAct = new QAction(QIcon(":/images/iconSetSource.svg"), tr("Set source folder..."), this);
    //setSourceFolderAct->setShortcuts(QKeySequence::SaveAs);
    setSourceFolderAct->setStatusTip(tr("Select a source folder to index"));
    connect(setSourceFolderAct, SIGNAL(triggered()), this, SLOT(setSourceFolder()));

    setTargetFolderAct = new QAction(QIcon(":/images/iconSetTarget.svg"), tr("Set target folder..."), this);
    //setSourceFolderAct->setShortcuts(QKeySequence::SaveAs);
    setTargetFolderAct->setStatusTip(tr("Select a target folder for the index generation"));
    connect(setTargetFolderAct, SIGNAL(triggered()), this, SLOT(setTargetFolder()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));


    resetLogTextEditAct = new QAction(QIcon(":/images/iconTrash.svg"), tr("Reset Log..."), this);
    resetLogTextEditAct->setStatusTip(tr("Erases all log entries"));
    connect(resetLogTextEditAct, SIGNAL(triggered()), this, SLOT(resetLogUI()));


    generateHTMLAct = new QAction(QIcon(":/images/iconGenerate.svg"), tr("&Generate"), this);
    generateHTMLAct->setStatusTip(tr("Generates a new index for the selected folder"));
    connect(generateHTMLAct, SIGNAL(triggered()), this, SLOT(userTriggeredGeneration()));


    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));


    aboutQtAct = new QAction(tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}


// Add Actions to Menus
void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(exitAct);

    editMenu = menuBar()->addMenu(tr("&Functios"));
    editMenu->addAction(setSourceFolderAct);
    editMenu->addAction(setTargetFolderAct);
    editMenu->addSeparator();
    editMenu->addAction(generateHTMLAct);
    editMenu->addSeparator();
    editMenu->addAction(resetLogTextEditAct);

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
}


// Toolbars
void MainWindow::createToolBars()
{
    //fileToolBar = addToolBar(tr("File"));
    //fileToolBar->addAction(newAct);

    editToolBar = addToolBar(tr("Edit"));
    editToolBar->addAction(setSourceFolderAct);
    editToolBar->addAction(setTargetFolderAct);
    editToolBar->addAction(generateHTMLAct);
    editToolBar->addAction(resetLogTextEditAct);
}


// Statusbar
void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}





// ########################################################################
// READ and WRITE SETTINGS
// ########################################################################

void MainWindow::readSettings()
{
    QSettings settings("QtProject", "Dirgister");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    resize(size);
    move(pos);

    //qWarning() << "loading settings";
    srcFolder = settings.value("srcFolder").toString();
    targetFolder = settings.value("targetFolder").toString();

    //qWarning() << "- Reading Settings: SourceFolder: "+srcFolder+".";
    //qWarning() << "- Reading Settings: TargetFolder: "+targetFolder+".";

    ui->le_sourceFolder->setText(srcFolder);
    ui->le_targetFolder->setText(targetFolder);


    //ui->textEdit->setPlainText("Source folder: "+srcFolder+"\n");
    //ui->textEdit->moveCursor (QTextCursor::End);
    //ui->textEdit->insertPlainText ("Target folder: "+targetFolder+"\n");

    //qWarning() << "readSettings() finished";
}




void MainWindow::writeSettings()
{
    QSettings settings("QtProject", "Dirgister");
    settings.setValue("pos", pos());
    settings.setValue("size", size());

    if(srcFolder != "")
    {
        settings.setValue("srcFolder", srcFolder);
    }

    if(targetFolder != "")
    {
       settings.setValue("targetFolder", targetFolder);
    }
}





// ########################################################################
// MOST LIKELY NO LONGER NEEDED
// ########################################################################

/*
void MainWindow::documentWasModified()
{
    //setWindowModified(textEdit->document()->isModified());
}


QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}
*/

