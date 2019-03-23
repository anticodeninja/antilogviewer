#ifndef MEMORY_STORAGE_H
#define MEMORY_STORAGE_H

#include <memory>
#include <QList>

#include "log_item.h"
#include "chain_elements/chain_element.h"

class QGridLayout;

class MemoryStorage : public ChainElement
{
public:
    MemoryStorage();

    virtual const char* name() {
        return "Memory Storage";
    }

    virtual void createUI(QGridLayout* layout);

    virtual void accept(std::shared_ptr<LogItem> item);

private:
    void sendClear();

    QList<std::shared_ptr<LogItem>> _rows;
};

#endif // MEMORY_STORAGE_H
