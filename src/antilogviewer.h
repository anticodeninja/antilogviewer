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
class QFrame;

class AntiLogViewer : public QMainWindow
{
    Q_OBJECT

public:
    AntiLogViewer(QWidget *parent = 0);
    ~AntiLogViewer();

private:
    void addChainElement(ChainElement *element);
    void moveChainElement(ChainElement *element, bool up);
    void removeChainElement(ChainElement *element);
    void configureProfileButton();

    TableModel* _logModel;
    Terminator* _terminator;
    QList<ChainElement*> _chain;
    QBoxLayout* _filtersLayout;
    QPushButton* _profileButton;

    bool _autoScroll;
};

#endif // ANTILOGVIEWER_H
