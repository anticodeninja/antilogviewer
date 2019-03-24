// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// Copyright 2019 Artem Yamshanov, me [at] anticode.ninja

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
