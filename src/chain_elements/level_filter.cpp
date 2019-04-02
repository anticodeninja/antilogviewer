// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// Copyright 2019 Artem Yamshanov, me [at] anticode.ninja

#include "level_filter.h"

#include <QCheckBox>
#include <QGridLayout>
#include <QComboBox>

LevelFilter::LevelFilter()
    : _straightforward(true)
    , _level(LogLevel::TRACE)
{
}

void LevelFilter::createUI(QGridLayout* layout)
{
    auto ctrStraightforward = new QCheckBox("Straightforward");
    ctrStraightforward->setChecked(_straightforward);
    auto ctrLevel = new QComboBox();
    ctrLevel->addItems(QStringList({"Trace", "Debug","Info", "Warn", "Error", "Fatal"}));
    ctrLevel->setCurrentIndex(static_cast<int>(_level));

    layout->addWidget(ctrStraightforward, 1, 0, 1, 2);
    layout->addWidget(ctrLevel, 2, 0, 1, 2);

    ctrStraightforward->connect(ctrStraightforward, &QCheckBox::toggled, [this](bool value) {
        _straightforward = value;
    });

    ctrLevel->connect(ctrLevel, QOverload<int>::of(&QComboBox::currentIndexChanged), [this] (int index) {
        _level = static_cast<LogLevel>(index);
    });
}

void LevelFilter::accept(std::shared_ptr<LogItem> item)
{
    if (item->Type == LogItemType::Log) {
        if (_straightforward ? item->Level >= _level : _level >= item->Level) {
            ChainElement::accept(item);
        }
    } else if (item->Type == LogItemType::Clear) {
        ChainElement::accept(item);
    }
}
