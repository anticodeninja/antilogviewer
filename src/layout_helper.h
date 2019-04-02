// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// Copyright 2019 Artem Yamshanov, me [at] anticode.ninja

#ifndef LAYOUT_HELPER_H
#define LAYOUT_HELPER_H

class QWidget;
class QGridLayout;

int getRow(QGridLayout* layout, QWidget* widget);
void insertRow(QGridLayout* layout, int offset, int rows);
void removeRow(QGridLayout* layout, int offset, int rows);

#endif // LAYOUT_HELPER_H
