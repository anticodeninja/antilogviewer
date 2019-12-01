// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// Copyright 2019 Artem Yamshanov, me [at] anticode.ninja

#ifndef TEST_SINK_H
#define TEST_SINK_H

#include "log_item.h"
#include "chain_elements/chain_element.h"

class TestSink : public ChainElement
{
public:
    virtual const QString name() const { return "Test Sink"; }
    virtual ChainElementType type() const { return ChainElementType::Sink; }

    virtual void createUI(QGridLayout* layout) { Q_UNUSED(layout) }
    virtual void load(const QJsonObject& data) { Q_UNUSED(data) }
    virtual void save(QJsonObject& data) const { Q_UNUSED(data) }

    virtual void accept(std::shared_ptr<LogItem> item) {
        _last = item;
    }

    void reset() { _last.reset(); }
    std::shared_ptr<LogItem> last() const { return _last; }

private:
    std::shared_ptr<LogItem> _last;
};

#endif // TEST_SINK_H
