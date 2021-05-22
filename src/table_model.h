// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// Copyright 2019-2021 Artem Yamshanov, me [at] anticode.ninja

#ifndef LOGMODEL_H
#define LOGMODEL_H

#include <memory>

#include <QAbstractTableModel>
#include <QStyleOptionViewItem>

#include <QList>
#include <QHash>
#include <QColor>

#include "log_item.h"

class TableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    TableModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void add(std::shared_ptr<LogItem> item);
    std::shared_ptr<LogItem> get(int index) const;
    void clear();

    void setTextColor(LogColor level, QColor color);
    void setBackColor(LogColor level, QColor color);
    void setTimeFormat(const QString& value) { _timeFormat = value; }
    void setSourceElide(Qt::TextElideMode value) { _sourceElide = value; }
    void setMessageElide(Qt::TextElideMode value) { _messageElide = value; }

    QColor textColor(LogColor level) const;
    QColor backColor(LogColor level) const;
    QString timeFormat() const { return _timeFormat; }
    Qt::TextElideMode sourceElide() const { return _sourceElide; }
    Qt::TextElideMode messageElide() const { return _messageElide; }

    QStyleOptionViewItem options(const QStyleOptionViewItem& option, int column);

    void setLinked(bool state) { _linked = state; }
    bool linked() const { return _linked; }

private:
    void timerEvent(QTimerEvent *event) override;

    bool _linked;

    bool _clear;
    QList<std::shared_ptr<LogItem>> _queue;
    QList<std::shared_ptr<LogItem>> _rows;

    QVector<QColor> _textColors;
    QVector<QColor> _backColors;
    QString _timeFormat;
    Qt::TextElideMode _sourceElide;
    Qt::TextElideMode _messageElide;
};

#endif // LOGMODEL_H
