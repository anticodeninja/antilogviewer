#ifndef LOGMODEL_H
#define LOGMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include <QHash>
#include <QColor>

#include "log_item.h"

class LogViewItem
{
public:
    LogLevel Level;
    QString Timestamp;
    QString Source; // TODO Use ref here
    QString Message; // TODO Use ref here
};

class TableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    TableModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    void add(const LogItem& item);

private:
    QList<LogViewItem> _rows;
    QVector<QColor> _textColors;
    QVector<QColor> _backColors;
};

#endif // LOGMODEL_H
