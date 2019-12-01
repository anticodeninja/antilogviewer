// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// Copyright 2019 Artem Yamshanov, me [at] anticode.ninja

#include "table_model.h"

#include <iostream>
#include <QDateTime>

#include "constants.h"

TableModel::TableModel(QObject *parent)
    : QAbstractTableModel(parent)
    , _linked(false)
    , _clear(false)
    , _textColors(static_cast<int>(LogColor::End))
    , _backColors(static_cast<int>(LogColor::End))
    , _timeFormat("hh:mm:ss.zzz")
    , _sourceElide(Qt::ElideMiddle)
    , _messageElide(Qt::ElideRight)
{
    startTimer(250);
}

int TableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return _rows.count();
}

int TableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 3;
}

QVariant TableModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case static_cast<int>(Column::Timestamp):
            return QDateTime::fromMSecsSinceEpoch(_rows[index.row()]->Timestamp).toString(_timeFormat);
        case static_cast<int>(Column::Source):
            return _rows[index.row()]->Source;
        case static_cast<int>(Column::Message):
            return _rows[index.row()]->Message;
        }
    } else if (role == Qt::BackgroundColorRole) {
        return _backColors[static_cast<int>(_rows[index.row()]->Color)];
    } else if (role == Qt::TextColorRole) {
        return _textColors[static_cast<int>(_rows[index.row()]->Color)];
    }

    return QVariant();
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        switch (section) {
        case static_cast<int>(Column::Timestamp):
            return QString("Timestamp");
        case static_cast<int>(Column::Source):
            return QString("Source");
        case static_cast<int>(Column::Message):
            return QString("Message");
        default:
            return QVariant();
        }
    }

    return QVariant();
}

void TableModel::add(std::shared_ptr<LogItem> item)
{
    if (item->Type == LogItemType::Log) {
        _queue.append(item);
    } else {
        _queue.clear();
        _clear = true;
    }
}

std::shared_ptr<LogItem> TableModel::get(int index) const
{
    return _rows[index];
}

void TableModel::setTextColor(LogColor level, QColor color)
{
    _textColors[static_cast<int>(level)] = color;
}

void TableModel::setBackColor(LogColor level, QColor color)
{
    _backColors[static_cast<int>(level)] = color;
}

QColor TableModel::textColor(LogColor level) const
{
    return _textColors[static_cast<int>(level)];
}

QColor TableModel::backColor(LogColor level) const
{
    return _backColors[static_cast<int>(level)];
}

QStyleOptionViewItem TableModel::options(const QStyleOptionViewItem& option, int column)
{
    if (column == static_cast<int>(Column::Source)) {
        auto overrided = option;
        overrided.textElideMode = _sourceElide;
        return overrided;
    }

    if (column == static_cast<int>(Column::Message)) {
        auto overrided = option;
        overrided.textElideMode = _messageElide;
        return overrided;
    }

    return option;
}

void TableModel::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event)

    if (_clear) {
        _clear = false;
        if (!_rows.empty()) {
            beginRemoveRows(QModelIndex(), 0, _rows.count() - 1);
            _rows.clear();
            endRemoveRows();
        }
    }

    if (!_queue.empty()) {
        beginInsertRows(QModelIndex(), _rows.count(), _rows.count() + _queue.count() - 1);
        _rows.append(_queue);
        _queue.clear();
        endInsertRows();
    }
}
