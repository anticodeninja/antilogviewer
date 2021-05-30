// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// Copyright 2019-2021 Artem Yamshanov, me [at] anticode.ninja

#ifndef EXPORT_DIALOG_H
#define EXPORT_DIALOG_H

#include <QFileDialog>
#include <QLabel>
#include <QLineEdit>

class ExportDialog : public QFileDialog
{
    Q_OBJECT

public:
    explicit ExportDialog(QWidget *parent);

    QString range() const;

private:
    QLabel *_lblRange;
    QLineEdit *_ctrRange;

};

#endif //EXPORT_DIALOG_H
