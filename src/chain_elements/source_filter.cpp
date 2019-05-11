// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// Copyright 2019 Artem Yamshanov, me [at] anticode.ninja

#include "source_filter.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QGridLayout>
#include <QCheckBox>
#include <QMenu>
#include <QLabel>
#include <QPushButton>

#include "layout_helper.h"

const int DYNAMIC_PRE = 2;
const int DYNAMIC_POST = 1;

SourceFilter::SourceFilter()
    : _straightforward(true)
{
}

void SourceFilter::createUI(QGridLayout *layout)
{
    _menu = new QMenu();
    auto ctrStraightforward = new QCheckBox("Straightforward");
    ctrStraightforward->setChecked(_straightforward);
    auto ctrAdd = new QPushButton("+");
    ctrAdd->setMenu(_menu);

    _layout = layout;
    layout->setColumnStretch(0, 1000);
    layout->addWidget(ctrStraightforward, 1, 0, 1, 2);
    layout->addWidget(ctrAdd, DYNAMIC_PRE, 0, 1, 2);

    foreach (auto source, _sources)
        add(source);

    ctrStraightforward->connect(ctrStraightforward, &QCheckBox::toggled, [this](bool value) {
        _straightforward = value;
    });

    _menu->connect(_menu, &QMenu::triggered, [this](QAction* action){
        auto source = action->data().toString();
        if (!_sources.contains(source)) {
            _sources.append(source);
            add(source);
        }
    });
}

void SourceFilter::load(const QJsonObject &data)
{
    if (data["straightforward"].isBool())
        _straightforward = data["straightforward"].toBool();

    if (data["sources"].isArray()) {
        auto sources = data["sources"].toArray();
        foreach(auto source, sources) {
            if (source.isString())
                _sources.append(source.toString());
        }
    }
}

void SourceFilter::save(QJsonObject &data) const
{
    data["straightforward"] = _straightforward;

    QJsonArray sources;
    foreach(auto source, _sources)
        sources.append(source);
    data["sources"] = sources;
}

void SourceFilter::accept(std::shared_ptr<LogItem> item)
{
    if (item->Type == LogItemType::Log) {
        addNewSource(item->Source);

        if (_sources.empty()) {
            ChainElement::accept(item);
        } else {
            foreach (auto source, _sources) {
                if (item->Source.startsWith(source)) {
                    if (_straightforward)
                        ChainElement::accept(item);
                    return;
                }
            }

            if (!_straightforward)
                ChainElement::accept(item);
        }
    } else if (item->Type == LogItemType::Clear) {
        _allSources.clear();
        _menu->clear();

        foreach (auto source, _allSources) {
            _allSources.insert(source);
            addNewSource(source);
        }

        ChainElement::accept(item);
    }
}

void SourceFilter::add(QString source)
{
    auto ctrLabel = new QLabel(source);
    auto ctrRemove = new QPushButton("-");
    ctrRemove->setFixedWidth(20);

    auto offset = _sources.count() + DYNAMIC_PRE - 1;
    insertRow(_layout, offset, _sources.count() + DYNAMIC_PRE + DYNAMIC_POST);
    _layout->addWidget(ctrLabel, offset, 0);
    _layout->addWidget(ctrRemove, offset, 1);

    ctrRemove->connect(ctrRemove, &QPushButton::clicked, [this, ctrRemove] {
        remove(getRow(_layout, ctrRemove) - DYNAMIC_PRE);
    });
}

void SourceFilter::remove(int index)
{
    _sources.removeAt(index);
    removeRow(_layout, index + DYNAMIC_PRE, _sources.count() + DYNAMIC_PRE + DYNAMIC_POST);
}

void SourceFilter::addNewSource(QString source)
{
    if (_allSources.contains(source))
        return;
    _allSources.insert(source);

    int index = 0;
    QMenu* menu = _menu;
    for (;;) {
        int pos = source.indexOf('.', index);
        int last = pos == -1;
        int end = last ? source.count() : pos;
        auto chunk = source.mid(index, end - index);

        auto max = menu->actions().count() - 1;
        QMenu* inner = nullptr;
        QAction* before = nullptr;

        // Binary search... and bit of magic
        auto l = 0;
        if (l <= max && menu->actions()[l]->text() == "this")
            l = 1;
        auto r = max;

        while (l <= r) {
            auto m = (l + r) / 2;
            auto node = menu->actions()[m];

            if (node->text() == chunk) {
                inner = node->menu();
                break;
            }

            auto f = chunk > node->text();
            if (l == r) {
                if (!f && m <= max)
                    before = menu->actions()[m];
                else if (m < max)
                    before = menu->actions()[m + 1];
                break;
            }

            if (f)
                l = m + 1;
            else
                r = m - 1;
        }

        if (inner != nullptr) {
            menu = inner;
        } else {
            auto newMenu = new QMenu(chunk, _menu);
            auto action = newMenu->addAction("this");
            action->setData(source.left(end));
            menu->insertMenu(before, newMenu);
            menu = newMenu;
        }

        if (last)
            break;
        index = pos + 1;
    }
}
