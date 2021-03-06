// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// Copyright 2019-2021 Artem Yamshanov, me [at] anticode.ninja

#include "helpers.h"

#include <QApplication>
#include <QWidget>
#include <QGridLayout>
#include "constants.h"

int getRow(QGridLayout *layout, QWidget *widget) {
    int row, column, rowSpan, colSpan;
    layout->getItemPosition(layout->indexOf(widget), &row, &column, &rowSpan, &colSpan);
    return row;
}

void insertRow(QGridLayout *layout, int offset) {
    auto rowCount = layout->rowCount();
    for (auto i = rowCount; i > offset; --i) {
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

void removeRow(QGridLayout *layout, int offset) {
    for (auto j = 0; j < layout->columnCount(); ++j) {
        delete layout->itemAtPosition(offset, j)->widget();
    }

    auto rowCount = layout->rowCount();
    for (auto i = offset; i < rowCount; ++i) {
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

bool splitOnChunks(const QString& source, int& index, QString* part, QString* chunk) {
    if (index == -1 || index > source.length())
        return false;

    int newIndex = source.indexOf('.', index);
    int end = newIndex != -1 ? newIndex : source.count();

    if (part != nullptr)
        *part = source.left(end);
    if (chunk != nullptr)
        *chunk = source.mid(index, end - index);

    index = newIndex != -1 ? newIndex + 1 : -1;

    return true;
}

std::vector<std::tuple<quint64, quint64>> parseRange(const QString &range) {
    std::vector<std::tuple<quint64,quint64>> ranges;;

    for (auto& chunk: range.split(",")) {
        auto delimiter = chunk.indexOf('-');

        if (delimiter == -1)
        {
            auto value = chunk.toULong();
            if (value == 0)
                return ranges;
            ranges.emplace_back(value, value);
        }
        else
        {
            auto start = chunk.leftRef(delimiter).trimmed().size() != 0
                    ? chunk.leftRef(delimiter).toULong()
                    : 1;

            auto end = chunk.rightRef(chunk.length() - delimiter - 1).trimmed().size() != 0
                    ? chunk.rightRef(chunk.length() - delimiter - 1).toULong()
                    : std::numeric_limits<quint64>::max();

            if (start == 0 || end == 0)
                return ranges;

            ranges.emplace_back(start, end);
        }
    }

    std::sort(ranges.begin(), ranges.end());

    auto i = 0;
    while (i < ranges.size() - 1)
    {
        if (std::get<1>(ranges[i]) + 1 >= std::get<0>(ranges[i + 1]))
        {
            ranges[i] = std::make_tuple(std::get<0>(ranges[i]), std::get<1>(ranges[i+1]));
            ranges.erase(ranges.begin() + (i + 1));
            continue;
        }

        i += 1;
    }

    return ranges;
}
