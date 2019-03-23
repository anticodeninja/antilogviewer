#ifndef CHAINELEMENT_H
#define CHAINELEMENT_H

#include <memory>

#include <QtDebug>

class LogItem;
class QGridLayout;

class ChainElement
{
public:
    ChainElement(): _next(nullptr)
    {
    };

    virtual const char* name() = 0;

    virtual void createUI(QGridLayout* layout) = 0;

    virtual void setNext(ChainElement* element){
        _next = element;
    }

    virtual void accept(std::shared_ptr<LogItem> item) {
        if (_next) _next->accept(item);
    };

private:
    ChainElement* _next;
};

#endif // CHAINELEMENT_H
