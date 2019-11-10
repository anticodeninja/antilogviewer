// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// Copyright 2019 Artem Yamshanov, me [at] anticode.ninja

#ifndef CHAIN_ELEMENT_H
#define CHAIN_ELEMENT_H

#include <memory>
#include <QtDebug>
#include "constants.h"

class LogItem;
class QComboBox;
class QGridLayout;
class QMenu;
class QWidget;

enum class ChainElementType {
    Source,
    Filter,
    Sink,
};

class ChainElement
{
public:
    ChainElement()
        : _next(nullptr)
        , _widget(nullptr)
    {
    }

    virtual ~ChainElement();

    virtual const QString name() const = 0;
    const QString fullname() const;

    virtual ChainElementType type() const = 0;

    virtual void createUI(QGridLayout* layout) = 0;
    virtual void createMenuOnEntry(QMenu* menu, std::shared_ptr<LogItem> item) {
        Q_UNUSED(menu)
        Q_UNUSED(item)
    }
    virtual void createMenuOnSelection(QMenu* menu, const QString& selection) {
        Q_UNUSED(menu)
        Q_UNUSED(selection)
    }

    virtual void load(const QJsonObject& data) = 0;
    virtual void save(QJsonObject& data) const = 0;

    void setNext(ChainElement* element) { _next = element; }
    ChainElement* next() const { return _next; }

    void setSlotId(int slotId) { _slotId = slotId; }
    int slotId() const { return _slotId; }

    void setWidget(QWidget* widget) { _widget = widget; }
    QWidget* widget() const { return _widget; }

    virtual void accept(std::shared_ptr<LogItem> item) {
        if (_next) _next->accept(item);
    }

    static void configureModeComboBox(QComboBox *comboBox, ChainElementMode mode, std::function<void(ChainElementMode)> setter);
    static bool updateElement(bool triggered, ChainElementMode mode, std::shared_ptr<LogItem> item);

private:
    ChainElement* _next;
    int _slotId;
    QWidget* _widget;
};

#endif // CHAIN_ELEMENT_H
