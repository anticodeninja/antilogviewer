// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// Copyright 2019-2021 Artem Yamshanov, me [at] anticode.ninja

#include "chain_element.h"
#include "log_item.h"

#include <QComboBox>
#include <QWidget>

ChainElement::~ChainElement() {
    _next = nullptr;
    delete _widget;
}

const QString ChainElement::fullname() const
{
    return _slotId < 2
            ? name()
            : QString("%0:%1").arg(name()).arg(_slotId);
}

void ChainElement::configureModeComboBox(
        QComboBox *comboBox,
        ChainElementMode mode,
        std::function<void(ChainElementMode)> setter)
{
    comboBox->addItems({ "Pass", "Discard", "Mark 1", "Mark 2", "Mark 3", "Mark 4" });
    comboBox->setCurrentIndex(static_cast<int>(mode));
    comboBox->connect(comboBox, qOverload<int>(&QComboBox::currentIndexChanged), [setter](int value) {
        setter(static_cast<ChainElementMode>(value));
    });
}

bool ChainElement::updateElement(bool triggered, ChainElementMode mode, std::shared_ptr<LogItem> item)
{
    if (mode == ChainElementMode::Pass)
        return triggered;

    if (mode == ChainElementMode::Discard)
        return !triggered;

    if (mode >= ChainElementMode::Mark1 && mode <= ChainElementMode::Mark4 && triggered) {
        item->Color = static_cast<LogColor>(static_cast<int>(LogColor::Custom1) + (static_cast<int>(mode) - static_cast<int>(ChainElementMode::Mark1)));
    }

    return true;
}
