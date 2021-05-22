// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// Copyright 2019-2021 Artem Yamshanov, me [at] anticode.ninja

#include "table_view.h"
#include "table_model.h"

#include <QApplication>
#include <QJsonArray>
#include <QJsonObject>
#include <QGridLayout>
#include <QPushButton>

#include "default_settings.h"
#include "table_view_config.h"

TableView::TableView(QTableView* tableView, TableModel *tableModel)
    : _tableView(tableView)
    , _tableModel(tableModel)
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

    if (data["widths"].isArray()) {
        auto widths = data["widths"].toArray();
        for (auto i = 0; i < static_cast<int>(Column::End); ++i) {
            if (i == static_cast<int>(Column::Message))
                continue;

            if (widths.size() > i && widths[i].isDouble())
                _tableView->setColumnWidth(i, widths[i].toInt());
            else
                _tableView->setColumnWidth(i, COLUMN_WIDTHS[i]);
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

    QJsonArray widths;
    for (auto i = 0; i < static_cast<int>(Column::End); ++i)
        widths.append(_tableView->columnWidth(i));
    data["widths"] = widths;
}

void TableView::accept(std::shared_ptr<LogItem> item)
{
    _tableModel->add(item);
    ChainElement::accept(item);
}

void TableView::setDefaultSettings()
{
    setGlobalPalette(
                DARK_TEXT_COLORS[static_cast<int>(LogColor::Window)],
                DARK_BACK_COLORS[static_cast<int>(LogColor::Window)]);

    for (auto i = 0; i < static_cast<int>(Column::End); ++i)
    {
        if (i != static_cast<int>(Column::Message))
            _tableView->setColumnWidth(i, COLUMN_WIDTHS[i]);
    }
}

void TableView::setGlobalPalette(const QColor& text, const QColor& back)
{
    QPalette palette(back);
    palette.setColor(QPalette::Base, back);
    palette.setColor(QPalette::Text, text);
    QApplication::setPalette(palette);
}
