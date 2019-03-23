#ifndef ANTILOGVIEWER_H
#define ANTILOGVIEWER_H

#include <QMainWindow>
#include <QList>

class ChainElement;
class TableModel;
class Terminator;

class QLineEdit;
class QPushButton;
class QComboBox;
class QBoxLayout;
class QGridLayout;

class AntiLogViewer : public QMainWindow
{
    Q_OBJECT

public:
    AntiLogViewer(QWidget *parent = 0);
    ~AntiLogViewer();

private:
    QWidget* createToolSet(ChainElement *element);
    void updateChain(int start, int remove, int add);
    void configureProfileButton();

    TableModel* _logModel;
    Terminator* _terminator;
    QList<ChainElement*> _chain;
    QBoxLayout* _filtersLayout;
    QPushButton* _profileButton;

    bool _autoScroll;
};

#endif // ANTILOGVIEWER_H
