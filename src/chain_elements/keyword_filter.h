#ifndef KEYWORD_FILTER_H
#define KEYWORD_FILTER_H

#include <QList>

#include "log_item.h"
#include "chain_element.h"

class KeywordFilter : public ChainElement
{
public:
    KeywordFilter();

    virtual const char* name() {
        return "Keyword Filter";
    }

    virtual void createUI(QGridLayout* layout);

    virtual void accept(std::shared_ptr<LogItem> item);

private:
    QList<QString> _keywords;
};

#endif // KEYWORD_FILTER_H
