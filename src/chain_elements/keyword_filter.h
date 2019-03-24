// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// Copyright 2019 Artem Yamshanov, me [at] anticode.ninja

#ifndef KEYWORD_FILTER_H
#define KEYWORD_FILTER_H

#include <QList>

#include "log_item.h"
#include "chain_element.h"

class KeywordFilter : public ChainElement
{
public:
    KeywordFilter();

    virtual const char* name() {
        return "Keyword Filter";
    }

    virtual void createUI(QGridLayout* layout);

    virtual void accept(std::shared_ptr<LogItem> item);

private:
    QList<QString> _keywords;
};

#endif // KEYWORD_FILTER_H
