#include "narytreedialog.h"
#include "ui_narytreedialog.h"

NaryTreeDialog::NaryTreeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NaryTreeDialog)
{
    ui->setupUi(this);

    eventsList = new EventList(this);
    connect(eventsList,SIGNAL(countChanged(int)),this,SLOT(countChanged(int)));

    ui->eventsLayout->addWidget(eventsList);
    connect(ui->buttonBox->button(QDialogButtonBox::Ok),SIGNAL(clicked(bool)),this,SLOT(accept()));
    connect(ui->buttonBox->button(QDialogButtonBox::Cancel),SIGNAL(clicked(bool)),this,SLOT(reject()));
    ui->buttonBox->button(QDialogButtonBox::Ok)->setDefault(true);
}

NaryTreeDialog::~NaryTreeDialog()
{
    delete ui;
}

QStringList NaryTreeDialog::getEvents()
{
    return eventsList->getEvents();
}

void NaryTreeDialog::countChanged(int value)
{
    int count = 1 ;
    int last_layer = 1 ;
    for (int i = value ; i > 0 ; i--)
    {
        int layer_count = last_layer * i ;
        count += layer_count ;
        last_layer = layer_count ;
    }
    ui->label_2->setText(tr("Si vous cliquez sur OK, un nouveau chapitre sera créé comptant %1 pages.").arg(count));
}
