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
    , _textColors({
        // TODO Unhardcode
        QColor(101, 123, 131),
        QColor(147, 161, 161),
        QColor(133, 153, 0),
        QColor(181, 137, 0),
        QColor(203, 75 ,22),
        QColor(211, 55, 130),
    })
    , _backColors({
        // TODO Unhardcode
        QColor(7, 54, 66),
        QColor(7, 54, 66),
        QColor(7, 54, 66),
        QColor(7, 54, 66),
        QColor(7, 54, 66),
        QColor(7, 54, 66),
    })
    , _linked(false)
    , _clear(false)
{
    startTimer(250);
}

int TableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return _rows.count();
}

int TableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 3;
}

QVariant TableModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case 0:
            return QDateTime::fromMSecsSinceEpoch(_rows[index.row()]->Timestamp).toString("hh:mm:ss.zzz");
        case 1:
            return _rows[index.row()]->Source;
        case 2:
            return _rows[index.row()]->Message;
        }
    } else if (role == Qt::BackgroundColorRole) {
        return _backColors[static_cast<int>(_rows[index.row()]->Level)];
    } else if (role == Qt::TextColorRole) {
        return _textColors[static_cast<int>(_rows[index.row()]->Level)];
    }

    return QVariant();
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        switch (section) {
        case 0:
            return QString("Timestamp");
        case 1:
            return QString("Source");
        case 2:
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

void TableModel::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);

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
