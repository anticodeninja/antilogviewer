#include "table_model.h"

#include <iostream>
#include <QDateTime>

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
{
}

int TableModel::rowCount(const QModelIndex &parent) const
{
    return _rows.count();
}

int TableModel::columnCount(const QModelIndex &parent) const
{
    return 3;
}

QVariant TableModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case 0:
            return _rows[index.row()].Timestamp;
        case 1:
            return _rows[index.row()].Source;
        case 2:
            return _rows[index.row()].Message;
        default:
            return QVariant();
        }
    } else if (role == Qt::BackgroundColorRole) {
        return _backColors[(int)_rows[index.row()].Level];
    } else if (role == Qt::TextColorRole) {
        return _textColors[(int)_rows[index.row()].Level];
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
            break;
        case 1:
            return QString("Source");
            break;
        case 2:
            return QString("Message");
            break;
        default:
            return QVariant();
        }
    }

    return QVariant();
}

void TableModel::add(const LogItem &item)
{
    if (item.Type == LogItemType::Log) {
        beginInsertRows(QModelIndex(), _rows.count(), _rows.count());

        LogViewItem logEntry;
        logEntry.Level = item.Level;
        logEntry.Source = item.Source;
        logEntry.Message = item.Message;
        logEntry.Timestamp = QDateTime::fromMSecsSinceEpoch(item.Timestamp).toString("hh:mm:ss.zzz");
        _rows.append(logEntry);

        endInsertRows();
    } else {
        beginRemoveRows(QModelIndex(), 0, _rows.count() - 1);
        _rows.clear();
        endRemoveRows();
    }
}
