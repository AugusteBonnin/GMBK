#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "document.h"
#include "form.h"

#include <QMainWindow>
#include <QToolButton>

#include <hunspell/hunspell.hxx>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
//! [class definition with macro]
    explicit MainWindow(const QString &fileName);

    void tile(const QMainWindow *previous);
    Hunspell * getSpellChecker(){return m_Hunspell;}
    QTextCodec * getTextCodec(){return m_Codec;}
protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void newFile();
    void newPage();
    void deletePage();
    void gotoPage();
    void open();
    bool save();
    bool saveAs();
    void updateRecentFileActions();
    void openRecentFile();
    void about();
    void documentWasModified();
void rebuidGotoMenu();
void rebuidHistoryMenu();
void clearHistory();
void exportHtml();
void home();
void createAlgebra();
void createBinaryTree();
void createRandom();
void createNaryTree();
private:
    enum { MaxRecentFiles = 5 };

    void init();
    void createActions();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    bool maybeSave();
    void openFile(const QString &fileName);
    void loadFile(const QString &fileName);
    static bool hasRecentFiles();
    void prependToRecentFiles(const QString &fileName);
    void setRecentFilesVisible(bool visible);
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    static QString strippedName(const QString &fullFileName);
    MainWindow *findMainWindow(const QString &fileName) const;

    Hunspell *m_Hunspell;
    QString m_Encoding;
    QTextCodec * m_Codec;

    Form * form ;
Document * document;

    QAction *recentFileActs[MaxRecentFiles];
    QAction *recentFileSeparator;
    QAction *recentFileSubMenuAct;

    QToolButton * gotoButton;
    QMenu * gotoMenu;

    QMenu * historyMenu;

    QString curFile;
    bool isUntitled;private:
    Ui::MainWindow *ui;
    void createNaryNode(QString parent, const QStringList &events, QStringList used);
};

#endif // MAINWINDOW_H
