/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/


#include <QtWidgets>
#include "mainwindow.h"
#include <QDebug>



MainWindow::MainWindow()
{
    textEdit = new QPlainTextEdit;
    textEdit->setReadOnly(true);

    setCentralWidget(textEdit);

    initValues();

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();

    readSettings();

    connect(textEdit->document(), SIGNAL(contentsChanged()), this, SLOT(documentWasModified()));

    //setCurrentFile("");
    setUnifiedTitleAndToolBarOnMac(true);
}



void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    event->accept();
}



void MainWindow::about()
{
   QMessageBox::about(this, tr("About Dirgister"),tr("<b>Dirgister</b> creates HTML based information pages based an user-defined folder and it's content."));
}



void MainWindow::resetLogUI()
{
   //textEdit->setPlainText("");
   textEdit->clear();
   readSettings();
}




void MainWindow::initValues()
{
   appVersion = "20150616.02";      // App Version String
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
        textEdit->moveCursor (QTextCursor::End);
        textEdit->insertPlainText ("Source folder set to: "+dir+"\n");

        srcFolder = dir;

        writeSettings();
        resetLogUI();
    }


}


// User defines a target folder - should be writable
void MainWindow::setTargetFolder()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select target for Index"),"/home",QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    textEdit->moveCursor (QTextCursor::End);
    textEdit->insertPlainText ("Target folder set to: "+dir+"\n");

    targetFolder = dir;

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
             //QDir().mkdir("/home/fidel/MuLiA/Output");
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
    qWarning() << "...The current directory: "+currentPath+" contains";

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

        qWarning() << allFolders;

        // Create file
        QTextStream stream( &file );

        // deleting the former content
        file.write("");
        file.resize(file.pos());


        // write the new content
        stream << "<html>\n";
        stream << "<head>\n";
        stream << "<title>DirGister</title>\n";
        stream << "<script src='//code.jquery.com/jquery-1.11.3.min.js'></script>\n";
        stream << "<link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.3.0/css/font-awesome.min.css'>\n";
        stream << "<style>";
        stream << "h3 { color: grey; font-size: 12;}";
        stream << "a:visited { color: black;}";
        stream << "</style>";


        stream << "</head>\n";

        stream << "<body>\n";
        stream << "<h1><i class='fa fa-list-alt'></i>&nbsp;DirGister</h1>\n";

        // nur auf den unterseiten
        if(srcFolder != currentPath) // all sub-pages
        {
            stream << "<h3><a href='../index.html'><i class='fa fa-arrow-circle-left'></i></a>&nbsp;"+currentPath+"</h3>";
        }
        else // main-page
        {
            stream << "<h3>"+currentPath+"</h3>";
            stream << "<p><i class='fa fa-question-circle'></i> This is a html-based Index for a folder and its content - generated by <b>DirGister</b> at <i class='fa fa-calendar'></i> "+dateTimeString+".</p>";
        }

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
                stream << "'>";
                stream << QString("%1").arg(str);
                stream << "</a><br>";

                newTarget = targetFolder+"/"+QString("%1").arg(str);
                qWarning() << "... Creating "+newTarget+" in "+targetFolder;

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
            stream << "<ul>\n";
            foreach (QString str, allFiles)
            {
                QString icon = "";


                QString currentFile = QString("%1").arg(str);

                // typ: audio
                if(currentFile.contains(".mp3"))
                {
                    icon = "<i class='fa fa-music'></i>";
                }

                // typ: image
                if(currentFile.contains(".jpg"))
                {
                    icon = "<i class='fa fa-image'></i>";
                }

                // add check for other filetypes


                // lets check if icon is undefined
                if(icon == "")
                {
                    icon =  "<i class='fa fa-file-o'></i>";
                }


                stream << "<li>";
                stream << icon+"&nbsp;";
                stream << QString("%1").arg(str);
                stream << "</li>";
            }
            stream << "</ul>\n";

        }

        stream << "</body>\n";

        file.close();

        textEdit->moveCursor (QTextCursor::End);
        textEdit->insertPlainText ("Creating: "+filename+"\n");
        textEdit->repaint();


        // Issue on Fedora with qDebug()
        // Fedora: /etc/xdg/QtProject/qtlogging.ini
        // https://forum.qt.io/topic/54820/qt-qdebug-not-working-with-qconsoleapplication-or-qapplication/5

        checkSubDirs(currentPath, targetFolder);
    }
}



//  check subdirectories of current folder
void MainWindow::checkSubDirs(QString currentSubPath, QString currentTargetPath)
{
    textEdit->moveCursor (QTextCursor::End);
    textEdit->insertPlainText ("Checking subdirectories of "+currentSubPath+"\n\n");
    textEdit->repaint();

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
        msgBox.setText("Dirgister is going to create a new Index for "+srcFolder+" in "+targetFolder+".");
        msgBox.setInformativeText("Do you want to proceed?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        int ret = msgBox.exec();

        // handle user-answer
        switch (ret)
        {
          case QMessageBox::Yes:
                textEdit->setPlainText("...started index generation\n\n");

                createSingleHTMLIndex(srcFolder,targetFolder);

                textEdit->moveCursor (QTextCursor::End);
                textEdit->insertPlainText ("\n\nIndex generation finished.\n");

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

    //cutAct->setEnabled(false);
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

    qWarning() << "loading settings";
    srcFolder = settings.value("srcFolder").toString();
    targetFolder = settings.value("targetFolder").toString();

    qWarning() << "- Reading Settings: SourceFolder: "+srcFolder+".";
    qWarning() << "- Reading Settings: TargetFolder: "+targetFolder+".";

    textEdit->setPlainText("Source folder: "+srcFolder+"\n");
    textEdit->moveCursor (QTextCursor::End);
    textEdit->insertPlainText ("Target folder: "+targetFolder+"\n");
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

void MainWindow::documentWasModified()
{
    //setWindowModified(textEdit->document()->isModified());
}


QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}
