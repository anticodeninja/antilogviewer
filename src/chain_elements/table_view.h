// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// Copyright 2019-2021 Artem Yamshanov, me [at] anticode.ninja

#ifndef TERMINATOR_H
#define TERMINATOR_H

#include <QTableView>
#include "chain_elements/chain_element.h"

class TableModel;

class TableView : public ChainElement
{
public:
    TableView(QTableView* tableView, TableModel* tableModel);

    ~TableView() override;

    const QString name() const override { return "Table View"; }
    ChainElementType type() const override { return ChainElementType::Sink; }

    void createUI(QGridLayout *layout) override;

    void load(const QJsonObject& data) override;
    void save(QJsonObject& data) const override;

    void accept(std::shared_ptr<LogItem> item) override;

    void setDefaultSettings();
    static void setGlobalPalette(const QColor& text, const QColor& back);

private:
    QTableView* _tableView;
    TableModel* _tableModel;
};

#endif // TERMINATOR_H
