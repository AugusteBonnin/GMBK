#ifndef EVENTLIST_H
#define EVENTLIST_H

#include <QWidget>

namespace Ui {
class EventList;
}

class EventList : public QWidget
{
    Q_OBJECT

public:
    explicit EventList(QWidget *parent = 0);
    ~EventList();
QStringList getEvents();
private slots:
    void on_delButton_clicked();
    void on_addButton_clicked();
    void on_topButton_clicked();
    void on_upButton_clicked();
    void on_bottomButton_clicked();
    void on_downButton_clicked();
private:
    Ui::EventList *ui;
signals :
    void countChanged(int);
};

#endif // EVENTLIST_H
