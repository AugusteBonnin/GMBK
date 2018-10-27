#ifndef FORM_H
#define FORM_H

#include "document.h"
#include "textedit.h"

#include <QCompleter>
#include <QStringListModel>
#include <QUrl>
#include <QWidget>

namespace Ui {
class Form;
}

class Form : public QWidget
{
    Q_OBJECT
Document *doc;
QMenu * addSequelMenu;
QListWidget * sequels;
public:
    explicit Form(QWidget *parent, Document *doc);
    ~Form();

    QFont getSequelsFont();
public slots:
void updateBook();
void updatePage();
private slots:
void on_bookTitle_returnPressed();

void on_author_returnPressed();

void absract_textChanged();

void on_pageTitle_returnPressed();

void text_textChanged();

void chapterComboBox_currentTextChanged(const QString &arg1);

void rebuildAddSequelMenu();

void addSequel();

void rebuildSequelsList();
void on_delSequelButton_clicked();

void on_sequelDown_clicked();

void on_sequelup_clicked();

void on_sequelTopButton_clicked();

void on_sequelBottomButton_clicked();

void preview();

void openLink(QUrl url);
void sequelsFont(QString f);
void sequelsSize(QString f);

void sequelChanged(int row);
void on_sequelText_returnPressed();

void on_addChapterButton_clicked();

void on_delChapterButton_clicked();


void on_sequelsFontButton_clicked();

void chapterChanged(QListWidgetItem * item);

void on_topChapterButton_clicked();

void on_upChapterButton_clicked();

void on_downChapterButton_clicked();

void on_bottomChapterButton_clicked();
private:
    Ui::Form *ui;

    TextEdit * abstract;
    TextEdit * text;
    void sequelsFontChanged(const QFont &f);
    void rebuildChaptersList();
};

#endif // FORM_H
