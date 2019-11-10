// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// Copyright 2019 Artem Yamshanov, me [at] anticode.ninja

#include "memory_storage.h"

#include <QGridLayout>
#include <QPushButton>

#include "log_item.h"

MemoryStorage::MemoryStorage()
{

}

void MemoryStorage::createUI(QGridLayout *layout)
{
    auto ctrRegen = new QPushButton("Regen");
    ctrRegen->connect(ctrRegen, &QPushButton::clicked, [this] {
        sendClear();
        foreach (auto item, _rows) {
            item->Color = static_cast<LogColor>(item->Level);
            ChainElement::accept(item);
        }
    });

    auto ctrClear = new QPushButton("Clear");
    ctrClear->connect(ctrClear, &QPushButton::clicked, [this] {
        _rows.clear();
        sendClear();
    });

    layout->addWidget(ctrRegen, 1, 0, 1, 2);
    layout->addWidget(ctrClear, 2, 0, 1, 2);
}

void MemoryStorage::accept(std::shared_ptr<LogItem> item) {
    if (item->Type == LogItemType::Log) {
        _rows.append(item);
    } else {
        _rows.clear();
    }

    ChainElement::accept(item);
}

void MemoryStorage::sendClear()
{
    auto logItem = std::make_shared<LogItem>();
    logItem->Type = LogItemType::Clear;
    ChainElement::accept(logItem);
}
