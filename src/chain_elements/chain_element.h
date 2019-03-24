#ifndef CHAIN_ELEMENT_H
#define CHAIN_ELEMENT_H

#include <memory>

#include <QtDebug>

class LogItem;
class QGridLayout;

class ChainElement
{
public:
    ChainElement(): _next(nullptr)
    {
    }

    virtual ~ChainElement()
    {
        _next = nullptr;
    }

    virtual const char* name() = 0;

    virtual void createUI(QGridLayout* layout) = 0;

    virtual void setNext(ChainElement* element){
        _next = element;
    }

    virtual ChainElement* getNext(){
        return _next;
    }

    virtual void accept(std::shared_ptr<LogItem> item) {
        if (_next) _next->accept(item);
    }

private:
    ChainElement* _next;
};

#endif // CHAIN_ELEMENT_H
