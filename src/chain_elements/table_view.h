// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// Copyright 2019 Artem Yamshanov, me [at] anticode.ninja

#ifndef TERMINATOR_H
#define TERMINATOR_H

#include "chain_elements/chain_element.h"

class TableModel;

class TableView : public ChainElement
{
public:
    TableView(TableModel* tableModel);

    virtual ~TableView();

    virtual const QString name() const { return "Table View"; }
    virtual ChainElementType type() const { return ChainElementType::Sink; }

    virtual void createUI(QGridLayout *layout);

    virtual void load(const QJsonObject& data);
    virtual void save(QJsonObject& data) const;

    virtual void accept(std::shared_ptr<LogItem> item);

    static void setGlobalPalette(QColor text, QColor back);
    static void setDefaultPalette();

private:
    TableModel* _tableModel;
};

#endif // TERMINATOR_H
