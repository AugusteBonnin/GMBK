#ifndef NARYTREEDIALOG_H
#define NARYTREEDIALOG_H

#include "eventlist.h"

#include <QDialog>
#include <QPushButton>

namespace Ui {
class NaryTreeDialog;
}

class NaryTreeDialog : public QDialog
{
    Q_OBJECT

    EventList * eventsList;

public:
    explicit NaryTreeDialog(QWidget *parent = 0);
    ~NaryTreeDialog();

    QStringList getEvents();
private slots:
    void countChanged(int value);
private:
    Ui::NaryTreeDialog *ui;
};

#endif // NARYTREEDIALOG_H
