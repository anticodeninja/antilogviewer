#include "memory_storage.h"

#include <QGridLayout>
#include <QPushButton>

#include "log_item.h"

MemoryStorage::MemoryStorage()
{

}

void MemoryStorage::createUI(QGridLayout *layout)
{
    auto ctrRegen = new QPushButton("Regen");
    ctrRegen->connect(ctrRegen, &QPushButton::clicked, [this] {
        sendClear();
        foreach (auto item, _rows) {
            ChainElement::accept(item);
        }
    });

    auto ctrClear = new QPushButton("Clear");
    ctrClear->connect(ctrClear, &QPushButton::clicked, [this] {
        _rows.clear();
        sendClear();
    });

    layout->addWidget(ctrRegen, 1, 0, 1, 2);
    layout->addWidget(ctrClear, 2, 0, 1, 2);
}

void MemoryStorage::accept(std::shared_ptr<LogItem> item) {
    _rows.append(item);
    ChainElement::accept(item);
}

void MemoryStorage::sendClear()
{
    auto logItem = std::make_shared<LogItem>();
    logItem->Type = LogItemType::Clear;
    ChainElement::accept(logItem);
}
