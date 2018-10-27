#include "pseudorandomdialog.h"
#include "ui_pseudorandomdialog.h"

bool PseudoRandomDialog::isPrime(int n)
{
    bool result = true ;
    for (int i = 2 ; i < (n >> 1) ; ++i)
        if ((n%i)==0)
        {
            result = false ;
            break;
        }
    return result ;
}

QVector<QVector<int> > PseudoRandomDialog::getPrimitiveElementsSeries() const
{
    return primitiveElementsSeries;
}

void PseudoRandomDialog::findPrimitiveElements(int n)
{
    primitiveElementsSeries.clear();
    for (int i = 2 ; i < n ; ++i)
    {
        int s = 1 ;
        int iterations = 1 ;
        QVector<int> serie;
        do {
            serie << s ;
            s*=i;
            s%=n;
            iterations ++ ;
        } while ((s != 1)&&(iterations <= n));
        if ((s == 1)&&(iterations == n))
            primitiveElementsSeries << serie ;
    }
}

PseudoRandomDialog::PseudoRandomDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PseudoRandomDialog)
{
    ui->setupUi(this);

    eventsList = new EventList(this);
    connect(eventsList,SIGNAL(countChanged(int)),this,SLOT(countChanged(int)));

    ui->eventsLayout->addWidget(eventsList);
    connect(ui->buttonBox->button(QDialogButtonBox::Ok),SIGNAL(clicked(bool)),this,SLOT(accept()));
    connect(ui->buttonBox->button(QDialogButtonBox::Cancel),SIGNAL(clicked(bool)),this,SLOT(reject()));
    ui->buttonBox->button(QDialogButtonBox::Ok)->setDefault(true);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

PseudoRandomDialog::~PseudoRandomDialog()
{
    delete ui;
}

QStringList PseudoRandomDialog::getEvents()
{
    return eventsList->getEvents();
}

void PseudoRandomDialog::countChanged(int value)
{
    if (isPrime(value+1))
    {
        findPrimitiveElements(value+1);
        QString text =tr("Si vous cliquez sur OK, un nouveau chapitre sera créé comptant %1 pages.").arg(primitiveElementsSeries.count()+1);
//        for (int i = 0 ; i < primitiveElementsSeries.count() ; ++i)
//        {
//            text.append("(");
//            for (int j = 0 ; j < primitiveElementsSeries[i].count() ; ++j)
//                text.append(QString(" %1 ").arg(primitiveElementsSeries[i][j]));
//            text.append(")");
//        }
        ui->label_2->setText(text);
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    }
    else
    {
        ui->label_2->setText(tr("Le nombre d'évènements ne permet pas un parcours pseudo-aléatoire."));
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    }
}
