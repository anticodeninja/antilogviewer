// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// Copyright 2019-2021 Artem Yamshanov, me [at] anticode.ninja

#include "export_dialog.h"

#include <QLabel>
#include <QLineEdit>
#include <QGridLayout>

ExportDialog::ExportDialog(QWidget *parent)
    : QFileDialog(parent)
{
    // https://wiki.qt.io/Technical_FAQ#How_can_I_add_widgets_to_my_QFileDialog_instance.3F
    setOption(QFileDialog::DontUseNativeDialog, true);
    setAcceptMode(QFileDialog::AcceptSave);
    setLabelText(FileType, tr("Export As:"));

    _lblRange = new QLabel(this);
    _lblRange->setText("Range:");

    _ctrRange = new QLineEdit(this);
    _ctrRange->setText("");

    auto *mainLayout = qobject_cast<QGridLayout*>(layout());
    auto rangeRow = mainLayout->rowCount();
    mainLayout->addWidget(_lblRange, rangeRow, 0);
    mainLayout->addWidget(_ctrRange, rangeRow, 1);
}

QString ExportDialog::range() const
{
    return _ctrRange->text();
}
