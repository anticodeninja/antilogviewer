// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// Copyright 2019-2021 Artem Yamshanov, me [at] anticode.ninja

#include "table_view_hacks.h"

#include "table_model.h"

TableViewColumnStyledItem::TableViewColumnStyledItem(TableModel* model)
    : _model(model)
{
}

void TableViewColumnStyledItem::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QStyledItemDelegate::paint(painter, _model->options(option, index.column()), index);
}
