#ifndef TABLE_H
#define TABLE_H

#include <QTableView>

class QDialog;
class QLabel;

class Table : public QTableView
{
public:
    Table(QWidget *parent = Q_NULLPTR);
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QDialog* _popup;
    QLabel* _popupLabel;
};

#endif // TABLE_H
