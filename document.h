#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QObject>
#include <QDomDocument>
#include <QHash>
#include <QString>
#include <QMenu>
#include <QListWidget>
#include <QDir>

#define GAMEBOOK_DOCUMENT_TYPE "GMBK"
#define GAMEBOOK_ELEMENT_TAG "GameBook"
#define TITLE_ATTRIBUTE_NAME "title"
#define START_ATTRIBUTE_NAME "start"
#define CURRENT_PAGE_ATTRIBUTE_NAME "current"
#define FREE_PAGE_ATTRIBUTE_NAME "free"
#define AUTHOR_ATTRIBUTE_NAME "author"
#define HISTORY_ELEMENT_TAG "History"
#define ID_ELEMENT_TAG "ID"
#define ID_ATTRIBUTE_NAME "id"
#define ABSTRACT_ELEMENT_TAG "Abstract"
#define CHAPTERS_ELEMENT_TAG "Chapters"
#define CHAPTER_ELEMENT_TAG "Chapter"
#define PAGE_ELEMENT_TAG "Page"
#define CHAPTER_ATTRIBUTE_NAME "chapter"
#define SEQUELS_ELEMENT_TAG "Sequels"
#define FONT_FAMILY_ATTRIBUTE_NAME "font-family"
#define FONT_SIZE_ATTRIBUTE_NAME "font-size"
#define TEXT_ELEMENT_TAG "Text"

class Document : public QObject
{
    Q_OBJECT
    QDomDocument *domDocument;
    QDomElement gamebook;
    bool modified;
    QDomElement currentPage;
    QHash<QString,QDomElement> idToPage;
    QString nextPageFreeId();
    void deleteCurrentPageFromHistory();
    void deleteCurrentPageFromAllSequels();
    void removePageSequel(QDomElement page, QString sequel);
    QString exportImages(QString html,QDir dir);
    void stripHtml(QString &html);
    void rebuildPagesHash();
    void init();
public:
    QString getTitle() const;
    void setTitle(const QString &value);
    QString getStartPage() const;
    void setStartPage(QString & value);
    QDomElement getPage(QString id);

    bool isModified() const;
    void setModified(bool value);
    QString getAuthor() const;
    void setAuthor(const QString &value);
    void maybeAppendToHistory(QString page);
    QStringList getHistory();
    QString getAbstract();
    void setAbstract(QString value);

    QString getCurrentPageId() const;
    void setCurrentPageId(const QString &value);
    QString getCurrentPageTitle();
    void setCurrentPageTitle(QString value);
    QString getCurrentPageChapter();
    void setCurrentPageChapter(QString value);
    void addCurrentPageSequel(QString id);
    void removeCurrentPageSequel(QString id);
    QDomNodeList getCurrentPageSequels();
    void setCurrentPageSequelText(int row,QString text);
    QString getCurrentPageText();
    void setCurrentPageText(QString html);
    QString getPageTitle(QString id);
    void load(QString path);
    void save(QString path);
    Document();
    ~Document();
    QDomElement getChapter(QString title);
    QStringList getChapters();
    void changeChapterName(int row, QString newName);
    void moveChapterTop(int row);
    QDomDocument *getDomDocument() const;
    QString newPage();
    void newChapter(QString title);
    void deleteCurrentPage();
    void addChapterActions(QMenu * chapterMenu);
    void addSequelsItems(QListWidget * sequelsWidget);
    void moveSequelDown(QString id);
    void moveSequelUp(QString id);
    void moveSequelTop(QString id);
    void moveSequelBottom(QString id);
    void setCurrentPageSequelsFontFamily(QString font);
    void setCurrentPageSequelsFontSize(QString size);
    QFont getCurrentPageSequelsFont();
    QDomElement getCurrentPage() const;
    QString exportPageToHtml(QDomElement page);
    void exportBookToHtml(QString path);
    void addHistoryActions(QMenu * historyMenu);
    void addChaptersItems(QListWidget *chaptersWidget);
    void deleteChapter(QString title);
    void moveChapterUp(int row);
    void moveChapterBottom(int row);
    void moveChapterDown(int row);
    void setPageText(QString page, QString text);
    void addPageSequel(QString page, QString text, QString id);
signals:
    void contentsChanged(void);
public slots:
    void clearHistoryList();

};

#endif // DOCUMENT_H
