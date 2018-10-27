#ifndef PSEUDORANDOMDIALOG_H
#define PSEUDORANDOMDIALOG_H

#include "eventlist.h"

#include <QDialog>
#include <QPushButton>

namespace Ui {
class PseudoRandomDialog;
}

class PseudoRandomDialog : public QDialog
{
    Q_OBJECT

    EventList * eventsList ;
    bool isPrime(int n);
    QVector<QVector<int> > primitiveElementsSeries;
    void findPrimitiveElements(int n);
public:
    explicit PseudoRandomDialog(QWidget *parent = 0);
    ~PseudoRandomDialog();

    QStringList getEvents();
    QVector<QVector<int> > getPrimitiveElementsSeries() const;

private slots:
    void countChanged(int value);
private:
    Ui::PseudoRandomDialog *ui;
};

#endif // PSEUDORANDOMDIALOG_H
