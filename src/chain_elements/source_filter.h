#ifndef SOURCE_FILTER_H
#define SOURCE_FILTER_H

#include <QSet>
#include <QList>

#include "log_item.h"
#include "chain_element.h"

class QMenu;

class SourceFilter : public ChainElement
{
public:
    SourceFilter();

    virtual const char* name() {
        return "Source Filter";
    }

    virtual void createUI(QGridLayout* layout);

    virtual void accept(std::shared_ptr<LogItem> item);

private:
    void addSource(QString source);

    QMenu* _menu;
    QSet<QString> _allSources;
    QList<QString> _sources;
};

#endif // SOURCE_FILTER_H
