// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// Copyright 2019 Artem Yamshanov, me [at] anticode.ninja

#include "keyword_filter.h"

#include <QJsonObject>
#include <QJsonArray>

#include <QGridLayout>
#include <QComboBox>
#include <QLineEdit>
#include <QLabel>
#include <QMenu>
#include <QPushButton>

#include "helpers.h"

const int DYNAMIC_PRE = 2;
const int DYNAMIC_POST = 1;

KeywordFilter::KeywordFilter()
    : _mode(ChainElementMode::Pass)
{
}

void KeywordFilter::createUI(QGridLayout* layout)
{
    auto ctrMode = new QComboBox();
    configureModeComboBox(ctrMode, _mode, [this](ChainElementMode mode) { _mode = mode; });
    auto ctrInput = new QLineEdit();
    auto ctrAdd = new QPushButton("+");
    ctrAdd->setFixedWidth(20);

    _layout = layout;
    layout->addWidget(ctrMode, 1, 0, 1, 2);
    layout->addWidget(ctrInput, DYNAMIC_PRE, 0);
    layout->addWidget(ctrAdd, DYNAMIC_PRE, 1);

    foreach (auto keyword, _keywords)
        addUi(keyword);

    ctrAdd->connect(ctrAdd, &QPushButton::clicked, [this, ctrInput] {
        addItem(ctrInput->text());
        ctrInput->clear();
    });
}

void KeywordFilter::createMenuOnSelection(QMenu *menu, const QString &selection)
{
    auto index = _keywords.indexOf(selection);
    if (index == -1) {
        menu->addAction(
                    QString("Add to %0").arg(fullname()),
                    [this, selection](){
            addItem(selection);
        });
    } else {
        menu->addAction(
                    QString("Remove from %0").arg(fullname()),
                    [this, index](){
            remove(index);
        });
    }
}

void KeywordFilter::load(const QJsonObject &data)
{
    if (data["mode"].isDouble())
        _mode = static_cast<ChainElementMode>(data["mode"].toInt());

    if (data["keywords"].isArray()) {
        auto keywords = data["keywords"].toArray();
        foreach(auto keyword, keywords) {
            if (keyword.isString())
                _keywords.append(keyword.toString());
        }
    }
}

void KeywordFilter::save(QJsonObject &data) const
{
    data["mode"] = static_cast<int>(_mode);

    QJsonArray keywords;
    foreach(auto keyword, _keywords)
        keywords.append(keyword);
    data["keywords"] = keywords;
}

void KeywordFilter::accept(std::shared_ptr<LogItem> item)
{
    if ((item->Type == LogItemType::Log && _keywords.empty()) || item->Type == LogItemType::Clear) {
        ChainElement::accept(item);
        return;
    }

    if (updateElement(check(item), _mode, item)) {
        ChainElement::accept(item);
        return;
    }
}

bool KeywordFilter::check(std::shared_ptr<LogItem> item)
{
    foreach (auto keyword, _keywords) {
        if (item->Message.contains(keyword)) {
            return true;
        }
    }
    return false;
}

void KeywordFilter::addItem(const QString &keyword)
{
    if (keyword.count() != 0 && !_keywords.contains(keyword)) {
        _keywords.append(keyword);
        addUi(keyword);
    }
}

void KeywordFilter::addUi(const QString& keyword)
{
    auto ctrLabel = new QLabel(keyword);
    auto ctrRemove = new QPushButton("-");
    ctrRemove->setFixedWidth(20);

    auto offset = _keywords.count() + DYNAMIC_PRE - 1;
    insertRow(_layout, offset, _keywords.count() + DYNAMIC_PRE + DYNAMIC_POST);
    _layout->addWidget(ctrLabel, offset, 0);
    _layout->addWidget(ctrRemove, offset, 1);

    ctrRemove->connect(ctrRemove, &QPushButton::clicked, [this, ctrRemove] {
        remove(getRow(_layout, ctrRemove) - DYNAMIC_PRE);
    });
}

void KeywordFilter::remove(int index)
{
    _keywords.removeAt(index);
    removeRow(_layout, index + DYNAMIC_PRE, _keywords.count() + DYNAMIC_PRE + DYNAMIC_POST);
}
