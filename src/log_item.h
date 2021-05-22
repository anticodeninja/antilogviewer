// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// Copyright 2019 Artem Yamshanov, me [at] anticode.ninja

#ifndef LOG_ITEM_H
#define LOG_ITEM_H

#include <QString>
#include "constants.h"

class LogItem
{
public:
    quint64 Id;
    LogItemType Type;
    LogLevel Level;
    LogColor Color;
    qint64 Timestamp;
    QString Source;
    QString Message;
};

#endif // LOG_ITEM_H
