// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// Copyright 2019 Artem Yamshanov, me [at] anticode.ninja

#include "table_view.h"
#include "table_model.h"

#include <QApplication>
#include <QJsonArray>
#include <QJsonObject>
#include <QGridLayout>
#include <QPushButton>

#include "default_palettes.h"
#include "helpers.h"
#include "table_view_config.h"

TableView::TableView(TableModel *tableModel)
    : _tableModel(tableModel)
{
    _tableModel->setLinked(true);

    for (auto i = 0; i < static_cast<int>(LogColor::End); ++i) {
        auto color = static_cast<LogColor>(i);
        _tableModel->setTextColor(color, DARK_TEXT_COLORS[i]);
        _tableModel->setBackColor(color, DARK_BACK_COLORS[i]);
    }

    auto palette = QApplication::palette();
    _tableModel->setTextColor(LogColor::Window, palette.highlight().color());
    _tableModel->setBackColor(LogColor::Window, palette.base().color());
}

TableView::~TableView()
{
    _tableModel->setLinked(false);
}

void TableView::createUI(QGridLayout *layout) {
    auto ctrConfigure = new QPushButton("Configure");
    ctrConfigure->connect(ctrConfigure, &QPushButton::clicked, [this, ctrConfigure] {
        auto config = new TableViewConfig(ctrConfigure);

        config->setTimeFormat(_tableModel->timeFormat());
        config->setSourceElide(_tableModel->sourceElide());
        config->setMessageElide(_tableModel->messageElide());
        for (auto i = 0; i < static_cast<int>(LogColor::End); ++i) {
            auto color = static_cast<LogColor>(i);
            config->setTextColor(color, _tableModel->textColor(color));
            config->setBackColor(color, _tableModel->backColor(color));
        }
        config->calcPalette();

        if (config->exec() != QDialog::Accepted)
            return;

        _tableModel->setTimeFormat(config->timeFormat());
        _tableModel->setSourceElide(config->sourceElide());
        _tableModel->setMessageElide(config->messageElide());
        for (auto i = 0; i < static_cast<int>(LogColor::End); ++i) {
            auto color = static_cast<LogColor>(i);
            _tableModel->setTextColor(color, config->textColor(color));
            _tableModel->setBackColor(color, config->backColor(color));
        }
        setGlobalPalette(_tableModel->textColor(LogColor::Window), _tableModel->backColor(LogColor::Window));
    });

    auto ctrClear = new QPushButton("Clear");
    ctrClear->connect(ctrClear, &QPushButton::clicked, [this] {
        auto logItem = std::make_shared<LogItem>();
        logItem->Type = LogItemType::Clear;
        _tableModel->add(logItem);
    });

    layout->addWidget(ctrConfigure, 2, 0, 1, 2);
    layout->addWidget(ctrClear, 3, 0, 1, 2);
}

void TableView::load(const QJsonObject &data)
{
    if (data["timeFormat"].isString())
        _tableModel->setTimeFormat(data["timeFormat"].toString());
    if (data["sourceElide"].isDouble())
        _tableModel->setSourceElide(static_cast<Qt::TextElideMode>(data["sourceElide"].toInt()));
    if (data["messageElide"].isDouble())
        _tableModel->setMessageElide(static_cast<Qt::TextElideMode>(data["messageElide"].toInt()));

    if (data["textColors"].isArray()) {
        auto textColors = data["textColors"].toArray();
        for (auto i = 0; i < static_cast<int>(LogColor::End); ++i) {
            if (textColors.size() > i && textColors[i].isString())
                _tableModel->setTextColor(static_cast<LogColor>(i), QColor(textColors[i].toString()));
            else
                _tableModel->setTextColor(static_cast<LogColor>(i), DARK_TEXT_COLORS[i]);
        }
    }

    if (data["backColors"].isArray()) {
        auto backColors = data["backColors"].toArray();
        for (auto i = 0; i < static_cast<int>(LogColor::End); ++i) {
            if (backColors.size() > i && backColors[i].isString())
                _tableModel->setBackColor(static_cast<LogColor>(i), QColor(backColors[i].toString()));
            else
                _tableModel->setBackColor(static_cast<LogColor>(i), DARK_BACK_COLORS[i]);
        }
    }

    setGlobalPalette(_tableModel->textColor(LogColor::Window), _tableModel->backColor(LogColor::Window));
}

void TableView::save(QJsonObject &data) const
{
    data["timeFormat"] = _tableModel->timeFormat();
    data["sourceElide"] = _tableModel->sourceElide();
    data["messageElide"] = _tableModel->messageElide();

    QJsonArray textColors, backColors;
    for (auto i = 0; i < static_cast<int>(LogColor::End); ++i) {
        textColors.append(_tableModel->textColor(static_cast<LogColor>(i)).name());
        backColors.append(_tableModel->backColor(static_cast<LogColor>(i)).name());
    }
    data["textColors"] = textColors;
    data["backColors"] = backColors;
}

void TableView::accept(std::shared_ptr<LogItem> item)
{
    _tableModel->add(item);
    ChainElement::accept(item);
}

void TableView::setGlobalPalette(QColor text, QColor back)
{
    QPalette palette(back);
    palette.setColor(QPalette::Base, back);
    palette.setColor(QPalette::Highlight, text);
    QApplication::setPalette(palette);
}

void TableView::setDefaultPalette()
{
    setGlobalPalette(
                DARK_TEXT_COLORS[static_cast<int>(LogColor::Window)],
                DARK_BACK_COLORS[static_cast<int>(LogColor::Window)]);
}
