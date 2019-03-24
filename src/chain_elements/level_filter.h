#ifndef LEVEL_FILTER_H
#define LEVEL_FILTER_H

#include "log_item.h"
#include "chain_elements/chain_element.h"

class LevelFilter : public ChainElement
{
public:
    LevelFilter();

    virtual const char* name() {
        return "Level Filter";
    }

    virtual void createUI(QGridLayout* layout);

    virtual void accept(std::shared_ptr<LogItem> item);

private:
    LogLevel _level;
};

#endif // LEVEL_FILTER_H
