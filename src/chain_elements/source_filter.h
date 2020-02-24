// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// Copyright 2019 Artem Yamshanov, me [at] anticode.ninja

#ifndef SOURCE_FILTER_H
#define SOURCE_FILTER_H

#include <QSet>
#include <QList>

#include "log_item.h"
#include "chain_element.h"

class QMenu;

class SourceFilter : public ChainElement
{
public:
    SourceFilter();

    virtual const QString name() const { return "Source Filter"; }
    virtual ChainElementType type() const { return ChainElementType::Filter; }

    virtual void createUI(QGridLayout* layout);
    virtual void createMenuOnEntry(QMenu* menu, std::shared_ptr<LogItem> item);

    virtual void load(const QJsonObject& data);
    virtual void save(QJsonObject& data) const;

    virtual void accept(std::shared_ptr<LogItem> item);

    QSet<QString> allSources() { return _allSources; }
    QList<QString> sources() { return _sources; }

private:
    bool check(std::shared_ptr<LogItem> item);
    void addItemAndRow(QString source);
    void addItemAndRowToAllSources(QString source);
    void addRowToAllSources(QString source);
    void addRow(QString source, int index);
    void removeItemAndRow(int index);

    QGridLayout* _layout;
    QMenu* _menu;
    ChainElementMode _mode;
    QSet<QString> _allSources;
    // TODO replace _sources with set
    QList<QString> _sources;
};

#endif // SOURCE_FILTER_H
