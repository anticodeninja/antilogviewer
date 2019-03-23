#include "level_filter.h"

#include <QGridLayout>
#include <QComboBox>

LevelFilter::LevelFilter()
    : _level(LogLevel::WARN)
{
}

void LevelFilter::createUI(QGridLayout* layout)
{
    auto ctrLevel = new QComboBox();
    ctrLevel->addItems(QStringList({"Trace", "Debug","Info", "Warn", "Error", "Fatal"}));
    ctrLevel->setCurrentIndex(static_cast<int>(_level));
    ctrLevel->connect(ctrLevel, QOverload<int>::of(&QComboBox::currentIndexChanged), [this] (int index) {
        _level = static_cast<LogLevel>(index);
    });
    layout->addWidget(ctrLevel, 1, 0, 1, 2);
}

void LevelFilter::accept(std::shared_ptr<LogItem> item)
{
    if (item->Type != LogItemType::Log || item->Level >= _level)
        ChainElement::accept(item);
}
