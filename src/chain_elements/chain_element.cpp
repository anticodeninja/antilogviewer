// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// Copyright 2019 Artem Yamshanov, me [at] anticode.ninja

#include "chain_element.h"

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
