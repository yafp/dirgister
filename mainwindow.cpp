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
    resetLogUI();
    createActions();
    createMenus();
    readSettings();
    checkingRequirements();
    updateStatusBar("DirGister initialized");
}



MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
}


// Init some variables & UI-elements on app start
void MainWindow::initValues()
{
   appVersion = "20151103.01";      // App Version
   appName = "DirGister";           // App Name

   this->setWindowTitle(appName + " ("+ appVersion +")");


   // TAB: main
   //
   connect(ui->bt_selectSource, SIGNAL(clicked()), this, SLOT(setSourceFolder()));
   connect(ui->bt_selectTarget, SIGNAL(clicked()), this, SLOT(setTargetFolder()));
   connect(ui->bt_generateIndex, SIGNAL(clicked()), this, SLOT(userTriggeredGeneration()));

   ui->le_sourceFolder->setDisabled(true);
   ui->le_targetFolder->setDisabled(true);
   ui->rb_writeLog->setText("Enable log file creation");
   ui->l_logDescription->setText("<font color='grey'><i>The target folder will contain the generated log</i></font>");

    // reset source
   QPixmap pixmap(":/images/fa-trash-o_128_0_000000_none.png");
   QIcon ButtonIcon(pixmap);
   ui->tb_resetSource->setIcon(ButtonIcon);
   connect(ui->tb_resetSource, SIGNAL(clicked()), this, SLOT(resetSrc()));
   //ui->tb_resetSource->setIconSize(pixmap.rect().size());
   ui->tb_resetTarget->setIcon(ButtonIcon);
   connect(ui->tb_resetTarget, SIGNAL(clicked()), this, SLOT(resetTarget()));

   QPixmap pixmap4GenerateButton(":/images/fa-pencil_128_0_000000_none.png");
   QIcon ButtonIcon4GenerateButton(pixmap4GenerateButton);
   ui->bt_generateIndex->setIcon(ButtonIcon4GenerateButton);



   // TAB: about
   //
   ui->l_appTitle->setText(appName);
   ui->l_appVersion->setText(appVersion);
   ui->pte_aboutText->insertPlainText ("DirGister is a multi-platform directory indexer with HTML output written by Florian Poeck.\n\nHow it works:\n- You define a source folder and a target folder\n- It scans the source folder and\n- writes a html-based browseable index into the target-folder\n\n\nDeveloped under MIT license.");
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

   // set default-tab
   ui->tabWidget->setCurrentIndex(0);
}



// Reset the log textedit
//
void MainWindow::resetLogUI()
{
   ui->textEdit->clear();
   ui->textEdit->setReadOnly(true);
   ui->textEdit->setEnabled(false);
   ui->textEdit->setText("<center><font color='Lightgray' size='20'><br><b>DirGister Log</b></font></center>");
   ui->textEdit->setStyleSheet("QTextEdit { background-color: rgb(220, 220, 220) }");
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
        checkingRequirements();
    }
}


// User defines a target folder - should be writable
void MainWindow::setTargetFolder()
{
    QFileInfo userdefinedDir = QFileDialog::getExistingDirectory(this, tr("Select target for Index"),"/home",QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if(userdefinedDir.isDir() && userdefinedDir.isWritable()) // check user-selected input-folder
    {
        QString dir = userdefinedDir.absoluteFilePath();

        targetFolder = dir;
        ui->le_targetFolder->setText(dir);

        writeSettings();
        resetLogUI();
        checkingRequirements();

    }
    else // is not writeable
    {

        //qWarning() << "not writable";
        QMessageBox::about(this, tr("Error"),tr("Target folder is not writeable"));
    }
}



// ########################################################################
// USER resets SOURCE and/or TARGET
// ########################################################################

// User resets the source folder
void MainWindow::resetSrc()
{
    srcFolder = "";
    ui->le_sourceFolder->setText("");
    checkingRequirements();
    writeSettings();
}

// User resets the target folder
void MainWindow::resetTarget()
{
    targetFolder = "";
    ui->le_targetFolder->setText("");
    checkingRequirements();
    writeSettings();
}





// ########################################################################
// PRE - CHECKING REQUIREMENTS
// ########################################################################

// checks if the requirements are set to generate a report
void MainWindow::checkingRequirements()
{
    checkSrc();
    checkTarget();

    if((srcFolderExists == true) && (targetFolderExists == true)) // src & target exist
    {
        ui->bt_generateIndex->setEnabled(true);
    }
    else
    {
        ui->bt_generateIndex->setEnabled(false);
    }
    updateStatusBar("Finished checking requirements (source and target)");
}


// Check if source exists
void MainWindow::checkSrc()
{
    if(srcFolder != "") // if a src folder is defined
    {
        if(QDir(srcFolder).exists())
        {
            srcFolderExists = true;
        }
        else
        {
            srcFolderExists = false;
            ui->le_sourceFolder->setText(""); // reset UI
        }
    }
    else
    {
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
             targetFolderExists = true;
        }
        else
        {
            targetFolderExists = false;
            ui->le_targetFolder->setText(""); // reset UI
        }
    }
    else
    {
        targetFolderExists = false;
    }
}







// ########################################################################
// GENERATE
// ########################################################################

// User started index-generation process
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
          {
                updateStatusBar("Started index generation");
                resetLogUI();
                ui->textEdit->clear();              // to overwrite the fontsettings of resetLogUI
                ui->textEdit->setEnabled(true);     // makes text selectable again
                ui->textEdit->setPlainText("");     //

                newTimestampString = generateTimestampString();
                ui->textEdit->insertPlainText(newTimestampString+ " - Started index generation for:\n");

                newTimestampString = generateTimestampString();
                ui->textEdit->insertPlainText(newTimestampString+ " - Source: "+ srcFolder +"\n");

                newTimestampString = generateTimestampString();
                ui->textEdit->insertPlainText(newTimestampString+ " - Target: "+ targetFolder +"\n\n");

                // check if log file creation is enabled or not
                if(ui->rb_writeLog->isChecked())
                {
                    logFileCreationEnabled=true;
                }
                else
                {
                    logFileCreationEnabled=false;
                }

                // disable some UI items while index-generation is running
                //
                ui->le_sourceFolder->setEnabled(false);
                ui->bt_selectSource->setEnabled(false);
                ui->le_targetFolder->setEnabled(false);
                ui->bt_selectTarget->setEnabled(false);
                ui->rb_writeLog->setEnabled(false);             // disable the ui-item until index-generation is finished
                ui->bt_generateIndex->setEnabled(false);        // disable generate-index-button


                // create a new target folder in the user-defined target-folder
                oldTargetFolder =targetFolder;
                newTimestampString = generateTimestampString();
                targetFolder = targetFolder+"/"+newTimestampString+"_DirGister_Index";
                QDir().mkdir(targetFolder);

                // start the index generation
                createSingleHTMLIndex(srcFolder,targetFolder);

                // If logging is enabled, copy content of UI-log to the log file
                if(logFileCreationEnabled == true)
                {
                    newTimestampString = generateTimestampString();
                    QString logfileName=targetFolder+"/"+ newTimestampString +"_DirGister_log.txt";
                    QFile logFile( logfileName );
                    if ( logFile.open(QIODevice::ReadWrite) )
                    {
                        QTextStream stream( &logFile );

                        // deleting the former content
                        logFile.write("");
                        logFile.resize(logFile.pos());

                        // get text from textedit
                        QString logText = ui->textEdit->toPlainText();
                        stream << logText;

                        logFile.close();
                    }
                }

                // re-enable the GUI items
                ui->le_sourceFolder->setEnabled(true);
                ui->bt_selectSource->setEnabled(true);
                ui->le_targetFolder->setEnabled(true);
                ui->bt_selectTarget->setEnabled(true);
                ui->rb_writeLog->setEnabled(true);              // disable the ui-item until index-generation is finished
                ui->bt_generateIndex->setEnabled(true);         // disable generate-index-button

                ui->textEdit->moveCursor (QTextCursor::End);    // jump to end of UI-log-textedit
                newTimestampString = generateTimestampString();
                ui->textEdit->insertPlainText ("\n"+newTimestampString+" - Index generation finished.\n");
                ui->textEdit->moveCursor (QTextCursor::End);    // jump to end of UI-log-textedit

                // offer option to load the result index in default viewer
                QMessageBox::StandardButton reply;
                reply = QMessageBox::question(this, "Finished index generation", "Would you like to open and display it?",QMessageBox::Yes|QMessageBox::No);
                if (reply == QMessageBox::Yes)
                {
                    QString link = targetFolder+"/index.html";
                    QDesktopServices::openUrl(QUrl(link));
                }
                targetFolder = oldTargetFolder; // revert to old content -> targetFolder

                break;
          }

          case QMessageBox::No:
                {
                    QMessageBox::about(this, tr("Aborted"),tr("You aborted the Index generation"));
                    break;
                }

          default:
                {
                    // should never be reached
                    break;
                }
        }
    }
    else // either src- or target-folder doesnt exist
    {
        QMessageBox::about(this, tr("Error"),tr("Requirements failed. Aborting."));
    }
}




// Creates an index.html showing all containing files and folders for the folder/path submitted to the method
void MainWindow::createSingleHTMLIndex(QString currentPath, QString targetFolder)
{
    // write log
    ui->textEdit->moveCursor (QTextCursor::End);
    newTimestampString = generateTimestampString();
    ui->textEdit->insertPlainText ("\n"+newTimestampString+ "- Indexing: "+currentPath+"\n");

    QString filename=targetFolder+"/index.html";
    QFile file( filename );
    if ( file.open(QIODevice::ReadWrite) )
    {
        QDir recoredDir(currentPath);

        QStringList allFolders = recoredDir.entryList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs);     //(QDir::Filter::Files,QDir::SortFlag::NoSort)
        //QStringList allFiles = recoredDir.entryList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::Files);         //(QDir::Filter::Files,QDir::SortFlag::NoSort)



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
        stream << "<link rel='shortcut icon'' type='image/png'' href='https://raw.githubusercontent.com/yafp/dirgister/master/dirgister.ico'/>\n";
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

        // run the following part only on the sub-pages
        if(srcFolder != currentPath) // on all sub-pages -> add a backlink-navigation to upper dir
        {
            stream << "<h3><a href='../index.html'><i class='fa fa-arrow-circle-left'></i></a>&nbsp;"+currentPath+"</h3>";
        }
        else // main-page
        {
            stream << "<h3>"+currentPath+"</h3>";
        }
        stream << "</div>"; // close header div
        stream << "<div id='content'>";

        // Handle folders
        //
        if(allFolders.count() !=  0) // found folders
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

                // write log
                ui->textEdit->moveCursor (QTextCursor::End);
                newTimestampString = generateTimestampString();
                ui->textEdit->insertPlainText (newTimestampString+ "- Found folder: "+str+"\n");
            }
        }



        // new handling all files in the current dir
        QFileInfoList list = recoredDir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::Files);

        if(list.size() > 0)
        {
            stream << "<h2><i class='fa fa-file'></i>&nbsp; Files ("+QString::number(list.size())+")</h2>\n";

            for (int i = 0; i < list.size(); ++i)
            {
                QFileInfo fileInfo = list.at(i);

                QString icon = "";
                QString currentFile = fileInfo.fileName();

                // typ: audio
                if((currentFile.contains(".mp3")) | (currentFile.contains(".wav")) | (currentFile.contains(".flac")) | (currentFile.contains(".")))
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
                stream << fileInfo.fileName();
                stream << "<br>\n";
                stream << "<small>Size: ";
                stream << fileInfo.size()/1024/1024;
                stream << " MB</small>";
                stream << "<br><br>\n";


                // write log
                ui->textEdit->moveCursor (QTextCursor::End);
                newTimestampString = generateTimestampString();
                ui->textEdit->insertPlainText (newTimestampString+ "- Found file: "+currentFile+"\n");

                //qWarning() << fileInfo.absoluteFilePath();
                //qWarning() << fileInfo.fileName();
                //qWarning() << fileInfo.size();
                //qWarning() << "-------";
             }
        }

        stream << "</div>"; // close the content div
        stream << "</body>\n";
        stream << "</html>\n";

        file.close();

        newTimestampString = generateTimestampString();
        ui->textEdit->moveCursor (QTextCursor::End);
        ui->textEdit->insertPlainText (newTimestampString+ "- Created index: "+filename+"\n");
        ui->textEdit->repaint();

        checkSubDirs(currentPath, targetFolder);
    }
}



//  check subdirectories of current folder
void MainWindow::checkSubDirs(QString currentSubPath, QString currentTargetPath)
{
    newTimestampString = generateTimestampString();
    ui->textEdit->moveCursor (QTextCursor::End);
    ui->textEdit->insertPlainText (newTimestampString+ "- Checking subdirectories of "+currentSubPath+"\n");
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






// ########################################################################
// MENU, ACTIONS, TOOLBARS and STATUSBAR
// ########################################################################

// Creating Menu Actions
void MainWindow::createActions()
{
    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}


// Add Actions to Menus
void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(exitAct);

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutQtAct);
}





// ########################################################################
// READ and WRITE SETTINGS
// ########################################################################

// load the app-settings/values
void MainWindow::readSettings()
{
    QSettings settings("QtProject", "Dirgister");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    resize(size);
    move(pos);

    srcFolder = settings.value("srcFolder").toString();
    targetFolder = settings.value("targetFolder").toString();

    ui->le_sourceFolder->setText(srcFolder);
    ui->le_targetFolder->setText(targetFolder);

    updateStatusBar("Settings read");
}


// write/store the app-settings/values
void MainWindow::writeSettings()
{
    QSettings settings("QtProject", "Dirgister");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
    settings.setValue("srcFolder", srcFolder);
    settings.setValue("targetFolder", targetFolder);

    updateStatusBar("Settings written");
}



// ########################################################################
// HELPER
// ########################################################################
//
// generates and returns a timestamp (yyyyMMdd-hhmmss) as string
QString MainWindow::generateTimestampString()
{
    QDateTime dateTime = dateTime.currentDateTime();
    QString dateTimeString = dateTime.toString("yyyyMMdd-hhmmss");

    return dateTimeString;
}

// Writes a message to the app statusbar for x seconds
void MainWindow::updateStatusBar(QString statusMessage)
{
    statusBar()->showMessage(statusMessage,3000);
}
