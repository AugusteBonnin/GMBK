
#include "mainwindow.h"
#include "textedit.h"

#include <QApplication>
#include <QButtonGroup>
#include <QColorDialog>
#include <QFileDialog>
#include <QGridLayout>
#include <QDebug>

#include <hunspell/hunspell.hxx>


TextEdit::TextEdit(MainWindow *parent)
    : QWidget(parent)
{

    QGridLayout * layout = new QGridLayout;
    setLayout(layout);

    const QSize iconSize(32,32);

    const QIcon boldIcon(":/images/bold.png");
    actionTextBold = new QPushButton(this);
    actionTextBold->setIcon(boldIcon);
    actionTextBold->setIconSize(iconSize);
    actionTextBold->setToolTip(tr("Gras"));
    connect(actionTextBold,SIGNAL(clicked(bool)), this, SLOT(textBold()));
    actionTextBold->setCheckable(true);
    layout->addWidget(actionTextBold,0,0);

    const QIcon italicIcon(":/images/italic.png");
    actionTextItalic = new QPushButton(this);
    actionTextItalic->setIcon(italicIcon);
    actionTextItalic->setIconSize(iconSize);
    actionTextItalic->setToolTip(tr("Italique"));
    connect(actionTextItalic,SIGNAL(clicked(bool)), this, SLOT(textItalic()));
    actionTextItalic->setCheckable(true);
    layout->addWidget(actionTextItalic,0,1);

    const QIcon underlineIcon(":/images/underline.png");
    actionTextUnderline = new QPushButton(this);
    actionTextUnderline->setIcon(underlineIcon);
    actionTextUnderline->setIconSize(iconSize);
    actionTextUnderline->setToolTip( tr("Souligné"));
    connect(actionTextUnderline,SIGNAL(clicked(bool)), this, SLOT(textUnderline()));
    actionTextUnderline->setCheckable(true);
    layout->addWidget(actionTextUnderline,0,2);

    QButtonGroup * alignGroup = new QButtonGroup(this);

    const QIcon leftIcon(":/images/textleft.png");
    actionAlignLeft = new QPushButton(this);
    actionAlignLeft->setIcon(leftIcon);
    actionAlignLeft->setIconSize(iconSize);
    actionAlignLeft->setToolTip(tr("Aligné à gauche"));
    actionAlignLeft->setCheckable(true);
    connect(actionAlignLeft,SIGNAL(clicked(bool)), this, SLOT(textAlign()));
    layout->addWidget(actionAlignLeft,0,3);
    alignGroup->addButton(actionAlignLeft);

    const QIcon centerIcon(":/images/textcenter.png");
    actionAlignCenter = new QPushButton(this);
    actionAlignCenter->setIcon(centerIcon);
    actionAlignCenter->setIconSize(iconSize);
    actionAlignCenter->setToolTip(tr("Centré"));
    actionAlignCenter->setCheckable(true);
    connect(actionAlignCenter,SIGNAL(clicked(bool)), this, SLOT(textAlign()));
    layout->addWidget(actionAlignCenter,0,4);
    alignGroup->addButton(actionAlignCenter);

    const QIcon rightIcon(":/images/textright.png");
    actionAlignRight = new QPushButton(this);
    actionAlignRight->setIcon(rightIcon);
    actionAlignRight->setIconSize(iconSize);
    actionAlignRight->setToolTip(tr("Aligné à droite"));
    actionAlignRight->setCheckable(true);
    connect(actionAlignRight,SIGNAL(clicked(bool)), this, SLOT(textAlign()));
    layout->addWidget(actionAlignRight,0,5);
    alignGroup->addButton(actionAlignRight);

    const QIcon fillIcon(":/images/textjustify.png");
    actionAlignJustify = new QPushButton(this);
    actionAlignJustify->setIcon(fillIcon);
    actionAlignJustify->setIconSize(iconSize);
    actionAlignJustify->setToolTip(tr("Justifié"));
    actionAlignJustify->setCheckable(true);
    connect(actionAlignJustify,SIGNAL(clicked(bool)), this, SLOT(textAlign()));
    layout->addWidget(actionAlignJustify,0,6);
    alignGroup->addButton(actionAlignJustify);

    QPixmap pix(32,32);
    pix.fill(Qt::black);
    actionTextColor = new QPushButton(this);
    actionTextColor->setIcon(QIcon(pix));
    actionTextColor->setIconSize(iconSize);
    actionTextColor->setToolTip(tr("Couleur"));
    connect(actionTextColor,SIGNAL(clicked(bool)), this, SLOT(textColor()));
    layout->addWidget(actionTextColor,0,7);


    comboFont = new QFontComboBox(this);
    connect(comboFont, QOverload<const QString &>::of(&QComboBox::activated), this, &TextEdit::textFamily);
    layout->addWidget(comboFont,0,8);

    comboSize = new QComboBox(this);
    comboSize->setObjectName("comboSize");
    comboSize->setEditable(true);

    const QList<int> standardSizes = QFontDatabase::standardSizes();
    foreach (int size, standardSizes)
        comboSize->addItem(QString::number(size));
    comboSize->setCurrentIndex(standardSizes.indexOf(QApplication::font().pointSize()));

    connect(comboSize, QOverload<const QString &>::of(&QComboBox::activated), this, &TextEdit::textSize);
    layout->addWidget(comboSize,0,9);

    QPushButton * image = new QPushButton(this);
    image->setIcon(QIcon(":/images/image.png"));
    image->setIconSize(iconSize);
    image->setToolTip(tr("Insérer une image..."));
    layout->addWidget(image,0,10);
    connect(image,SIGNAL(clicked(bool)),this,SLOT(insertImage()));

    textEdit = new QTextEdit(this);
    connect(textEdit, &QTextEdit::currentCharFormatChanged,
            this, &TextEdit::currentCharFormatChanged);
    connect(textEdit, &QTextEdit::cursorPositionChanged,
            this, &TextEdit::cursorPositionChanged);
    connect(textEdit,SIGNAL(textChanged()),this,SLOT(contentChanged()));
    textEdit->setUndoRedoEnabled(true);
    layout->addWidget(textEdit,1,0,1,11);


    QFont textFont("Helvetica");
    textFont.setStyleHint(QFont::SansSerif);
    textEdit->setFont(textFont);
    fontChanged(textEdit->font());
    colorChanged(textEdit->textColor());
    alignmentChanged(textEdit->alignment());

    textEdit->setFocus();

    textEdit->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(textEdit, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(showContextMenu(const QPoint &)));

    hunspell = parent->getSpellChecker();
    codec = parent->getTextCodec();

}

QString TextEdit::toHtml()
{
    return textEdit->toHtml();
}

void TextEdit::setHtml(QString html)
{
    textEdit->setHtml(html);
}

void TextEdit::textBold()
{
    QTextCharFormat fmt;
    fmt.setFontWeight(actionTextBold->isChecked() ? QFont::Bold : QFont::Normal);
    mergeFormatOnWordOrSelection(fmt);
}

void TextEdit::textUnderline()
{
    QTextCharFormat fmt;
    fmt.setFontUnderline(actionTextUnderline->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}

void TextEdit::textItalic()
{
    QTextCharFormat fmt;
    fmt.setFontItalic(actionTextItalic->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}

void TextEdit::textFamily(const QString &f)
{
    QTextCharFormat fmt;
    fmt.setFontFamily(f);
    mergeFormatOnWordOrSelection(fmt);
}

void TextEdit::textSize(const QString &p)
{
    qreal pointSize = p.toFloat();
    if (p.toFloat() > 0) {
        QTextCharFormat fmt;
        fmt.setFontPointSize(pointSize);
        mergeFormatOnWordOrSelection(fmt);
    }
}


void TextEdit::textColor()
{
    QColor col = QColorDialog::getColor(textEdit->textColor(), this);
    if (!col.isValid())
        return;
    QTextCharFormat fmt;
    fmt.setForeground(col);
    mergeFormatOnWordOrSelection(fmt);
    colorChanged(col);
}

void TextEdit::textAlign()
{
    QPushButton * a=(QPushButton*)sender();
    if (a == actionAlignLeft)
        textEdit->setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
    else if (a == actionAlignCenter)
        textEdit->setAlignment(Qt::AlignHCenter);
    else if (a == actionAlignRight)
        textEdit->setAlignment(Qt::AlignRight | Qt::AlignAbsolute);
    else if (a == actionAlignJustify)
        textEdit->setAlignment(Qt::AlignJustify);
}

void TextEdit::currentCharFormatChanged(const QTextCharFormat &format)
{
    fontChanged(format.font());
    colorChanged(format.foreground().color());
}

void TextEdit::cursorPositionChanged()
{
    alignmentChanged(textEdit->alignment());
}

void TextEdit::clipboardDataChanged()
{

}

void TextEdit::contentChanged()
{
    emit textChanged();

    QString text = textEdit->toPlainText();
    text = text.replace('.',' ');
    text = text.replace(',',' ');
    text = text.replace('-',' ');
    text = text.replace('"',' ');
    text = text.replace('?',' ');
    text = text.replace('!',' ');
    text = text.replace(';',' ');
    text = text.replace(':',' ');
    text = text.replace("'"," ");
    QStringList list = text.split(' ',QString::SkipEmptyParts);
    QStringList misspelled_words;
    QStringList goodspelled_words;
    for (int i = 0 ; i < list.count() ; ++i)
        if ((list[i].length()>3)&&(!isSpellingCorrect(list[i])))
            misspelled_words << list[i] ;
        else
            goodspelled_words << list [i] ;

    disconnect(textEdit,SIGNAL(textChanged()),this,SLOT(contentChanged()));

    QTextDocument * doc = textEdit->document();
    QTextCharFormat format;
    format.setFontUnderline(false);
    for (int i = 0 ; i < goodspelled_words.count() ; ++i)
    {
        QTextCursor cursor = doc->find(goodspelled_words[i]);
        do
        {
            cursor.mergeCharFormat(format);
            cursor = doc->find(goodspelled_words[i],cursor);
        }
        while(cursor!=QTextCursor());
    }

    format.setUnderlineColor(QColor(Qt::red));
    format.setFontUnderline(true);
    for (int i = 0 ; i < misspelled_words.count() ; ++i)
    {
        QTextCursor cursor = doc->find(misspelled_words[i]);
        do
        {
            cursor.mergeCharFormat(format);
            cursor = doc->find(misspelled_words[i],cursor);
        }
        while(cursor!=QTextCursor());

    }
    connect(textEdit,SIGNAL(textChanged()),this,SLOT(contentChanged()));


}

bool TextEdit::isSpellingCorrect(const QString &word) const {
    bool isOk = false;
    try {
        isOk = hunspell->spell(codec->fromUnicode(word).constData());
    }
    catch (...) {
        isOk = false;
    }
    return isOk;
}

void TextEdit::showContextMenu(const QPoint & pos)
{
    QMenu* stdMenu= textEdit->createStandardContextMenu();
    tc = textEdit->cursorForPosition(pos);
    tc.select(QTextCursor::WordUnderCursor);
    QString word = tc.selectedText();

    if (!isSpellingCorrect(word))
    {
        QStringList suggestions = suggestCorrections(word);
        QAction *first = stdMenu->actions().first();

        for (int i = 0 ; i < suggestions.count() ; ++i)
        {
            QAction * action = new QAction(suggestions[i],stdMenu) ;
            stdMenu->insertAction(first,action);
            connect(action,SIGNAL(triggered()),this,SLOT(correct()));
        }
        stdMenu->insertSeparator(first);
    }

    stdMenu->popup(textEdit->viewport()->mapToGlobal(pos));
}

QStringList TextEdit::suggestCorrections(const QString &word) {
    QStringList suggestions;
    char **suggestWordList = NULL;

    try {
        // Encode from Unicode to the encoding used by current dictionary
        int count = hunspell->suggest(&suggestWordList, codec->fromUnicode(word).constData());

        for (int i = 0; i < count; ++i) {
            QString suggestion = codec->toUnicode(suggestWordList[i]);
            suggestions << suggestion;
            free(suggestWordList[i]);
        }
    }
    catch (...) {
        qDebug() << "Error for keyword:" << word;
    }

    return suggestions;
}

void TextEdit::correct(void)
{
    QAction * action = (QAction*)sender();
    tc.insertText(action->text(),QTextCharFormat());
}

void TextEdit::insertImage()
{
    const QString fileName = QFileDialog::getOpenFileName(this,tr("Sélectionnez l'image que vous désirez insérer."),QDir::homePath(),"*.png *.jpg");
    if (fileName!=QString())
    {
        QString html("<IMG src=\"");
        html.append(fileName);
        html.append("\" />");
        textEdit->insertHtml(html);
    }
}


void TextEdit::mergeFormatOnWordOrSelection(const QTextCharFormat &format)
{
    QTextCursor cursor = textEdit->textCursor();
    if (!cursor.hasSelection())
        cursor.select(QTextCursor::WordUnderCursor);
    cursor.mergeCharFormat(format);
    textEdit->mergeCurrentCharFormat(format);
}

void TextEdit::fontChanged(const QFont &f)
{
    comboFont->setCurrentIndex(comboFont->findText(QFontInfo(f).family()));
    comboSize->setCurrentIndex(comboSize->findText(QString::number(f.pointSize())));
    actionTextBold->setChecked(f.bold());
    actionTextItalic->setChecked(f.italic());
    actionTextUnderline->setChecked(f.underline());
}

void TextEdit::colorChanged(const QColor &c)
{
    QPixmap pix(32, 32);
    pix.fill(c);
    actionTextColor->setIcon(pix);
}

void TextEdit::alignmentChanged(Qt::Alignment a)
{
    if (a & Qt::AlignLeft)
        actionAlignLeft->setChecked(true);
    else if (a & Qt::AlignHCenter)
        actionAlignCenter->setChecked(true);
    else if (a & Qt::AlignRight)
        actionAlignRight->setChecked(true);
    else if (a & Qt::AlignJustify)
        actionAlignJustify->setChecked(true);
}

