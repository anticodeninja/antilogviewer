// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// Copyright 2019 Artem Yamshanov, me [at] anticode.ninja

#ifndef TABLE_VIEW_CONFIG_H
#define TABLE_VIEW_CONFIG_H

#include <QDialog>

#include "constants.h"

class QComboBox;
class QPushButton;

class TableViewConfig : public QDialog
{
    Q_OBJECT
public:
    explicit TableViewConfig(QWidget *parent = nullptr);

    void calcPalette();
    void setTextColor(LogColor level, QColor color);
    void setBackColor(LogColor level, QColor color);
    QColor getTextColor(LogColor level) const;
    QColor getBackColor(LogColor level) const;

signals:

public slots:

private:
    QComboBox* _ctrPalettes;
    QVector<QPushButton*> _ctrTextColors;
    QVector<QPushButton*> _ctrBackColors;
};

#endif // TABLE_VIEW_CONFIG_H
