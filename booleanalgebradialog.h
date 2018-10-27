#ifndef BOOLEANALGEBRADIALOG_H
#define BOOLEANALGEBRADIALOG_H

#include "eventlist.h"

#include <QDialog>
#include <QListWidget>
#include <QPushButton>

namespace Ui {
class BooleanAlgebraDialog;
}

class BooleanAlgebraDialog : public QDialog
{
    Q_OBJECT
    EventList * eventsList ;

public:
    explicit BooleanAlgebraDialog(QWidget *parent = 0);
    ~BooleanAlgebraDialog();
QStringList getEvents();
private:
    Ui::BooleanAlgebraDialog *ui;
private slots:

    void countChanged(int value);

};

#endif // BOOLEANALGEBRADIALOG_H
