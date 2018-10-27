#ifndef BINARYTREEDIALOG_H
#define BINARYTREEDIALOG_H

#include "eventlist.h"

#include <QDialog>
#include <QPushButton>

namespace Ui {
class BinaryTreeDialog;
}

class BinaryTreeDialog : public QDialog
{
    Q_OBJECT

    EventList * eventsList;

public:
    explicit BinaryTreeDialog(QWidget *parent = 0);
    ~BinaryTreeDialog();

    QStringList getEvents();
private slots:
    void countChanged(int value);
private:
    Ui::BinaryTreeDialog *ui;
};

#endif // BINARYTREEDIALOG_H
