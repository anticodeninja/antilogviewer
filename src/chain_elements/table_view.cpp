// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// Copyright 2019 Artem Yamshanov, me [at] anticode.ninja

#include "table_view.h"
#include "table_model.h"

#include <QGridLayout>
#include <QPushButton>

TableView::TableView(TableModel *tableModel)
    : _tableModel(tableModel)
{
    _tableModel->setLinked(true);
}

TableView::~TableView()
{
    _tableModel->setLinked(false);
}

void TableView::createUI(QGridLayout *layout) {
    auto ctrClear = new QPushButton("Clear");
    ctrClear->connect(ctrClear, &QPushButton::clicked, [this] {
        auto logItem = std::make_shared<LogItem>();
        logItem->Type = LogItemType::Clear;
        _tableModel->add(logItem);
    });

    layout->addWidget(ctrClear, 2, 0, 1, 2);
}

void TableView::accept(std::shared_ptr<LogItem> item)
{
    _tableModel->add(item);
}
