// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// Copyright 2019-2021 Artem Yamshanov, me [at] anticode.ninja

#ifndef MEMORY_STORAGE_H
#define MEMORY_STORAGE_H

#include <memory>
#include <QList>

#include "log_item.h"
#include "chain_elements/chain_element.h"

class QGridLayout;

class MemoryStorage : public ChainElement
{
public:
    MemoryStorage();

    virtual const QString name() const { return "Memory Storage"; }
    virtual ChainElementType type() const { return ChainElementType::Filter; }

    virtual void createUI(QGridLayout* layout);
    virtual void load(const QJsonObject& data) { Q_UNUSED(data); }
    virtual void save(QJsonObject& data) const { Q_UNUSED(data); }

    virtual void accept(std::shared_ptr<LogItem> item);

private:
    void sendClear();

    QList<std::shared_ptr<LogItem>> _rows;
    quint64 _counter;
};

#endif // MEMORY_STORAGE_H
