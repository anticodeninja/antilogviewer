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

    for (auto i = 0; i < _keywords.length(); ++i)
        addRow(_keywords[i], i);

    ctrAdd->connect(ctrAdd, &QPushButton::clicked, [this, ctrInput] {
        addItemAndRow(ctrInput->text());
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
            addItemAndRow(selection);
        });
    } else {
        menu->addAction(
                    QString("Remove from %0").arg(fullname()),
                    [this, index](){
            removeItemAndRow(index);
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
    switch (item->Type) {

    case LogItemType::Log:
        if (_keywords.empty() || updateElement(check(item), _mode, item))
            ChainElement::accept(item);
        break;

    case LogItemType::Clear:
        ChainElement::accept(item);
        break;

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

void KeywordFilter::addItemAndRow(const QString &keyword)
{
    if (keyword.count() != 0 && !_keywords.contains(keyword)) {
        _keywords.append(keyword);
        addRow(keyword, _keywords.count() - 1);
    }
}

void KeywordFilter::addRow(const QString& keyword, int index)
{
    auto ctrLabel = new QLabel(keyword);
    ctrLabel->setSizePolicy(QSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored));
    auto ctrRemove = new QPushButton("-");
    ctrRemove->setFixedWidth(20);

    auto offset = DYNAMIC_PRE + index;
    insertRow(_layout, offset);
    _layout->addWidget(ctrLabel, offset, 0);
    _layout->addWidget(ctrRemove, offset, 1);

    ctrRemove->connect(ctrRemove, &QPushButton::clicked, [this, ctrRemove] {
        removeItemAndRow(getRow(_layout, ctrRemove) - DYNAMIC_PRE);
    });
}

void KeywordFilter::removeItemAndRow(int index)
{
    _keywords.removeAt(index);
    removeRow(_layout, DYNAMIC_PRE + index);
}
