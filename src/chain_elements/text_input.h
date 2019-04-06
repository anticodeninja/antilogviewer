// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// Copyright 2019 Artem Yamshanov, me [at] anticode.ninja

#ifndef TEXT_INPUT_H
#define TEXT_INPUT_H

#include <QtGlobal>
#include <QHash>

#include "log_item.h"
#include "chain_elements/chain_element.h"

class QGridLayout;

class TextInput : public ChainElement
{
public:
    TextInput();

    virtual const char* name() const { return "Text Input"; }
    virtual ChainElementType type() const { return ChainElementType::Source; }

    virtual void createUI(QGridLayout* layout);

private:
    QHash<QString, LogLevel> _levels;
};

#endif // TEXT_INPUT_H
