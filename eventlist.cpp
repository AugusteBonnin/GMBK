#include "eventlist.h"
#include "ui_eventlist.h"

EventList::EventList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EventList)
{
    ui->setupUi(this);

    ui->addButton->setIcon(QIcon(":/images/add.png"));
    ui->addButton->setIconSize(QSize(32,32));
    ui->addButton->setToolTip(tr("Ajouter un évènement"));
    ui->addButton->setStatusTip(tr("Ajouter un évènement"));

    ui->delButton->setIcon(QIcon(":/images/del.png"));
    ui->delButton->setIconSize(QSize(32,32));
    ui->delButton->setToolTip(tr("Supprimer l'évènement sélectionné"));
    ui->delButton->setStatusTip(tr("Supprimer l'évènement sélectionné"));

    ui->topButton->setIcon(QIcon(":/images/top.png"));
    ui->topButton->setIconSize(QSize(32,32));
    ui->topButton->setToolTip(tr("Amène l'évènement sélectionné en haut de la liste"));
    ui->topButton->setStatusTip(tr("Amène l'évènement sélectionné en haut de la liste"));

    ui->upButton->setIcon(QIcon(":/images/up.png"));
    ui->upButton->setIconSize(QSize(32,32));
    ui->upButton->setToolTip(tr("Déplace l'évènement sélectionné vers le haut de la liste"));
    ui->upButton->setStatusTip(tr("Déplace l'évènement sélectionné vers le haut de la liste"));

    ui->bottomButton->setIcon(QIcon(":/images/bottom.png"));
    ui->bottomButton->setIconSize(QSize(32,32));
    ui->bottomButton->setToolTip(tr("Amène l'évènement sélectionné en bas de la liste"));
    ui->bottomButton->setStatusTip(tr("Amène l'évènement sélectionné en bas de la liste"));

    ui->downButton->setIcon(QIcon(":/images/down.png"));
    ui->downButton->setIconSize(QSize(32,32));
    ui->downButton->setToolTip(tr("Déplace l'évènement sélectionné vers le bas de la liste"));
    ui->downButton->setStatusTip(tr("Déplace l'évènement sélectionné vers le bas de la liste"));

}

EventList::~EventList()
{
    delete ui;
}

QStringList EventList::getEvents()
{
    QStringList events;
    for (int i = 0 ; i < ui->listWidget->count() ; ++i)
    {
        QListWidgetItem * item = ui->listWidget->item(i) ;
        events << item->text() ;
    }
    return events ;
}

void EventList::on_delButton_clicked()
{
    if (ui->listWidget->currentRow()>-1)
    {
        delete ui->listWidget->takeItem(ui->listWidget->currentRow());
        emit countChanged(ui->listWidget->count());
    }
}

void EventList::on_addButton_clicked()
{
    static int id = 1 ;
    QListWidgetItem *item = new QListWidgetItem(tr("Evènement %1").arg(id++));
    item->setFlags(Qt::ItemIsEditable|item->flags());
    ui->listWidget->addItem(item);
    emit countChanged(ui->listWidget->count());
}

void EventList::on_topButton_clicked()
{
    int row = ui->listWidget->currentRow();
    if (row>0)
    {
        ui->listWidget->insertItem(0,ui->listWidget->takeItem(row));
        ui->listWidget->setCurrentRow(0);
    }
}

void EventList::on_upButton_clicked()
{
    int row = ui->listWidget->currentRow();
    if (row>0)
    {
        ui->listWidget->insertItem(row-1,ui->listWidget->takeItem(row));
        ui->listWidget->setCurrentRow(row-1);
    }
}

void EventList::on_bottomButton_clicked()
{
    int row = ui->listWidget->currentRow();
    if ((row>-1)&&(row<ui->listWidget->count()-1))
    {
        ui->listWidget->addItem(ui->listWidget->takeItem(row));
        ui->listWidget->setCurrentRow(ui->listWidget->count()-1);
    }
}

void EventList::on_downButton_clicked()
{
    int row = ui->listWidget->currentRow();
    if ((row>-1)&&(row<ui->listWidget->count()-1))
    {
        ui->listWidget->insertItem(row+1,ui->listWidget->takeItem(row));
        ui->listWidget->setCurrentRow(row+1);
    }
}
