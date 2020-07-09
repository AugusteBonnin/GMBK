#include "binarytreedialog.h"
#include "booleanalgebradialog.h"
#include "form.h"
#include "mainwindow.h"
#include "narytreedialog.h"
#include "pseudorandomdialog.h"
#include "ui_mainwindow.h"

#include <QtWidgets>
#include <QFileDialog>
#include <QMessageBox>
#include <QMouseEvent>


MainWindow::MainWindow() :
    QMainWindow()
{
    //ui->setupUi(this);
    init();
    setCurrentFile(QString());
}


MainWindow::MainWindow(const QString &fileName)
{
    init();
    loadFile(fileName);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::newFile()
{
    MainWindow *other = new MainWindow;
    other->tile(this);
    other->show();
}

void MainWindow::newPage()
{
    QString id = document->newPage();
    document->setCurrentPageId(id);
    document->maybeAppendToHistory(id);
    form->updatePage();
}

void MainWindow::deletePage()
{
    if (document->getCurrentPageId()=="index")
        QMessageBox::warning(this,"GameBookEditor",tr("La première page ne peut pas être supprimée."));
    else
    {
        const QMessageBox::StandardButton ret
                = QMessageBox::warning(this, tr("GameBookEditor"),
                                       tr("Désirez vous vraiment supprimer la page en cours d'édition?\n"
                                          "Cette action ne pourra pas être annulée."),
                                       QMessageBox::Ok | QMessageBox::Cancel);
        switch (ret) {
        case QMessageBox::Ok:
            document->deleteCurrentPage();
            form->updatePage();
            break;
        case QMessageBox::Cancel:
        default:
            break;
        }


    }

}

void MainWindow::gotoPage()
{
    QAction * action = (QAction*)sender();
    QString id = action->data().toString();

    document->setCurrentPageId(id);
    document->maybeAppendToHistory(id);
    document->setModified(true);

    form->updatePage();
    rebuidHistoryMenu();
}

void MainWindow::open()
{
    const QString fileName = QFileDialog::getOpenFileName(this,tr("Sélectionnez l'index.xml du livre que vous désirez ouvrir"),QDir::homePath(),"*.xml");

    if (!fileName.isEmpty())
        openFile(fileName);
}

void MainWindow::openFile(const QString &fileName)
{
    MainWindow *existing = findMainWindow(fileName);
    if (existing) {
        existing->show();
        existing->raise();
        existing->activateWindow();
        return;
    }

    if (isUntitled && !document->isModified() && !isWindowModified()) {
        loadFile(fileName);
        return;
    }

    MainWindow *other = new MainWindow(fileName);
    if (other->isUntitled) {
        delete other;
        return;
    }
    other->tile(this);
    other->show();
}

bool MainWindow::save()
{
    return isUntitled ? saveAs() : saveFile(curFile);
    return true;
}

bool MainWindow::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Enregister sous..."),
                                                    curFile);
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About SDI"),
                       tr("The <b>SDI</b> example demonstrates how to write single "
                          "document interface applications using Qt."));
}

void MainWindow::documentWasModified()
{
    setWindowModified(true);
}

void MainWindow::init()
{
    try {
#ifdef Q_OS_MACOS
        m_Hunspell = new Hunspell("fr-classique.aff","fr-classique.dic");
        qDebug() << "Hunspell with AFF" << "fr-classique.aff" << "and DIC" << "fr-classique.dic";
#else
                m_Hunspell = new Hunspell("fr.aff","fr.dic");
                qDebug() << "Hunspell with AFF" << "fr.aff" << "and DIC" << "fr.dic";
#endif
        m_Encoding = QString::fromLatin1(m_Hunspell->get_dic_encoding());
        m_Codec = QTextCodec::codecForName(m_Encoding.toLatin1().constData());
    }
    catch(...) {
        qDebug() << "Error loading Hunspell" ;
        m_Hunspell = NULL;
    }
    //setAttribute(Qt::WA_DeleteOnClose);
    setIconSize(QSize(64,64));

    gotoMenu = new QMenu(this);
    historyMenu = new QMenu(this);
    isUntitled = true;
    document=new Document();
    form = new Form(this,document) ;
    setCentralWidget(form);

    createActions();
    createStatusBar();

    readSettings();

    connect(document, SIGNAL(contentsChanged()),
            this, SLOT(documentWasModified()));
    connect(document, SIGNAL(contentsChanged()),
            this, SLOT(rebuidGotoMenu()));
    connect(document, SIGNAL(contentsChanged()),
            this, SLOT(rebuidHistoryMenu()));

    setUnifiedTitleAndToolBarOnMac(true);


}

void MainWindow::tile(const QMainWindow *previous)
{
    if (!previous)
        return;
    int topFrameWidth = previous->geometry().top() - previous->pos().y();
    if (!topFrameWidth)
        topFrameWidth = 40;
    const QPoint pos = previous->pos() + 2 * QPoint(topFrameWidth, topFrameWidth);
    if (QApplication::desktop()->availableGeometry(this).contains(rect().bottomRight() + pos))
        move(pos);
}

//! [implicit tr context]
void MainWindow::createActions()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&Fichier"));
    //! [implicit tr context]
    QToolBar *fileToolBar = addToolBar(tr("Fichier"));

    const QIcon newIcon(":/images/newbook.png");
    QAction *newAct = new QAction(newIcon, tr("&Nouveau livre..."), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Créer un nouveau livre"));
    connect(newAct, SIGNAL(triggered(bool)), this, SLOT(newFile()));
    fileMenu->addAction(newAct);
    fileToolBar->addAction(newAct);

    const QIcon openIcon(":/images/openbook.png");
    QAction *openAct = new QAction(openIcon, tr("&Ouvrir un livre..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Ouvre un livre existant"));
    connect(openAct, SIGNAL(triggered(bool)), this, SLOT(open()));
    fileMenu->addAction(openAct);
    fileToolBar->addAction(openAct);

    const QIcon saveIcon(":/images/savebook.png");
    QAction *saveAct = new QAction(saveIcon, tr("Enregistrer le livre"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Enregistrer le livre"));
    connect(saveAct, SIGNAL(triggered(bool)), this, SLOT(save()));
    fileMenu->addAction(saveAct);
    fileToolBar->addAction(saveAct);

    const QIcon saveAsIcon = QIcon::fromTheme("document-save-as");
    QAction *saveAsAct = fileMenu->addAction(saveAsIcon, tr("Enregistrer sous..."), this, SLOT(saveAs()));
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Enregistre le livre sous un nouveau nom"));

    fileMenu->addSeparator();

    QMenu *recentMenu = fileMenu->addMenu(tr("Recent..."));
    connect(recentMenu, &QMenu::aboutToShow, this, &MainWindow::updateRecentFileActions);
    recentFileSubMenuAct = recentMenu->menuAction();

    for (int i = 0; i < MaxRecentFiles; ++i) {
        recentFileActs[i] = recentMenu->addAction(QString(), this, &MainWindow::openRecentFile);
        recentFileActs[i]->setVisible(false);
    }

    recentFileSeparator = fileMenu->addSeparator();

    setRecentFilesVisible(MainWindow::hasRecentFiles());

    const QIcon newPageIcon(":/images/newpage.png");
    QAction *newPageAct = new QAction(newPageIcon, tr("Nouvelle page"), this);
    newPageAct->setStatusTip(tr("Créer un nouveau livre"));
    connect(newPageAct, SIGNAL(triggered(bool)), this, SLOT(newPage()));
    fileMenu->addAction(newPageAct);
    fileToolBar->addAction(newPageAct);

    const QIcon delPageIcon(":/images/deletepage.png");
    QAction *delPageAct = new QAction(delPageIcon, tr("Supprimer la page"), this);
    delPageAct->setStatusTip(tr("Supprimer la page"));
    connect(delPageAct, SIGNAL(triggered(bool)), this, SLOT(deletePage()));
    fileMenu->addAction(delPageAct);
    fileToolBar->addAction(delPageAct);

    const QIcon htmlIcon(":/images/exporthtml.png");
    QAction *htmlAct = new QAction(htmlIcon, tr("Exporter en HTML..."), this);
    htmlAct->setStatusTip(tr("Exporter en HTML"));
    connect(htmlAct, SIGNAL(triggered(bool)), this, SLOT(exportHtml()));
    fileMenu->addAction(htmlAct);
    fileToolBar->addAction(htmlAct);


    QAction *closeAct = fileMenu->addAction(tr("&Close"), this, &QWidget::close);
    closeAct->setShortcut(tr("Ctrl+W"));
    closeAct->setStatusTip(tr("Close this window"));

    const QIcon exitIcon = QIcon::fromTheme("application-exit");
    QAction *exitAct = fileMenu->addAction(exitIcon, tr("E&xit"), qApp, &QApplication::closeAllWindows);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));

    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    QToolBar *editToolBar = addToolBar(tr("Edit"));

#ifndef QT_NO_CLIPBOARD
    const QIcon cutIcon = QIcon::fromTheme("edit-cut", QIcon(":/images/cut.png"));
    QAction *cutAct = new QAction(cutIcon, tr("Cu&t"), this);
    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                            "clipboard"));
    //connect(cutAct, &QAction::triggered, textEdit, &QTextEdit::cut);
    editMenu->addAction(cutAct);
    editToolBar->addAction(cutAct);

    const QIcon copyIcon = QIcon::fromTheme("edit-copy", QIcon(":/images/copy.png"));
    QAction *copyAct = new QAction(copyIcon, tr("&Copy"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    //connect(copyAct, &QAction::triggered, textEdit, &QTextEdit::copy);
    editMenu->addAction(copyAct);
    editToolBar->addAction(copyAct);

    const QIcon pasteIcon = QIcon::fromTheme("edit-paste", QIcon(":/images/paste.png"));
    QAction *pasteAct = new QAction(pasteIcon, tr("&Paste"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    //connect(pasteAct, &QAction::triggered, textEdit, &QTextEdit::paste);
    editMenu->addAction(pasteAct);
    editToolBar->addAction(pasteAct);

    menuBar()->addSeparator();
#endif // !QT_NO_CLIPBOARD

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    QAction *aboutAct = helpMenu->addAction(tr("&About"), this, &MainWindow::about);
    aboutAct->setStatusTip(tr("Show the application's About box"));

    QAction *aboutQtAct = helpMenu->addAction(tr("About &Qt"), qApp, &QApplication::aboutQt);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));

#ifndef QT_NO_CLIPBOARD
    cutAct->setEnabled(false);
    copyAct->setEnabled(false);
    //connect(textEdit, &QTextEdit::copyAvailable, cutAct, &QAction::setEnabled);
    //connect(textEdit, &QTextEdit::copyAvailable, copyAct, &QAction::setEnabled);
#endif // !QT_NO_CLIPBOARD

    const QIcon homeIcon(":/images/home.png");
    QAction *homeAct = new QAction(homeIcon, tr("Aller à la premère page"), this);
    homeAct->setStatusTip(tr("Aller à la premère page"));
    connect(homeAct, SIGNAL(triggered(bool)), this, SLOT(home()));
    fileToolBar->addAction(homeAct);

    gotoButton = new QToolButton(this);
    gotoButton->setIcon(QIcon(":/images/gotopage.png"));
    gotoButton->setToolTip(tr("Aller à la page"));
    rebuidGotoMenu();
    gotoButton->setMenu(gotoMenu);
    gotoButton->setPopupMode(QToolButton::InstantPopup);
    fileToolBar->addWidget(gotoButton);

    QToolButton* historyButton = new QToolButton(this);
    historyButton->setIcon(QIcon(":/images/history.png"));
    historyButton->setToolTip(tr("Historique"));
    rebuidHistoryMenu();
    historyButton->setMenu(historyMenu);
    historyButton->setPopupMode(QToolButton::InstantPopup);
    fileToolBar->addWidget(historyButton);

    const QIcon clearHistoryIcon(":/images/clearhistory.png");
    QAction *clearHistoryAct = new QAction(clearHistoryIcon, tr("Effacer l'historique"), this);
    clearHistoryAct->setStatusTip(tr("Effacer l'historique"));
    connect(clearHistoryAct, SIGNAL(triggered(bool)), this, SLOT(clearHistory()));
    fileToolBar->addAction(clearHistoryAct);

    const QIcon algebraIcon(":/images/algebra.png");
    QAction *algebraAct = new QAction(algebraIcon, tr("Créer une algèbre de Boole"), this);
    algebraAct->setStatusTip(tr("Créer une algèbre de Boole"));
    connect(algebraAct, SIGNAL(triggered(bool)), this, SLOT(createAlgebra()));
    fileToolBar->addAction(algebraAct);

    const QIcon treeIcon(":/images/tree.png");
    QAction *treeAct = new QAction(treeIcon, tr("Créer un arbre binaire"), this);
    treeAct->setStatusTip(tr("Créer un arbre binaire"));
    connect(treeAct, SIGNAL(triggered(bool)), this, SLOT(createBinaryTree()));
    fileToolBar->addAction(treeAct);

    const QIcon aleaIcon(":/images/alea.png");
    QAction *aleaAct = new QAction(aleaIcon, tr("Créer des suites pseudo-aléatoires"), this);
    aleaAct->setStatusTip(tr("Créer des suites pseudo-aléatoires"));
    connect(aleaAct, SIGNAL(triggered(bool)), this, SLOT(createRandom()));
    fileToolBar->addAction(aleaAct);

    const QIcon naryTreeIcon(":/images/narytree.png");
    QAction *naryAct = new QAction(naryTreeIcon, tr("Créer un arbre n-aire"), this);
    naryAct->setStatusTip(tr("Créer un arbre n-aire"));
    connect(naryAct, SIGNAL(triggered(bool)), this, SLOT(createNaryTree()));
    fileToolBar->addAction(naryAct);



}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::readSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
    if (geometry.isEmpty()) {
        const QRect availableGeometry = QApplication::desktop()->availableGeometry(this);
        resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
        move((availableGeometry.width() - width()) / 2,
             (availableGeometry.height() - height()) / 2);
    } else {
        restoreGeometry(geometry);
    }
}

void MainWindow::writeSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    settings.setValue("geometry", saveGeometry());
}

bool MainWindow::maybeSave()
{
    if (!document->isModified())
        return true;
    const QMessageBox::StandardButton ret
            = QMessageBox::warning(this, tr("GameBookEditor"),
                                   tr("The document has been modified.\n"
                                      "Do you want to save your changes?"),
                                   QMessageBox::Save | QMessageBox::Discard
                                   | QMessageBox::Cancel);
    switch (ret) {
    case QMessageBox::Save:
        return save();
    case QMessageBox::Cancel:
        return false;
    default:
        break;
    }
    return true;
}

void MainWindow::loadFile(const QString &fileName)
{

    document->load(fileName);
    form->updateBook();
    form->updatePage();
    rebuidGotoMenu();
    setCurrentFile(fileName);
    statusBar()->showMessage(tr("Fichier chargé"), 2000);
}

void MainWindow::setRecentFilesVisible(bool visible)
{
    recentFileSubMenuAct->setVisible(visible);
    recentFileSeparator->setVisible(visible);
}

static inline QString recentFilesKey() { return QStringLiteral("recentFileList"); }
static inline QString fileKey() { return QStringLiteral("file"); }

static QStringList readRecentFiles(QSettings &settings)
{
    QStringList result;
    const int count = settings.beginReadArray(recentFilesKey());
    for (int i = 0; i < count; ++i) {
        settings.setArrayIndex(i);
        result.append(settings.value(fileKey()).toString());
    }
    settings.endArray();
    return result;
}

static void writeRecentFiles(const QStringList &files, QSettings &settings)
{
    const int count = files.size();
    settings.beginWriteArray(recentFilesKey());
    for (int i = 0; i < count; ++i) {
        settings.setArrayIndex(i);
        settings.setValue(fileKey(), files.at(i));
    }
    settings.endArray();
}

bool MainWindow::hasRecentFiles()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    const int count = settings.beginReadArray(recentFilesKey());
    settings.endArray();
    return count > 0;
}

void MainWindow::prependToRecentFiles(const QString &fileName)
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());

    const QStringList oldRecentFiles = readRecentFiles(settings);
    QStringList recentFiles = oldRecentFiles;
    recentFiles.removeAll(fileName);
    recentFiles.prepend(fileName);
    if (oldRecentFiles != recentFiles)
        writeRecentFiles(recentFiles, settings);

    setRecentFilesVisible(!recentFiles.isEmpty());
}

void MainWindow::updateRecentFileActions()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());

    const QStringList recentFiles = readRecentFiles(settings);
    const int count = qMin(int(MaxRecentFiles), recentFiles.size());
    int i = 0;
    for ( ; i < count; ++i) {
        const QString fileName = MainWindow::strippedName(recentFiles.at(i));
        recentFileActs[i]->setText(tr("&%1 %2").arg(i + 1).arg(fileName));
        recentFileActs[i]->setData(recentFiles.at(i));
        recentFileActs[i]->setVisible(true);
    }
    for ( ; i < MaxRecentFiles; ++i)
        recentFileActs[i]->setVisible(false);
}

void MainWindow::openRecentFile()
{
    if (const QAction *action = qobject_cast<const QAction *>(sender()))
        openFile(action->data().toString());
}

bool MainWindow::saveFile(const QString &fileName)
{

    QApplication::setOverrideCursor(Qt::WaitCursor);
    document->save(fileName);
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("Fichier enregistré"), 2000);
    return true;
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    static int sequenceNumber = 1;

    isUntitled = fileName.isEmpty();
    if (isUntitled) {
        curFile = tr("Sans-titre%1.xml").arg(sequenceNumber++);
    } else {
        curFile = QFileInfo(fileName).canonicalFilePath();
    }

    document->setModified(false);
    setWindowModified(false);

    if (!isUntitled && windowFilePath() != curFile)
        MainWindow::prependToRecentFiles(curFile);

    setWindowFilePath(curFile);
}

QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

MainWindow *MainWindow::findMainWindow(const QString &fileName) const
{
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

    foreach (QWidget *widget, QApplication::topLevelWidgets()) {
        MainWindow *mainWin = qobject_cast<MainWindow *>(widget);
        if (mainWin && mainWin->curFile == canonicalFilePath)
            return mainWin;
    }

    return 0;
}

void MainWindow::rebuidGotoMenu()
{
    gotoMenu->clear();
    QStringList chapters = document->getChapters();
    for (int i = 0 ; i < chapters.count() ;++i)
    {
        QMenu * chapter = new QMenu(chapters.at(i),gotoMenu);
        gotoMenu->addMenu(chapter);
        document->addChapterActions(chapter);

        QList<QAction *> actions = chapter->actions();
        for (int j = 0 ; j < actions.count() ; ++j)
            connect(actions.at(j),SIGNAL(triggered(bool)),this,SLOT(gotoPage()));
    }

}

void MainWindow::rebuidHistoryMenu()
{
    historyMenu->clear();
    document->addHistoryActions(historyMenu);
    QList<QAction *> actions = historyMenu->actions();
    for (int j = 0 ; j < actions.count() ; ++j)
        connect(actions.at(j),SIGNAL(triggered(bool)),this,SLOT(gotoPage()));

}

void MainWindow::clearHistory()
{
    document->clearHistoryList();
    rebuidHistoryMenu();
}

void MainWindow::exportHtml()
{
    QSettings settings;
    QString path = QFileDialog::getExistingDirectory(this,tr("Sélectionnez le dossier dans lequel vous voulez réaliser l'export HTML."),
                                                     settings.value("LastHtmlExportDirPath",QDir::homePath()).toString());
    if (path!=QString())
    {
        settings.setValue("LastHtmlExportDirPath",path);
        document->exportBookToHtml(path);
    }
}

void MainWindow::home()
{
    document->setCurrentPageId("index");
    form->updatePage();
}

void MainWindow::createAlgebra()
{
    BooleanAlgebraDialog bad(this);
    int ret = bad.exec();
    switch(ret)
    {
    case QDialog::Accepted :
    {
        QStringList events = bad.getEvents();
        document->newChapter(tr("Algèbre de Boole"));
        int count = 1 << events.count() ;
        QStringList pages;
        for (int i = 0 ; i < count ; ++i)
        {
            QString page = document->newPage();
            QString text;
            for (int j = 0 ; j < events.count() ; ++j)
            {
                if ((i & (1<<j))==0)
                    text.append("NON ");
                text.append(events.at(j)+"<BR>");
            }
            document->setPageText(page,text);
            pages << page ;
        }

        for (int i = 0 ; i < count ; ++i)
        {
            for (int j = 0 ; j < events.count() ; ++j)
                if ((i&(1<<j))==0)
                    document->addPageSequel(pages.at(i),events.at(j),pages.at(i+(1<<j)));
        }

        document->setCurrentPageId(pages.at(0));
        document->setModified(true);
        form->updatePage();
    }
    default:
        break;

    }
}

void MainWindow::createBinaryTree()
{
    BinaryTreeDialog btd(this);
    int ret = btd.exec();
    switch(ret)
    {
    case QDialog::Accepted :
    {
        QStringList events = btd.getEvents();
        document->newChapter(tr("Arbre binaire"));
        int count = (2 << events.count())-1 ;
        QStringList pages;
        for (int i = 0 ; i < count ; ++i)
        {
            QString page = document->newPage();
            QString text;
            int max = (int)log2(i+1) ;
            for (int j = 0 ; j < max ; ++j)
            {
                if ((i+1)&((1<<(max-j-1))))
                    text.append("NON ");
                text.append(events.at(j)+"<BR>");
            }
            document->setPageText(page,text);
            pages << page ;
        }

        for (int i = 0 ; i < (2<<(events.count()-1))-1 ; ++i)
        {
            int j = (int)log2(i+1);
            document->addPageSequel(pages.at(i),events.at(j),pages.at((i<<1)+1));
            document->addPageSequel(pages.at(i),tr("NON ")+events.at(j),pages.at((i<<1)+2));
        }

        document->setCurrentPageId(pages.at(0));
        document->setModified(true);
        form->updatePage();
    }
    default:
        break;

    }
}

void MainWindow::createRandom()
{
    PseudoRandomDialog prd(this);
    int ret = prd.exec();
    switch(ret)
    {
    case QDialog::Accepted :
    {
        QStringList events = prd.getEvents();
        QVector<QVector<int> > series = prd.getPrimitiveElementsSeries();
        document->newChapter(tr("Suites pseudo-aléatoires"));
        QStringList pages;
        QString page = document->newPage();
        pages << page ;
        document->setPageText(page,events.at(0));
        for (int i = 0 ; i < series.count() ; ++i)
        {
            QString text;
            for (int j = 1 ; j < series[i].count(); ++j)
                text.append(events.at(series[i][j]-1)+"<BR>");
            page = document->newPage();
            pages << page;
            document->setPageText(page,text);
        }

        for (int i = 0 ; i < series.count() ; ++i)
        {
            document->addPageSequel(pages.at(0),events.at(series[i].at(1)-1),pages[i+1]);
        }

        document->setCurrentPageId(pages.at(0));
        document->setModified(true);
        form->updatePage();
    }
    default:
        break;

    }
}

void MainWindow::createNaryTree()
{
    NaryTreeDialog ntd(this);
    int ret = ntd.exec();
    switch(ret)
    {
    case QDialog::Accepted :
    {
        QStringList events = ntd.getEvents();
        document->newChapter(tr("Arbre n-aire"));
        QStringList pages;
        QString page = document->newPage();
        pages << page ;
        document->setPageText(page,"Introduction");
        createNaryNode(page,events,QStringList());

        document->setCurrentPageId(page);
        document->setModified(true);
        form->updatePage();
    }
    default:
        break;

    }
}

void MainWindow::createNaryNode(QString parent,const QStringList & events,QStringList used)
{
    for (int i = 0 ; i < events.count() ; ++i)
        if (!used.contains(events[i]))
        {
            QString new_page = document->newPage();

            document->addPageSequel(parent,events[i],new_page);
            QStringList new_used = used ;
            new_used << events[i] ;
            QString text = new_used.join("<BR>");
            document->setPageText(new_page,text);
            createNaryNode(new_page,events,new_used );
        }
}
