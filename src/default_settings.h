// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// Copyright 2019-2021 Artem Yamshanov, me [at] anticode.ninja

#ifndef DEFAULT_PALETTES_H
#define DEFAULT_PALETTES_H

#include <QColor>
#include <QString>

#include "constants.h"

const QColor DARK_TEXT_COLORS[] = {
    QColor(SOLARIZED_BASE01),
    QColor(SOLARIZED_BASE1),
    QColor(SOLARIZED_GREEN),
    QColor(SOLARIZED_YELLOW),
    QColor(SOLARIZED_RED),
    QColor(SOLARIZED_BASE03),
    QColor(SOLARIZED_BASE1),
    QColor(SOLARIZED_BASE03),
    QColor(SOLARIZED_BASE03),
    QColor(SOLARIZED_BASE03),
    QColor(SOLARIZED_BASE03),
};

const QColor DARK_BACK_COLORS[] = {
    QColor(SOLARIZED_BASE03),
    QColor(SOLARIZED_BASE03),
    QColor(SOLARIZED_BASE03),
    QColor(SOLARIZED_BASE03),
    QColor(SOLARIZED_BASE03),
    QColor(SOLARIZED_RED),
    QColor(SOLARIZED_BASE03),
    QColor(SOLARIZED_MAGENTA),
    QColor(SOLARIZED_VIOLET),
    QColor(SOLARIZED_BLUE),
    QColor(SOLARIZED_CYAN),
};

const QColor LIGHT_TEXT_COLORS[] = {
    QColor(SOLARIZED_BASE01),
    QColor(SOLARIZED_BASE02),
    QColor(SOLARIZED_GREEN),
    QColor(SOLARIZED_YELLOW),
    QColor(SOLARIZED_RED),
    QColor(SOLARIZED_BASE3),
    QColor(SOLARIZED_BASE03),
    QColor(SOLARIZED_BASE3),
    QColor(SOLARIZED_BASE3),
    QColor(SOLARIZED_BASE3),
    QColor(SOLARIZED_BASE3),
};

const QColor LIGHT_BACK_COLORS[] = {
    QColor(SOLARIZED_BASE3),
    QColor(SOLARIZED_BASE3),
    QColor(SOLARIZED_BASE3),
    QColor(SOLARIZED_BASE3),
    QColor(SOLARIZED_BASE3),
    QColor(SOLARIZED_RED),
    QColor(SOLARIZED_BASE3),
    QColor(SOLARIZED_MAGENTA),
    QColor(SOLARIZED_VIOLET),
    QColor(SOLARIZED_BLUE),
    QColor(SOLARIZED_CYAN),
};

const int COLUMN_WIDTHS[] = {
    35,
    100,
    100,
    0
};

#endif // DEFAULT_PALETTES_H
