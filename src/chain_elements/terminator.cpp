#include "terminator.h"

#include "table_model.h"

void Terminator::accept(std::shared_ptr<LogItem> item)
{
    _tableModel->add(*item);
}
