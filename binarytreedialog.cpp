#include "binarytreedialog.h"
#include "ui_binarytreedialog.h"

BinaryTreeDialog::BinaryTreeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BinaryTreeDialog)
{
    ui->setupUi(this);

    eventsList = new EventList(this);
    connect(eventsList,SIGNAL(countChanged(int)),this,SLOT(countChanged(int)));

    ui->eventsLayout->addWidget(eventsList);
    connect(ui->buttonBox->button(QDialogButtonBox::Ok),SIGNAL(clicked(bool)),this,SLOT(accept()));
    connect(ui->buttonBox->button(QDialogButtonBox::Cancel),SIGNAL(clicked(bool)),this,SLOT(reject()));
    ui->buttonBox->button(QDialogButtonBox::Ok)->setDefault(true);
}

BinaryTreeDialog::~BinaryTreeDialog()
{
    delete ui;
}

QStringList BinaryTreeDialog::getEvents()
{
    return eventsList->getEvents();
}

void BinaryTreeDialog::countChanged(int value)
{
    ui->label_2->setText(tr("Si vous cliquez sur OK, un nouveau chapitre sera créé comptant %1 pages.").arg((2<<value)-1));
}
