// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// Copyright 2019 Artem Yamshanov, me [at] anticode.ninja

#include "level_filter.h"

#include <QJsonObject>
#include <QGridLayout>
#include <QComboBox>

LevelFilter::LevelFilter()
    : _mode(ChainElementMode::Pass)
    , _level(LogLevel::Trace)
{
}

void LevelFilter::createUI(QGridLayout* layout)
{
    auto ctrMode = new QComboBox();
    configureModeComboBox(ctrMode, _mode, [this](ChainElementMode mode) { _mode = mode; });
    auto ctrLevel = new QComboBox();
    ctrLevel->addItems(QStringList({"Trace", "Debug","Info", "Warn", "Error", "Fatal"}));
    ctrLevel->setCurrentIndex(static_cast<int>(_level));

    layout->addWidget(ctrMode, 1, 0, 1, 2);
    layout->addWidget(ctrLevel, 2, 0, 1, 2);

    ctrLevel->connect(ctrLevel, QOverload<int>::of(&QComboBox::currentIndexChanged), [this] (int index) {
        _level = static_cast<LogLevel>(index);
    });
}

void LevelFilter::load(const QJsonObject &data)
{
    if (data["mode"].isDouble())
        _mode = static_cast<ChainElementMode>(data["mode"].toInt());
    if (data["level"].isDouble())
        _level = static_cast<LogLevel>(data["level"].toInt());
}

void LevelFilter::save(QJsonObject &data) const
{
    data["mode"] = static_cast<int>(_mode);
    data["level"] = static_cast<int>(_level);
}

void LevelFilter::accept(std::shared_ptr<LogItem> item)
{
    if (item->Type == LogItemType::Clear) {
        ChainElement::accept(item);
        return;
    }

    if (updateElement(check(item), _mode, item)) {
        ChainElement::accept(item);
        return;
    }
}

bool LevelFilter::check(std::shared_ptr<LogItem> item)
{
    return item->Level >= _level;
}
