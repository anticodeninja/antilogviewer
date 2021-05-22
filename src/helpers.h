// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// Copyright 2019-2021 Artem Yamshanov, me [at] anticode.ninja

#ifndef HELPERS_H
#define HELPERS_H

class QString;
class QWidget;
class QGridLayout;

int getRow(QGridLayout* layout, QWidget* widget);
void insertRow(QGridLayout* layout, int offset);
void removeRow(QGridLayout* layout, int offset);

// Helper for splitting source string in chunks, some kind of a "very simple iterator"
bool splitOnChunks(const QString& source, int& index, QString* part, QString* chunk);

#endif // HELPERS_H
