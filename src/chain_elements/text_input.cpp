// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// Copyright 2019 Artem Yamshanov, me [at] anticode.ninja

#include "text_input.h"

#include <QJsonObject>
#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QFileInfo>
#include <QDateTime>

#include "constants.h"

TextInput::TextInput()
    : _levels({
        { "TRACE", LogLevel::TRACE },
        { "DEBUG", LogLevel::DEBUG },
        { "INFO", LogLevel::INFO },
        { "WARN", LogLevel::WARN },
        { "ERROR", LogLevel::ERROR },
        { "FATAL", LogLevel::FATAL },
    })
{
    _path = QDir::home().filePath("log.txt");
}

void TextInput::createUI(QGridLayout* layout) {
    auto ctrPath = new QLineEdit();
    ctrPath->setText(_path);

    auto ctrOpen = new QPushButton("Open");
    ctrOpen->connect(ctrOpen, &QPushButton::clicked, [this, ctrPath] {
        auto result = QFileDialog::getOpenFileName(ctrPath, QString(), ctrPath->text(), "Log Files (*.*)");
        if (result != QString())
        {
            _path = result;
            ctrPath->setText(result);
        }
    });

    auto ctrRead = new QPushButton("Read");
    ctrRead->connect(ctrRead, &QPushButton::clicked, [this] {
        auto logItem = std::make_shared<LogItem>();
        logItem->Type = LogItemType::Clear;
        ChainElement::accept(logItem);

        QFile file(_path);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return;

        logItem = nullptr;

        QTextStream stream(&file);
        while (!stream.atEnd()) {
            auto line = stream.readLine();

            auto sep1 = line.indexOf('|', 0);
            auto sep2 = line.indexOf('|', sep1 + 1);
            auto sep3 = line.indexOf('|', sep2 + 1);

            auto datetime = QDateTime::fromString(line.left(23), "yyyy-MM-dd HH:mm:ss.zzz");
            if (datetime.isValid()) {
                if (logItem != nullptr)
                    ChainElement::accept(logItem);

                logItem = std::make_shared<LogItem>();
                logItem->Type = LogItemType::Log;
                logItem->Timestamp = datetime.toMSecsSinceEpoch();
                logItem->Level = _levels[line.mid(sep1 + 1, sep2 - sep1 - 1)];
                logItem->Source = line.mid(sep2 + 1, sep3 - sep2 - 1);
                logItem->Message = line.mid(sep3 + 1);
            } else {
                logItem->Message += NEWLINE_CHAR + line;
            }

            ChainElement::accept(logItem);
        }

        if (logItem != nullptr)
            ChainElement::accept(logItem);
    });

    layout->addWidget(ctrPath, 1, 0);
    layout->addWidget(ctrOpen, 1, 1);
    layout->addWidget(ctrRead, 2, 0, 1, 2);
}

void TextInput::load(const QJsonObject &data)
{
    if (data["path"].isString())
        _path = data["path"].toString();
}

void TextInput::save(QJsonObject &data) const
{
    data["path"] = _path;
}
