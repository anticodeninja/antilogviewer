// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// Copyright 2019 Artem Yamshanov, me [at] anticode.ninja

#ifndef LEVEL_FILTER_H
#define LEVEL_FILTER_H

#include "log_item.h"
#include "chain_elements/chain_element.h"

class LevelFilter : public ChainElement
{
public:
    LevelFilter();

    virtual const QString name() const { return "Level Filter"; }
    virtual ChainElementType type() const { return ChainElementType::Filter; }

    virtual void createUI(QGridLayout* layout);
    virtual void load(const QJsonObject& data);
    virtual void save(QJsonObject& data) const;

    virtual void accept(std::shared_ptr<LogItem> item);

private:
    bool _straightforward;
    LogLevel _level;
};

#endif // LEVEL_FILTER_H
