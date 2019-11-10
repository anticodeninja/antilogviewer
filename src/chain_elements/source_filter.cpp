// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// Copyright 2019 Artem Yamshanov, me [at] anticode.ninja

#include "source_filter.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QGridLayout>
#include <QComboBox>
#include <QMenu>
#include <QLabel>
#include <QPushButton>

#include "helpers.h"

const int DYNAMIC_PRE = 2;
const int DYNAMIC_POST = 1;

SourceFilter::SourceFilter()
    : _mode(ChainElementMode::Pass)
{
}

void SourceFilter::createUI(QGridLayout *layout)
{
    _menu = new QMenu();
    auto ctrMode = new QComboBox();
    configureModeComboBox(ctrMode, _mode, [this](ChainElementMode mode) { _mode = mode; });
    auto ctrAdd = new QPushButton("+");
    ctrAdd->setMenu(_menu);

    _layout = layout;
    layout->setColumnStretch(0, 1000);
    layout->addWidget(ctrMode, 1, 0, 1, 2);
    layout->addWidget(ctrAdd, DYNAMIC_PRE, 0, 1, 2);

    foreach (auto source, _sources)
        addUi(source);

    _menu->connect(_menu, &QMenu::triggered, [this](QAction* action){
        addItem(action->data().toString());
    });
}

void SourceFilter::createMenuOnEntry(QMenu *menu, std::shared_ptr<LogItem> item)
{
    int index = 0;
    QString part;
    while (splitOnChunks(item->Source, index, &part, nullptr)) {
        auto index = _sources.indexOf(part);
        if (index == -1) {
            menu->addAction(
                        QString("Add \"%0\" to %1").arg(part).arg(fullname()),
                        [this, part](){
                addItem(part);
            });
        } else {
            menu->addAction(
                        QString("Remove \"%0\" from %1").arg(part).arg(fullname()),
                        [this, index](){
                remove(index);
            });
        }
    }
}

void SourceFilter::load(const QJsonObject &data)
{
    if (data["mode"].isDouble())
        _mode = static_cast<ChainElementMode>(data["mode"].toInt());

    if (data["sources"].isArray()) {
        auto sources = data["sources"].toArray();
        foreach (auto source, sources) {
            if (source.isString())
                _sources.append(source.toString());
        }
    }
}

void SourceFilter::save(QJsonObject &data) const
{
    data["mode"] = static_cast<int>(_mode);

    QJsonArray sources;
    foreach(auto source, _sources)
        sources.append(source);
    data["sources"] = sources;
}

void SourceFilter::accept(std::shared_ptr<LogItem> item)
{
    if (item->Type == LogItemType::Log && _sources.empty()) {
        ChainElement::accept(item);
        return;
    }

    if (item->Type == LogItemType::Clear) {
        _allSources.clear();
        _menu->clear();

        foreach (auto source, _allSources) {
            _allSources.insert(source);
            addItemToAllSources(source);
        }

        ChainElement::accept(item);
        return;
    }

    if (updateElement(check(item), _mode, item)) {
        ChainElement::accept(item);
        return;
    }
}

bool SourceFilter::check(std::shared_ptr<LogItem> item)
{
    foreach (auto source, _sources) {
        if (item->Source.startsWith(source)) {
            return true;
        }
    }
    return false;
}

void SourceFilter::addItem(QString source)
{
    if (_sources.contains(source))
        return;

    _sources.append(source);
    addUi(source);
}

void SourceFilter::addItemToAllSources(QString source)
{
    if (_allSources.contains(source))
        return;
    _allSources.insert(source);

    QMenu* menu = _menu;
    int index = 0;
    QString part;
    QString chunk;
    while (splitOnChunks(source, index, &part, &chunk)) {
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
            action->setData(part);
            menu->insertMenu(before, newMenu);
            menu = newMenu;
        }
    }
}

void SourceFilter::addUi(QString source)
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
