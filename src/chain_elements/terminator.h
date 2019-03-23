#ifndef TERMINATOR_H
#define TERMINATOR_H

#include "chain_elements/chain_element.h"

class TableModel;

class Terminator : public ChainElement
{
public:
    Terminator(TableModel* tableModel)
        : _tableModel(tableModel)
    {
    }

    virtual const char *name() {
        return "Terminator";
    }

    virtual void createUI(QGridLayout *layout) {
    }

    virtual void accept(std::shared_ptr<LogItem> item);

private:
    TableModel* _tableModel;

};

#endif // TERMINATOR_H
