#include "form.h"
#include "mainwindow.h"
#include "ui_form.h"

#include <QFontDialog>
#include <QListWidget>
#include <QMessageBox>
#include <QStandardItem>
#include <QTextBrowser>

Form::Form(MainWindow *parent,Document * doc) :
    QWidget(parent),
    ui(new Ui::Form),doc(doc)
{
    ui->setupUi(this);

    abstract = new TextEdit(parent);
    ui->abstractLayout->addWidget(abstract);
    connect(abstract,SIGNAL(textChanged()),this,SLOT(absract_textChanged()));

    ui->addChapterButton->setIcon(QIcon(":/images/add.png"));
    ui->addChapterButton->setIconSize(QSize(32,32));
    ui->addChapterButton->setToolTip(tr("Ajouter un chapitre"));
    ui->addChapterButton->setStatusTip(tr("Ajouter un chapitre"));

    ui->delChapterButton->setIcon(QIcon(":/images/del.png"));
    ui->delChapterButton->setIconSize(QSize(32,32));
    ui->delChapterButton->setToolTip(tr("Supprimer le chapitre sélectionné"));
    ui->delChapterButton->setStatusTip(tr("Supprimer le chapitre sélectionné"));

    ui->topChapterButton->setIcon(QIcon(":/images/top.png"));
    ui->topChapterButton->setIconSize(QSize(32,32));
    ui->topChapterButton->setToolTip(tr("Amène le chapitre sélectionné en haut de la liste"));
    ui->topChapterButton->setStatusTip(tr("Amène le chapitre sélectionné en haut de la liste"));

    ui->upChapterButton->setIcon(QIcon(":/images/up.png"));
    ui->upChapterButton->setIconSize(QSize(32,32));
    ui->upChapterButton->setToolTip(tr("Déplace le chapitre sélectionné vers le haut de la liste"));
    ui->upChapterButton->setStatusTip(tr("Déplace le chapitre sélectionné vers le haut de la liste"));

    ui->bottomChapterButton->setIcon(QIcon(":/images/bottom.png"));
    ui->bottomChapterButton->setIconSize(QSize(32,32));
    ui->bottomChapterButton->setToolTip(tr("Amène le chapitre sélectionné en bas de la liste"));
    ui->bottomChapterButton->setStatusTip(tr("Amène le chapitre sélectionné en bas de la liste"));

    ui->downChapterButton->setIcon(QIcon(":/images/down.png"));
    ui->downChapterButton->setIconSize(QSize(32,32));
    ui->downChapterButton->setToolTip(tr("Déplace le chapitre sélectionné vers le bas de la liste"));
    ui->downChapterButton->setStatusTip(tr("Déplace le chapitre sélectionné vers le bas de la liste"));

    text = new TextEdit(parent);
    ui->textLayout->addWidget(text);
    connect(text,SIGNAL(textChanged()),this,SLOT(text_textChanged()));

    ui->addSequelButton->setIcon(QIcon(":/images/add.png"));
    ui->addSequelButton->setIconSize(QSize(32,32));
    ui->addSequelButton->setToolTip(tr("Ajouter un lien"));
    ui->addSequelButton->setStatusTip(tr("Ajouter un lien"));

    ui->delSequelButton->setIcon(QIcon(":/images/del.png"));
    ui->delSequelButton->setIconSize(QSize(32,32));
    ui->delSequelButton->setToolTip(tr("Supprimer le lien selectionné de la liste"));
    ui->delSequelButton->setStatusTip(tr("Supprimer le lien selectionné de la liste"));

    ui->sequelTopButton->setIcon(QIcon(":/images/top.png"));
    ui->sequelTopButton->setIconSize(QSize(32,32));
    ui->sequelTopButton->setToolTip(tr("Amener le lien selectionné en haut de la liste"));
    ui->sequelTopButton->setStatusTip(tr("Amener le lien selectionné en haut de la liste"));

    ui->sequelup->setIcon(QIcon(":/images/up.png"));
    ui->sequelup->setIconSize(QSize(32,32));
    ui->sequelup->setToolTip(tr("Faire monter le lien selectionné dans la liste"));
    ui->sequelup->setStatusTip(tr("Faire monter le lien selectionné dans la liste"));

    ui->sequelDown->setIcon(QIcon(":/images/down.png"));
    ui->sequelDown->setIconSize(QSize(32,32));
    ui->sequelDown->setToolTip(tr("Faire descendre le lien selectionné dans la liste"));
    ui->sequelDown->setStatusTip(tr("Faire descendre le lien selectionné dans la liste"));

    ui->sequelBottomButton->setIcon(QIcon(":/images/bottom.png"));
    ui->sequelBottomButton->setIconSize(QSize(32,32));
    ui->sequelBottomButton->setToolTip(tr("Amener le lien selectionné en bas de la liste"));
    ui->sequelBottomButton->setStatusTip(tr("Amener le lien selectionné en bas de la liste"));

    sequels = new QListWidget(this);
    connect(sequels,SIGNAL(currentRowChanged(int)),this,SLOT(sequelChanged(int)));
    ui->sequelsLayout->addWidget(sequels,3,1,5,2);

    ui->chapterComboBox->setEditable(false);

    addSequelMenu = new QMenu(this);
    ui->addSequelButton->setMenu(addSequelMenu);

    connect(ui->sequelsFont, SIGNAL(activated(QString)), this,SLOT(sequelsFont(QString)));

    ui->sequelsSize->setEditable(true);
    const QList<int> standardSizes = QFontDatabase::standardSizes();
    foreach (int size, standardSizes)
        ui->sequelsSize->addItem(QString::number(size));
    ui->sequelsSize->setCurrentIndex(standardSizes.indexOf(QApplication::font().pointSize()));

    connect(ui->sequelsSize, SIGNAL(activated(QString)), this, SLOT(sequelsSize(QString)));

    ui->addSequelButton->setPopupMode(QToolButton::InstantPopup);

    connect(doc,SIGNAL(contentsChanged()),this,SLOT(rebuildAddSequelMenu()));
    connect(doc,SIGNAL(contentsChanged()),this,SLOT(rebuildSequelsList()));
    connect(doc,SIGNAL(contentsChanged()),this,SLOT(preview()));

    ui->textBrowser->setOpenLinks(false);
    connect(ui->textBrowser,SIGNAL(anchorClicked(QUrl)),this,SLOT(openLink(QUrl)));

    connect(ui->chaptersList,SIGNAL(itemChanged(QListWidgetItem*)),this,SLOT(chapterChanged(QListWidgetItem*)));

    updateBook();
    updatePage();
}

Form::~Form()
{
    delete ui;
}

QFont Form::getSequelsFont()
{
    return sequels->font();
}

void Form::updateBook()
{
    ui->bookTitle->setText(doc->getTitle());
    ui->author->setText(doc->getAuthor());
    abstract->setHtml(doc->getAbstract());

}

void Form::updatePage()
{
    ui->pageTitle->setText(doc->getCurrentPageTitle());
    QStringList chapters = doc->getChapters() ;
    disconnect(ui->chapterComboBox,SIGNAL(currentTextChanged(QString)),this,SLOT(chapterComboBox_currentTextChanged(QString)));
    ui->chapterComboBox->clear();
    ui->chapterComboBox->addItems(chapters);

    QString chapter = doc->getCurrentPageChapter();
    int index =chapters.indexOf(QRegExp(chapter));
    ui->chapterComboBox->setCurrentIndex(index);
    connect(ui->chapterComboBox,SIGNAL(currentTextChanged(QString)),this,SLOT(chapterComboBox_currentTextChanged(QString)));
    text->setHtml(doc->getCurrentPageText());
    sequels->setFont(doc->getCurrentPageSequelsFont());
    sequelsFontChanged(sequels->font());

    rebuildAddSequelMenu();
    rebuildSequelsList();
    rebuildChaptersList();
}

void Form::on_bookTitle_returnPressed()
{
    doc->setTitle(ui->bookTitle->text());
    doc->setModified(true);
}

void Form::on_author_returnPressed()
{
    doc->setAuthor(ui->author->text());
    doc->setModified(true);
}

void Form::absract_textChanged()
{
    doc->setAbstract(abstract->toHtml());
    doc->setModified(true);
}

void Form::on_pageTitle_returnPressed()
{
    doc->setCurrentPageTitle(ui->pageTitle->text());
    doc->setModified(true);
}

void Form::text_textChanged()
{
    doc->setCurrentPageText(text->toHtml());
    doc->setModified(true);
}

void Form::chapterComboBox_currentTextChanged(const QString &arg1)
{
    doc->setCurrentPageChapter(arg1);
    doc->setModified(true);
}


void Form::rebuildAddSequelMenu()
{
    addSequelMenu->clear();
    QStringList chapters = doc->getChapters();
    for (int i = 0 ; i < chapters.count() ;++i)
    {
        QMenu * chapter = new QMenu(chapters.at(i),addSequelMenu);
        addSequelMenu->addMenu(chapter);
        doc->addChapterActions(chapter);

        QList<QAction *> actions = chapter->actions();
        for (int j = 0 ; j < actions.count() ; ++j)
            connect(actions.at(j),SIGNAL(triggered(bool)),this,SLOT(addSequel()));
    }

}

void Form::addSequel()
{
    QAction * action = (QAction*)sender();
    QString id = action->data().toString();
    doc->addCurrentPageSequel(id);
    doc->setModified(true);
    updatePage();
}

void Form::rebuildSequelsList()
{
    disconnect(sequels,SIGNAL(currentRowChanged(int)),this,SLOT(sequelChanged(int)));
    int selected = sequels->currentRow();
    sequels->clear();
    doc->addSequelsItems(sequels);
    sequels->setCurrentRow(selected);
    connect(sequels,SIGNAL(currentRowChanged(int)),this,SLOT(sequelChanged(int)));
}

void Form::rebuildChaptersList()
{
    ui->chaptersList->clear();
    QStringList stringList = doc->getChapters();
    for (int i = 0 ; i < stringList.count() ; ++i)
    {
        QListWidgetItem * item = new QListWidgetItem(stringList.at(i)) ;
        item->setFlags(item->flags()|Qt::ItemIsEditable);
        ui->chaptersList->addItem(item);
    }
}

void Form::on_delSequelButton_clicked()
{
    QListWidgetItem *item  = sequels->currentItem();
    if (item)
    {
        doc->removeCurrentPageSequel(item->data(Qt::UserRole).toString());
        doc->setModified(true);
        updatePage();
    }
}

void Form::on_sequelDown_clicked()
{
    QListWidgetItem *item  = sequels->currentItem();
    int index = sequels->currentRow();
    if ((item)&&(index<sequels->count()-1))
    {
        doc->moveSequelDown(item->data(Qt::UserRole).toString());
        doc->setModified(true);
        updatePage();
        sequels->setCurrentRow(index+1);
    }
}

void Form::on_sequelup_clicked()
{
    QListWidgetItem *item  = sequels->currentItem();
    int index = sequels->currentRow();
    if ((item)&&(index>0))
    {
        doc->moveSequelUp(item->data(Qt::UserRole).toString());
        doc->setModified(true);
        updatePage();
        sequels->setCurrentRow(index-1);
    }
}

void Form::on_sequelTopButton_clicked()
{
    QListWidgetItem *item  = sequels->currentItem();
    int index = sequels->currentRow();
    if ((item)&&(index>0))
    {
        doc->moveSequelTop(item->data(Qt::UserRole).toString());
        doc->setModified(true);
        updatePage();
        sequels->setCurrentRow(0);
    }
}

void Form::on_sequelBottomButton_clicked()
{
    QListWidgetItem *item  = sequels->currentItem();
    int index = sequels->currentRow();
    if ((item)&&(index<sequels->count()-1))
    {
        doc->moveSequelBottom(item->data(Qt::UserRole).toString());
        doc->setModified(true);
        updatePage();
        sequels->setCurrentRow(sequels->count()-1);
    }
}

void Form::preview()
{
    ui->textBrowser->setHtml(doc->exportPageToHtml(doc->getCurrentPage()));
}

void Form::openLink(QUrl url)
{
    QString fileName =url.fileName();
    QString id = fileName.left(fileName.length()-5);
    doc->setCurrentPageId(id);
    QStringList history = doc->getHistory();
    if (history.indexOf(QRegExp(id))==-1)
    {
        doc->maybeAppendToHistory(id);
        doc->setModified(true);
    }
    updatePage();
}

void Form::sequelsFont(QString f)
{
    QFont font = sequels->font();
    font.setFamily(f);
    sequels->setFont(font);
    doc->setCurrentPageSequelsFontFamily(f);
    doc->setModified(true);
}

void Form::sequelsSize(QString f)
{
    float p = f.toFloat();
    QFont font = sequels->font();
    font.setPointSize(p);
    sequels->setFont(font);
    doc->setCurrentPageSequelsFontSize(f);
    doc->setModified(true);
}

void Form::sequelChanged(int row)
{
    ui->sequelText->setText(sequels->item(row)->data(Qt::UserRole+1).toString());
}

void Form::sequelsFontChanged(const QFont &f)
{
    ui->sequelsFont->setCurrentIndex(ui->sequelsFont->findText(QFontInfo(f).family()));
    ui->sequelsSize->setCurrentIndex(ui->sequelsSize->findText(QString::number(f.pointSize())));
    //actionTextBold->setChecked(f.bold());
    //actionTextItalic->setChecked(f.italic());
    //actionTextUnderline->setChecked(f.underline());
}

void Form::on_sequelText_returnPressed()
{
    if (sequels->currentRow()>-1)
    {
        doc->setCurrentPageSequelText(sequels->currentRow(),ui->sequelText->text());
        doc->setModified(true);
    }
}

void Form::on_addChapterButton_clicked()
{
    doc->newChapter(tr("Nouveau chapitre"));
    rebuildChaptersList();
    doc->setModified(true);
}

void Form::on_delChapterButton_clicked()
{
    if (ui->chaptersList->currentRow()>-1)
    {
        if(ui->chaptersList->count()>1)
        {
            QMessageBox msgBox;
            msgBox.setText(tr("Vous êtes sur le point de supprimer un chapitre. Les pages qu'il contient seont déplacées dans le dernier chapitre."));
            msgBox.setInformativeText(tr("Etes vous sûr de vouloir effectuer cette action?"));
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No );
            msgBox.setDefaultButton(QMessageBox::No);

            int ret = msgBox.exec();
            switch (ret)
            {
            case QMessageBox::Yes :
            {
                QListWidgetItem * item =  ui->chaptersList->takeItem(ui->chaptersList->currentRow());
                doc->deleteChapter(item->text());
                doc->setModified(true);
                updatePage();
            }
                break ;
            default :
                break ;
            }

        }
        else
            QMessageBox::warning(this,tr("GameBook Editor"),tr("Il est impossible de supprimer le dernier chapitre."));
    }
    else
        QMessageBox::warning(this,tr("GameBook Editor"),tr("Il n'y a aucun chapitre sélectionné."));
}



void Form::on_sequelsFontButton_clicked()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok,this);
    ui->sequelsFontButton->setFont(font);
    ui->sequelsFontButton->setText(font.family()+","+QString::number(font.pointSize()));
}

void Form::chapterChanged(QListWidgetItem *item)
{
    doc->changeChapterName(ui->chaptersList->currentRow(),item->text());
    doc->setModified(true);
}

void Form::on_topChapterButton_clicked()
{
    int row = ui->chaptersList->currentRow() ;
    if (row>0)
    {
        doc->moveChapterTop(row);
        doc->setModified(true);
        rebuildChaptersList();
        ui->chaptersList->setCurrentRow(0);

    }
}

void Form::on_upChapterButton_clicked()
{
    int row = ui->chaptersList->currentRow() ;
    if (row>0)
    {
        doc->moveChapterUp(row);
        doc->setModified(true);
        rebuildChaptersList();
        ui->chaptersList->setCurrentRow(row-1);
    }
}

void Form::on_downChapterButton_clicked()
{
    int row = ui->chaptersList->currentRow() ;
    if ((row<ui->chaptersList->count()-1)&&(row>-1))
    {
        doc->moveChapterDown(row);
        doc->setModified(true);
        rebuildChaptersList();
        ui->chaptersList->setCurrentRow(row+1);
    }
}

void Form::on_bottomChapterButton_clicked()
{
    int row = ui->chaptersList->currentRow() ;
    if ((row<ui->chaptersList->count()-1)&&(row>-1))
    {
        doc->moveChapterBottom(row);
        doc->setModified(true);
        rebuildChaptersList();
        ui->chaptersList->setCurrentRow(ui->chaptersList->count()-1);
    }
}
