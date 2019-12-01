// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// Copyright 2019 Artem Yamshanov, me [at] anticode.ninja

#ifndef TABLE_VIEW_HACKS_H
#define TABLE_VIEW_HACKS_H

#include <QStyledItemDelegate>

class TableModel;

class TableViewColumnStyledItem : public QStyledItemDelegate
{
public:
    TableViewColumnStyledItem(TableModel* model);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    TableModel* _model;
};

#endif // TABLE_VIEW_HACKS_H
