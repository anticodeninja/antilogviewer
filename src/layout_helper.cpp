// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// Copyright 2019 Artem Yamshanov, me [at] anticode.ninja

#include "layout_helper.h"

#include <QWidget>
#include <QGridLayout>

int getRow(QGridLayout *layout, QWidget *widget) {
    int row, column, rowSpan, colSpan;
    layout->getItemPosition(layout->indexOf(widget), &row, &column, &rowSpan, &colSpan);
    return row;
}

void insertRow(QGridLayout *layout, int offset, int rows) {
    for (auto i = rows - 1; i > offset; --i) {
        for (auto j = 0; j < layout->columnCount(); ++j) {
            auto item = layout->itemAtPosition(i - 1, j);
            if (item == nullptr)
                continue;

            int row, column, rowSpan, colSpan;
            layout->getItemPosition(layout->indexOf(item->widget()), &row, &column, &rowSpan, &colSpan);
            layout->addWidget(item->widget(), i, j, rowSpan, colSpan);
        }
    }
}

void removeRow(QGridLayout *layout, int offset, int rows) {
    for (auto j = 0; j < layout->columnCount(); ++j) {
        delete layout->itemAtPosition(offset, j)->widget();
    }

    for (auto i = offset; i < rows; ++i) {
        for (auto j = 0; j < layout->columnCount(); ++j) {
            auto item = layout->itemAtPosition(i + 1, j);
            if (item == nullptr)
                continue;

            int row, column, rowSpan, colSpan;
            layout->getItemPosition(layout->indexOf(item->widget()), &row, &column, &rowSpan, &colSpan);
            layout->addWidget(item->widget(), i, j, rowSpan, colSpan);
        }
    }
}
