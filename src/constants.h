// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// Copyright 2019 Artem Yamshanov, me [at] anticode.ninja

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QString>
#include <QColor>

enum class LogItemType
{
    Log,
    Clear,
};

enum class ChainElementMode
{
    Pass,
    Discard,
    Mark1,
    Mark2,
    Mark3,
    Mark4,
};

enum class LogLevel
{
    Trace,
    Debug,
    Info,
    Warn,
    Error,
    Fatal,
};

enum class LogColor
{
    Trace,
    Debug,
    Info,
    Warn,
    Error,
    Fatal,
    Window,
    Custom1,
    Custom2,
    Custom3,
    Custom4,
    End,
};

enum class Column
{
    Timestamp,
    Source,
    Message,
};

const QChar NEWLINE_CHAR = QChar(11152);
const QString PROFILES_SECTION = QString("Profiles");
const QString DEFAULT_PROFILE = QString("Default");

#define SOLARIZED_BASE03 0, 43, 54
#define SOLARIZED_BASE02 7, 54, 66
#define SOLARIZED_BASE01 88, 110, 117
#define SOLARIZED_BASE00 101, 123, 131
#define SOLARIZED_BASE0 131, 148, 150
#define SOLARIZED_BASE1 147, 161, 161
#define SOLARIZED_BASE2 238, 232, 213
#define SOLARIZED_BASE3 253, 246, 227
#define SOLARIZED_YELLOW 181, 137, 0
#define SOLARIZED_ORANGE 203, 75, 22
#define SOLARIZED_RED 220, 50, 47
#define SOLARIZED_MAGENTA 211, 54, 130
#define SOLARIZED_VIOLET 108, 113, 196
#define SOLARIZED_BLUE 38, 139, 210
#define SOLARIZED_CYAN 42, 161, 152
#define SOLARIZED_GREEN 133, 153, 0

#endif // CONSTANTS_H
