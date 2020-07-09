#include "document.h"

#include <QApplication>
#include <QFile>
#include <QFileDialog>
#include <QProgressDialog>
#include <QSet>
#include <QTextDocument>
#include <QTextStream>

#include <QMessageBox>

QString Document::getTitle() const
{
    return gamebook.attribute(TITLE_ATTRIBUTE_NAME);
}

void Document::setTitle(const QString &value)
{
    gamebook.setAttribute(TITLE_ATTRIBUTE_NAME,value);
}

QString Document::getAuthor() const
{
    QString title = gamebook.attribute(AUTHOR_ATTRIBUTE_NAME);
    return title;
}

void Document::setAuthor(const QString &value)
{
    gamebook.setAttribute(AUTHOR_ATTRIBUTE_NAME,value);
}

void Document::maybeAppendToHistory(QString page)
{
    QStringList list = getHistory();
    if (!list.contains(page))
    {
        QDomElement history = gamebook.firstChildElement(HISTORY_ELEMENT_TAG);
        if (!history.isNull())
        {
            QDomElement id = domDocument->createElement(ID_ELEMENT_TAG);
            id.setAttribute(ID_ATTRIBUTE_NAME,page);
            history.appendChild(id);
        }
    }
}


QStringList Document::getHistory()
{
    QStringList stringList;

    QDomElement history = gamebook.firstChildElement(HISTORY_ELEMENT_TAG);
    if (!history.isNull())
    {
        QDomNodeList list = history.childNodes();
        for (int i = 0 ; i < list.count() ; ++i)
        {
            stringList << list.item(i).toElement().attribute(ID_ATTRIBUTE_NAME);
        }
    }

    return stringList;
}

QString Document::getAbstract()
{
    QString text;

    QDomElement abstract = gamebook.firstChildElement(ABSTRACT_ELEMENT_TAG);
    if (!abstract.isNull())
    {
        text = abstract.text();
    }

    return text;
}

void Document::setAbstract(QString value)
{
    QDomElement abstract = gamebook.firstChildElement(ABSTRACT_ELEMENT_TAG);
    if (!abstract.isNull())
    {
        abstract.firstChild().toText().setNodeValue(value);
    }
}

QString Document::getStartPage() const
{
    return gamebook.attribute(START_ATTRIBUTE_NAME);
}

void Document::setStartPage(QString &value)
{
    gamebook.setAttribute(START_ATTRIBUTE_NAME,value);
}

QDomElement Document::getPage(QString id)
{
    return idToPage.value(id);
}

QString Document::newPage()
{

    QDomNode chapter = gamebook.firstChildElement(CHAPTERS_ELEMENT_TAG).lastChildElement(CHAPTER_ELEMENT_TAG) ;
    QDomElement page = domDocument->createElement(PAGE_ELEMENT_TAG);
    page.setAttribute(ID_ATTRIBUTE_NAME,nextPageFreeId());
    page.setAttribute(TITLE_ATTRIBUTE_NAME,tr("Page %1").arg(page.attribute(ID_ATTRIBUTE_NAME)));
    QDomElement text = domDocument->createElement(TEXT_ELEMENT_TAG);
    page.appendChild(text);
    QDomText t2 = domDocument->createTextNode(tr("Pas de texte"));
    text.appendChild(t2);
    QDomElement sequels = domDocument->createElement(SEQUELS_ELEMENT_TAG);
    page.appendChild(sequels);
    chapter.appendChild(page);

    idToPage.insert(page.attribute(ID_ATTRIBUTE_NAME),page);

    return page.attribute(ID_ATTRIBUTE_NAME);

}

void Document::newChapter(QString title)
{
    QDomElement chapters = gamebook.firstChildElement(CHAPTERS_ELEMENT_TAG) ;
    QDomElement chapter = domDocument->createElement(CHAPTER_ELEMENT_TAG);
    chapter.setAttribute(TITLE_ATTRIBUTE_NAME,title);
    chapters.appendChild(chapter);
}

void Document::deleteCurrentPage()
{

    deleteCurrentPageFromAllSequels();
    deleteCurrentPageFromHistory();
    currentPage.parentNode().removeChild(currentPage);
    idToPage.remove(getCurrentPageId());
    setCurrentPageId("index");

}

void Document::deleteChapter(QString title)
{
    QDomNode oldChapter = getChapter(title) ;
    QDomNode chapters = oldChapter.parentNode() ;
    oldChapter = chapters.removeChild(oldChapter) ;
    QDomElement newChapter = chapters.lastChildElement(CHAPTER_ELEMENT_TAG) ;
    QDomNodeList list = oldChapter.childNodes();
    for (int i = 0 ; i < list.count() ; ++i)
        newChapter.appendChild(list.item(i)) ;
}

void Document::addChapterActions(QMenu *chapterMenu)
{
    QString chapter = chapterMenu->title();
    QDomNode chapterNode = getChapter(chapter);
    QDomNodeList pages = chapterNode.childNodes();
    for (int i = 0 ; i < pages.count() ; ++i)
    {
        QDomElement page = pages.item(i).toElement();
        QAction * action = chapterMenu->addAction(page.attribute(TITLE_ATTRIBUTE_NAME));
        action->setData(page.attribute(ID_ATTRIBUTE_NAME));
    }
}

void Document::addSequelsItems(QListWidget *sequelsWidget)
{
    QDomNodeList sequels = getCurrentPageSequels();
    for (int i = 0 ; i < sequels.count() ; ++i)
    {
        QDomElement sequel = sequels.item(i).toElement();
        QDomElement page = getPage(sequel.attribute(ID_ATTRIBUTE_NAME));
        QString text = sequel.attribute(TITLE_ATTRIBUTE_NAME);
        QString title = page.attribute(TITLE_ATTRIBUTE_NAME) ;
        QString id = page.attribute(ID_ATTRIBUTE_NAME) ;
        QString name = text+"=>"+title ;
        QListWidgetItem * item = new QListWidgetItem(sequelsWidget);
        item->setData(Qt::UserRole+1,text);
        item->setData(Qt::UserRole,id);
        item->setData(Qt::EditRole,name);
        sequelsWidget->addItem(item);
    }
}

void Document::addChaptersItems(QListWidget *chaptersWidget)
{
    QStringList chapters = getChapters();
    for (int i = 0 ; i < chapters.count() ; ++i)
    {
        QListWidgetItem * item = new QListWidgetItem(chaptersWidget);
        item->setData(Qt::EditRole,chapters.at(i));
        chaptersWidget->addItem(item);
    }
}

void Document::moveSequelDown(QString id)
{
    QDomElement sequels = currentPage.firstChildElement(SEQUELS_ELEMENT_TAG);
    QDomNodeList list = sequels.childNodes();
    for (int i=0 ; i < list.count() ; ++i)
    {
        if (list.item(i).toElement().attribute(ID_ATTRIBUTE_NAME)==id)
        {
            sequels.insertAfter(list.item(i),list.item(i+1));
            break;
        }
    }
}

void Document::moveSequelBottom(QString id)
{
    QDomElement sequels = currentPage.firstChildElement(SEQUELS_ELEMENT_TAG);
    QDomNodeList list = sequels.childNodes();
    for (int i=0 ; i < list.count() ; ++i)
    {
        if (list.item(i).toElement().attribute(ID_ATTRIBUTE_NAME)==id)
        {
            sequels.insertAfter(list.item(i),list.item(list.count()-1));
            break;
        }
    }
}

void Document::setCurrentPageSequelsFontFamily(QString font)
{
    QDomElement sequels = currentPage.firstChildElement(SEQUELS_ELEMENT_TAG);
    sequels.setAttribute(FONT_FAMILY_ATTRIBUTE_NAME,font);
}

void Document::setCurrentPageSequelsFontSize(QString size)
{
    QDomElement sequels = currentPage.firstChildElement(SEQUELS_ELEMENT_TAG);
    sequels.setAttribute(FONT_SIZE_ATTRIBUTE_NAME,size);
}

QFont Document::getCurrentPageSequelsFont()
{
    QDomElement sequels = currentPage.firstChildElement(SEQUELS_ELEMENT_TAG);
    QFont font;
    font.setFamily(sequels.attribute(FONT_FAMILY_ATTRIBUTE_NAME,"Helvetica"));
    font.setPointSize(sequels.attribute(FONT_SIZE_ATTRIBUTE_NAME,"11").toInt());
    return font;
}



void Document::moveSequelUp(QString id)
{
    QDomElement sequels = currentPage.firstChildElement(SEQUELS_ELEMENT_TAG);
    QDomNodeList list = sequels.childNodes();
    for (int i=0 ; i < list.count() ; ++i)
    {
        if (list.item(i).toElement().attribute(ID_ATTRIBUTE_NAME)==id)
        {
            sequels.insertBefore(list.item(i),list.item(i-1));
            break;
        }
    }
}

void Document::moveSequelTop(QString id)
{
    QDomElement sequels = currentPage.firstChildElement(SEQUELS_ELEMENT_TAG);
    QDomNodeList list = sequels.childNodes();
    for (int i=0 ; i < list.count() ; ++i)
    {
        if (list.item(i).toElement().attribute(ID_ATTRIBUTE_NAME)==id)
        {
            sequels.insertBefore(list.item(i),list.item(0));
            break;
        }
    }
}

void Document::deleteCurrentPageFromHistory()
{

    QDomElement history = gamebook.firstChildElement(HISTORY_ELEMENT_TAG) ;
    QDomNodeList list = history.childNodes();
    for (int i = 0 ; i < list.count() ; ++i)
    {
        QDomElement ID = list.item(i).toElement();
        if (ID.attribute(ID_ATTRIBUTE_NAME)==getCurrentPageId())
        {
            history.removeChild(ID);
            break;
        }
    }

    QStringList stringList = getHistory() ;
    setCurrentPageId("index");


}

void Document::removePageSequel(QDomElement page,QString sequel)
{
    QDomElement sequels = page.firstChildElement(SEQUELS_ELEMENT_TAG);
    QDomNodeList list = sequels.childNodes() ;
    for (int i = 0 ; i < list.count() ; ++i)
    {
        if (list.item(i).toElement().attribute(ID_ATTRIBUTE_NAME)==sequel)
        {
            sequels.removeChild(list.item(i));
            break;
        }
    }

}

QString Document::exportImages(QString html, QDir dir)
{
    QString result;
    QStringList stringList1 = html.split(QChar('<'),QString::SkipEmptyParts);
    for (int i = 0 ; i < stringList1.count(); ++i)
    {
        QStringList stringlist2 = stringList1.at(i).split(QChar('>'),QString::SkipEmptyParts);
        for (int j = 0 ; j < stringlist2.count() ; ++j)
        {
            if (stringlist2.at(j).startsWith("IMG",Qt::CaseInsensitive))
            {
                QStringList stringList3 = stringlist2.at(j).split(QChar('"'));
                QFile file(stringList3.at(1));
                QFileInfo fileInfo(file);
                QString fileName = fileInfo.fileName();
                file.copy(dir.absoluteFilePath(fileName));
                QString added = QString("<IMG src=\"%1\" />").arg(fileName) ;
                result.append(added);
            }
            else
            {
                if (j%2)
                    result.append(stringlist2.at(j));
                else
                    result.append(QString("<%1>").arg(stringlist2.at(j)));
            }
        }
    }
    return result;
}

void Document::stripHtml(QString &html)
{
    QRegExp doctypeRegExp("<!DOCTYPE.*>");
    doctypeRegExp.setMinimal(true);
    html = html.replace(doctypeRegExp,"");
    QRegExp htmlheadmetaRegExp("<html><head><meta.*/>");
    htmlheadmetaRegExp.setMinimal(true);
    html = html.replace(htmlheadmetaRegExp,"");
    QRegExp styleRegExp("<style.*</style>");
    html = html.replace(styleRegExp,"");
    QRegExp headbodyRegExp("</head><body.*>");
    headbodyRegExp.setMinimal(true);
    html = html.replace(headbodyRegExp,"");
    html = html.replace("</body></html>","");
}

void Document::rebuildPagesHash()
{
    idToPage.clear();
    QDomElement chapters = gamebook.firstChildElement(CHAPTERS_ELEMENT_TAG);
    QDomNodeList chapterList = chapters.childNodes() ;
    for (int i = 0 ; i < chapterList.count(); ++i)
    {
        QDomNodeList pages = chapterList.item(i).childNodes() ;
        for (int j = 0 ; j < pages.count() ; ++j)
        {
            QDomElement page = pages.item(j).toElement() ;
            idToPage.insert(page.attribute(ID_ATTRIBUTE_NAME),page);
        }
    }
}



void Document::deleteCurrentPageFromAllSequels()
{

    QList<QDomElement> pages = idToPage.values();
    QString id = getCurrentPageId() ;
    for (int i = 0 ; i < pages.count() ; ++i)
        removePageSequel(pages.at(i),id);
}

bool Document::isModified() const
{
    return modified;
}

void Document::setModified(bool value)
{
    modified = value;
    if (modified)
        emit contentsChanged();
}



QString Document::getCurrentPageId() const
{
    return gamebook.attribute(CURRENT_PAGE_ATTRIBUTE_NAME);
}

void Document::setCurrentPageId(const QString &value)
{
    gamebook.setAttribute(CURRENT_PAGE_ATTRIBUTE_NAME,value);
    maybeAppendToHistory(value);
    currentPage = getPage(getCurrentPageId());
}

QString Document::getCurrentPageTitle()
{
    return currentPage.attribute(TITLE_ATTRIBUTE_NAME);
}

void Document::setCurrentPageTitle(QString value)
{
    currentPage.setAttribute(TITLE_ATTRIBUTE_NAME,value);
}

QString Document::getCurrentPageChapter()
{
    return currentPage.parentNode().toElement().attribute(TITLE_ATTRIBUTE_NAME);
}

void Document::setCurrentPageChapter(QString value)
{
    getChapter(value).appendChild(currentPage.parentNode().removeChild(currentPage));
}

void Document::addCurrentPageSequel(QString id)
{
    QDomElement sequels = currentPage.firstChildElement(SEQUELS_ELEMENT_TAG);
    QDomElement sequel = domDocument->createElement(ID_ELEMENT_TAG);
    sequel.setAttribute(ID_ATTRIBUTE_NAME,id);
    sequel.setAttribute(TITLE_ATTRIBUTE_NAME,tr("Pas de texte"));
    sequels.appendChild(sequel);
}

void Document::addPageSequel(QString page,QString text,QString id)
{
    QDomElement sequels = idToPage.value(page).firstChildElement(SEQUELS_ELEMENT_TAG);
    QDomElement sequel = domDocument->createElement(ID_ELEMENT_TAG);
    sequel.setAttribute(ID_ATTRIBUTE_NAME,id);
    sequel.setAttribute(TITLE_ATTRIBUTE_NAME,text);
    sequels.appendChild(sequel);
}

void Document::removeCurrentPageSequel(QString id)
{
    QDomElement sequels = currentPage.firstChildElement(SEQUELS_ELEMENT_TAG);
    QDomNodeList list = sequels.childNodes() ;
    for (int i = 0 ; i < list.count() ; ++i)
    {
        if (list.item(i).toElement().attribute(ID_ATTRIBUTE_NAME)==id)
        {
            sequels.removeChild(list.item(i));
            break;
        }
    }

}

QDomNodeList Document::getCurrentPageSequels()
{
    QDomElement sequels = currentPage.firstChildElement(SEQUELS_ELEMENT_TAG);
    QDomNodeList list = sequels.childNodes() ;
    return list ;
}

void Document::setCurrentPageSequelText(int row, QString text)
{
    QDomNode sequel = getCurrentPageSequels().item(row);
    sequel.toElement().setAttribute(TITLE_ATTRIBUTE_NAME,text);
}

QString Document::getCurrentPageText()
{
    return currentPage.firstChildElement(TEXT_ELEMENT_TAG).text();
}

void Document::setCurrentPageText(QString html)
{
    stripHtml(html);
    currentPage.firstChildElement(TEXT_ELEMENT_TAG).firstChild().setNodeValue(html);
}

void Document::setPageText(QString page , QString text)
{
    idToPage.value(page).firstChildElement(TEXT_ELEMENT_TAG).firstChild().setNodeValue(text);
}

QString Document::getPageTitle(QString id)
{
    return getPage(id).attribute(TITLE_ATTRIBUTE_NAME);
}


void Document::load(QString path)
{
    QFile file(path);
    if (file.open(QIODevice::ReadOnly))
    {
        QString errMsg;
        int line,column;
        if (!domDocument->setContent(&file,&errMsg,&line,&column))
            QMessageBox::critical(NULL, tr("GameBookEditor"),tr("Erreur dans la lecture du fichier XML :\n%1\n(ligne %2, colonne %3)")
                                  .arg(errMsg).arg(line).arg(column));
        else
        {
            gamebook = domDocument->firstChildElement(GAMEBOOK_ELEMENT_TAG);
            if (gamebook.isNull())
            {
                QMessageBox::critical(NULL, tr("GameBookEditor"),tr("Ce n'est pas un fichier gamebook!"));
                delete domDocument;
                init();
            }
            else
            {
                rebuildPagesHash();
                setModified(false);
            }
        }

        file.close();
    }
    else
        QMessageBox::critical(NULL, tr("GameBookEditor"),
                              tr("Imossible d'ouvrir le fichier %1:\n%2.")
                              .arg(QDir::toNativeSeparators(path), file.errorString()));

}

void Document::save(QString path)
{
    QFile file(path);
    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream ts(&file);
        ts << domDocument->toString(4) ;
        file.close();
        setModified(false);
    }
    else
        QMessageBox::warning(NULL, tr("SDI"),
                             tr("Imossible d'écrire le fichier %1:\n%2.")
                             .arg(QDir::toNativeSeparators(path), file.errorString()));

}

QDomDocument *Document::getDomDocument() const
{
    return domDocument;
}

QDomElement Document::getCurrentPage() const
{
    return currentPage;
}

QString Document::exportPageToHtml(QDomElement page)
{
    QString html("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">");
    html.append("<HTML>\n\t<HEAD>\n\t\t<TITLE>");
    html.append(page.attribute(TITLE_ATTRIBUTE_NAME)) ;
    html.append("</TITLE>\n");
    html.append(("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />"));
    html.append("\t\t<style type=\"text/css\">\n");
    QDomElement sequels = page.firstChildElement(SEQUELS_ELEMENT_TAG);
    html.append(QString("\t\t\tli { font-family:'%1'; font-size:%2pt; }\n")
                .arg(sequels.attribute(FONT_FAMILY_ATTRIBUTE_NAME))
                .arg(sequels.attribute(FONT_SIZE_ATTRIBUTE_NAME)));
    html.append("\t\t</style>\n");
    html.append("\t</HEAD>\n\t<BODY>\n" );
    QString textHtml = page.firstChildElement(TEXT_ELEMENT_TAG).text();

    textHtml = textHtml.replace("<underline>","",Qt::CaseInsensitive);
    textHtml = textHtml.replace("</underline>","",Qt::CaseInsensitive);

    html.append(textHtml);
    html.append("\n\t\t<BR>\n");
    QDomNodeList sequelsList = sequels.childNodes();
    QString sequelsHtml("\t\t<UL>\n");
    for (int i = 0 ; i < sequelsList.count() ; ++i)
    {
        if (sequelsList.at(i).isElement())
        {
            QDomElement sequel = sequelsList.at(i).toElement() ;
            QString id = sequel.attribute(ID_ATTRIBUTE_NAME) ;
            QString text = sequel.attribute(TITLE_ATTRIBUTE_NAME);
            sequelsHtml.append("\t\t\t<LI><A HREF=\"" );
            sequelsHtml.append(id);
            sequelsHtml.append(".html\" >");
            sequelsHtml.append(text);
            sequelsHtml.append("</A></LI>\n");
        }
    }
    sequelsHtml.append("\t\t</UL>\n");
    html.append(sequelsHtml);
    html.append("\t</BODY>\n</HTML>");
    return html;
}

void Document::exportBookToHtml(QString path)
{
    QList<QDomElement> pages = idToPage.values() ;
    QProgressDialog progress(tr("Export HTML en cours..."),tr("Annuler"),0,pages.count(),NULL);
    progress.setModal(true);
    progress.setMinimumDuration(0);
    progress.setMaximum(pages.count());
    QDir dir(path);
    for (int i = 0 ; i < pages.count() ; ++i)
    {
        progress.setValue(i);
        QDomElement page = pages.at(i);
        QString fileName = QString("%1.html").arg(page.attribute(ID_ATTRIBUTE_NAME));
        QString filePath = dir.absoluteFilePath(fileName);
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly))
        {
            QTextStream ts(&file);
            QString html = exportPageToHtml(page);
            html = exportImages(html,dir);
            ts << html ;
            file.close();
        }
        else
        {
            QMessageBox::critical(NULL,tr("GameBookEditor"),tr("Impossible d'écrire le fichier %1 :\n%2.").arg(filePath).arg(file.errorString()));
            break;
        }
        if (progress.wasCanceled())
            break;
        qApp->processEvents();
    }
    progress.setValue(pages.count());
}

void Document::addHistoryActions(QMenu *historyMenu)
{
    QDomElement gamebook = domDocument->firstChildElement(GAMEBOOK_ELEMENT_TAG);
    if (!gamebook.isNull())
    {
        QDomElement history = gamebook.firstChildElement(HISTORY_ELEMENT_TAG);
        QDomNodeList historyList = history.childNodes();
        for (int i = 0 ; i < historyList.count() ; ++i)
        {
            QDomElement ID = historyList.item(i).toElement();
            QString id = ID.attribute(ID_ATTRIBUTE_NAME) ;
            QString title = getPage(id).attribute(TITLE_ATTRIBUTE_NAME);
            QAction * action = historyMenu->addAction(title);
            action->setData(id);
        }
    }

}

void Document::clearHistoryList()
{
    QDomElement gamebook = domDocument->firstChildElement(GAMEBOOK_ELEMENT_TAG);
    if (!gamebook.isNull())
    {
        QDomElement history = gamebook.firstChildElement(HISTORY_ELEMENT_TAG);
        QDomNodeList list = history.childNodes();
        for (int i = list.count()-1 ; i > 0 ; --i)
            history.removeChild(list.item(i));

        setModified(true);
    }
}

QString Document::nextPageFreeId()
{
    QString id_str = gamebook.attribute(FREE_PAGE_ATTRIBUTE_NAME);
    int id = id_str.toInt();
    ++id;
    gamebook.setAttribute(FREE_PAGE_ATTRIBUTE_NAME,id);

    return id_str;
}

Document::Document()
{
    init();
}

void
Document::init()
{
    domDocument = new QDomDocument(GAMEBOOK_DOCUMENT_TYPE);
    gamebook = domDocument->createElement(GAMEBOOK_ELEMENT_TAG);
    domDocument->appendChild(gamebook);

    gamebook.setAttribute(TITLE_ATTRIBUTE_NAME,tr("Sans titre"));
    gamebook.setAttribute(AUTHOR_ATTRIBUTE_NAME,QDir::home().dirName());
    gamebook.setAttribute(START_ATTRIBUTE_NAME,"index");
    gamebook.setAttribute(FREE_PAGE_ATTRIBUTE_NAME,"1");
    gamebook.setAttribute(CURRENT_PAGE_ATTRIBUTE_NAME,"index");

    QDomElement history = domDocument->createElement(HISTORY_ELEMENT_TAG);
    gamebook.appendChild(history);
    QDomElement first = domDocument->createElement(ID_ELEMENT_TAG);
    history.appendChild(first);
    first.setAttribute(ID_ATTRIBUTE_NAME,"index");

    QDomElement abstract = domDocument->createElement(ABSTRACT_ELEMENT_TAG);
    gamebook.appendChild(abstract);
    QDomText t = domDocument->createTextNode(tr("4ème de couverture"));
    abstract.appendChild(t);

    QDomElement chapters = domDocument->createElement(CHAPTERS_ELEMENT_TAG);
    gamebook.appendChild(chapters);

    QDomElement chapter = domDocument->createElement(CHAPTER_ELEMENT_TAG);
    chapter.setAttribute(TITLE_ATTRIBUTE_NAME,tr("Chapitre 1"));
    chapters.appendChild(chapter);

    QDomElement page = domDocument->createElement(PAGE_ELEMENT_TAG);
    chapter.appendChild(page);
    page.setAttribute(ID_ATTRIBUTE_NAME,"index");
    page.setAttribute(TITLE_ATTRIBUTE_NAME,tr("Sans titre"));

    QDomElement text = domDocument->createElement(TEXT_ELEMENT_TAG);
    page.appendChild(text);
    QDomText t2 = domDocument->createTextNode(tr("Pas de texte"));
    text.appendChild(t2);

    QDomElement sequels = domDocument->createElement(SEQUELS_ELEMENT_TAG);
    page.appendChild(sequels);

    rebuildPagesHash();
    setModified(false);
    setCurrentPageId(getStartPage());
}

Document::~Document()
{
    delete domDocument;

}

QDomElement Document::getChapter(QString title)
{
    QDomNode chapters = gamebook.firstChildElement(CHAPTERS_ELEMENT_TAG);
    QDomNodeList list = chapters.childNodes();
    QDomElement result;
    for (int i = 0 ; i < list.count() ; ++i)
    {
        QDomElement chapter = list.item(i).toElement();
        if (chapter.attribute(TITLE_ATTRIBUTE_NAME)==title)
        {
            result = chapter;
            break ;
        }
    }
   return result;
}

QStringList Document::getChapters()
{
    QStringList chapters;

    QDomNodeList list = gamebook.firstChildElement(CHAPTERS_ELEMENT_TAG).childNodes();
    for (int i = 0 ; i < list.count();++i)
        chapters << list.item(i).toElement().attribute(TITLE_ATTRIBUTE_NAME);

    return chapters;
}

void Document::changeChapterName(int row,QString newName)
{
    QDomNodeList chapters = gamebook.firstChildElement(CHAPTERS_ELEMENT_TAG).childNodes() ;
    QDomElement chapter = chapters.item(row).toElement() ;
    chapter.setAttribute(TITLE_ATTRIBUTE_NAME,newName);
}

void Document::moveChapterTop(int row)
{
    QDomElement chapters = gamebook.firstChildElement(CHAPTERS_ELEMENT_TAG) ;
    QDomNodeList list = chapters.childNodes();
    chapters.insertBefore(list.item(row),list.item(0));
}

void Document::moveChapterUp(int row)
{
    QDomElement chapters = gamebook.firstChildElement(CHAPTERS_ELEMENT_TAG) ;
    QDomNodeList list = chapters.childNodes();
    chapters.insertBefore(list.item(row),list.item(row-1));
}

void Document::moveChapterBottom(int row)
{
    QDomElement chapters = gamebook.firstChildElement(CHAPTERS_ELEMENT_TAG) ;
    QDomNodeList list = chapters.childNodes();
    chapters.insertAfter(list.item(row),list.item(list.count()-1));
}

void Document::moveChapterDown(int row)
{
    QDomElement chapters = gamebook.firstChildElement(CHAPTERS_ELEMENT_TAG) ;
    QDomNodeList list = chapters.childNodes();
    chapters.insertAfter(list.item(row),list.item(row+1));
}
