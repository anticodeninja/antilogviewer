// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// Copyright 2019 Artem Yamshanov, me [at] anticode.ninja

#include "table_view_config.h"

#include <QColorDialog>
#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>

#include "default_settings.h"

void setButtonColor(QPushButton* button, QColor color) {
    QPalette palette = button->palette();
    palette.setColor(QPalette::Button, color);
    button->setPalette(palette);
}

QColor getButtonColor(QPushButton* button) {
    QPalette palette = button->palette();
    return palette.button().color();
}

QPushButton* createColorButton(QWidget *parent, QComboBox *ctrPalettes) {
    auto ctrButton = new QPushButton("", parent);
    ctrButton->setFlat(true);
    ctrButton->setAutoFillBackground(true);
    ctrButton->connect(ctrButton, &QPushButton::clicked, [ctrButton, ctrPalettes] {
        auto result = QColorDialog::getColor(getButtonColor(ctrButton), ctrButton);
        if (result.isValid()) {
            ctrPalettes->setCurrentText("Custom");
            setButtonColor(ctrButton, result);
        }
    });
    return ctrButton;
}

QComboBox* createElideSelector(QWidget *parent) {
    auto ctrComboBox = new QComboBox(parent);
    ctrComboBox->addItem("Left", Qt::ElideLeft);
    ctrComboBox->addItem("Middle", Qt::ElideMiddle);
    ctrComboBox->addItem("Right", Qt::ElideRight);
    return ctrComboBox;
}

TableViewConfig::TableViewConfig(QWidget *parent) : QDialog(parent)
{
    auto panelLayout = new QGridLayout(this);

    auto currentRow = 0;

    panelLayout->addWidget(new QLabel("Palette", this), currentRow, 0, 1, 1);
    _ctrPalettes = new QComboBox(this);
    _ctrPalettes->addItem("Dark");
    _ctrPalettes->addItem("Light");
    _ctrPalettes->addItem("Custom");
    _ctrPalettes->connect(_ctrPalettes, &QComboBox::currentTextChanged, [this](const QString& text) {
        if (text == "Custom")
            return;

        auto textColors = text == "Dark" ? DARK_TEXT_COLORS : LIGHT_TEXT_COLORS;
        auto backColors = text == "Dark" ? DARK_BACK_COLORS : LIGHT_BACK_COLORS;

        for (auto i = 0; i < static_cast<int>(LogColor::End); ++i) {
            setButtonColor(_ctrTextColors[i], textColors[i]);
            setButtonColor(_ctrBackColors[i], backColors[i]);
        }
    });
    panelLayout->addWidget(_ctrPalettes, currentRow, 1, 1, 2);
    currentRow += 1;

    panelLayout->addWidget(new QLabel("Timestamp", this), currentRow, 0, 1, 1);
    _ctrTimestamp = new QComboBox(parent);
    _ctrTimestamp->addItem("hh:mm:ss.zzz");
    _ctrTimestamp->addItem("yyyy-MM-dd HH:mm:ss.zzz");
    _ctrTimestamp->setEditable(true);
    panelLayout->addWidget(_ctrTimestamp, currentRow, 1, 1, 2);
    currentRow += 1;

    panelLayout->addWidget(new QLabel("Source Elide", this), currentRow, 0, 1, 1);
    _ctrSourceElide = createElideSelector(this);
    panelLayout->addWidget(_ctrSourceElide, currentRow, 1, 1, 2);
    currentRow += 1;

    panelLayout->addWidget(new QLabel("Message Elide", this), currentRow, 0, 1, 1);
    _ctrMessageElide = createElideSelector(this);
    panelLayout->addWidget(_ctrMessageElide, currentRow, 1, 1, 2);
    currentRow += 1;

    for (auto i = 0; i < static_cast<int>(LogColor::End); ++i) {
        panelLayout->addWidget(new QLabel(QString("%0 Colors:").arg(ColorNames[i]), this), currentRow, 0, 1, 1);

        auto textButton = createColorButton(this, _ctrPalettes);
        _ctrTextColors.append(textButton);
        panelLayout->addWidget(textButton, currentRow, 1, 1, 1);

        auto backButton = createColorButton(this, _ctrPalettes);
        _ctrBackColors.append(backButton);
        panelLayout->addWidget(backButton, currentRow, 2, 1, 1);

        currentRow += 1;
    }

    auto ctrApplyButton = new QPushButton("Apply");
    ctrApplyButton->connect(ctrApplyButton, &QPushButton::clicked, [this] {
        accept();
    });
    panelLayout->addWidget(ctrApplyButton, currentRow, 0, 1, 1);

    auto ctrCancelButton = new QPushButton("Cancel");
    ctrCancelButton->connect(ctrCancelButton, &QPushButton::clicked, [this] {
        reject();
    });
    panelLayout->addWidget(ctrCancelButton, currentRow, 1, 1, 2);
}

void TableViewConfig::calcPalette()
{
    auto isDark = true;
    auto isLight = true;

    for (auto i = 0; i < static_cast<int>(LogColor::End); ++i) {
        auto textColor = getButtonColor(_ctrTextColors[i]);
        isDark &= textColor == DARK_TEXT_COLORS[i];
        isLight &= textColor == LIGHT_TEXT_COLORS[i];

        auto backColor = getButtonColor(_ctrBackColors[i]);
        isDark &= backColor == DARK_BACK_COLORS[i];
        isLight &= backColor == LIGHT_BACK_COLORS[i];
    }

    if (isDark)
        _ctrPalettes->setCurrentText("Dark");
    else if (isLight)
        _ctrPalettes->setCurrentText("Light");
    else
        _ctrPalettes->setCurrentText("Custom");
}

void TableViewConfig::setTextColor(LogColor level, QColor color)
{
    setButtonColor(_ctrTextColors[static_cast<int>(level)], color);
}

void TableViewConfig::setBackColor(LogColor level, QColor color)
{
    setButtonColor(_ctrBackColors[static_cast<int>(level)], color);
}

void TableViewConfig::setTimeFormat(const QString& value)
{
    _ctrTimestamp->setEditText(value);
}

void TableViewConfig::setSourceElide(Qt::TextElideMode value)
{
    _ctrSourceElide->setCurrentIndex(_ctrSourceElide->findData(value));
}

void TableViewConfig::setMessageElide(Qt::TextElideMode value)
{
    _ctrMessageElide->setCurrentIndex(_ctrMessageElide->findData(value));
}

QColor TableViewConfig::textColor(LogColor level) const
{
    return getButtonColor(_ctrTextColors[static_cast<int>(level)]);
}

QColor TableViewConfig::backColor(LogColor level) const
{
    return getButtonColor(_ctrBackColors[static_cast<int>(level)]);
}

QString TableViewConfig::timeFormat() const
{
    return _ctrTimestamp->currentText();
}

Qt::TextElideMode TableViewConfig::sourceElide() const
{
    return static_cast<Qt::TextElideMode>(_ctrSourceElide->currentData().toInt());
}

Qt::TextElideMode TableViewConfig::messageElide() const
{
    return static_cast<Qt::TextElideMode>(_ctrMessageElide->currentData().toInt());
}
