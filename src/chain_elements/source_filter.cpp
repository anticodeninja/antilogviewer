#include "source_filter.h"

#include <QGridLayout>
#include <QMenu>
#include <QLabel>
#include <QPushButton>

SourceFilter::SourceFilter()
{

}

void SourceFilter::createUI(QGridLayout *layout)
{
    layout->setColumnStretch(0, 1000);

    _menu = new QMenu();

    auto ctrAdd = new QPushButton("+");
    ctrAdd->setMenu(_menu);
    _menu->connect(_menu, &QMenu::triggered, [this, layout, ctrAdd](QAction* action){
        auto source = action->data().toString();

        if (_sources.contains(source))
            return;
        _sources.append(source);

        auto ctrLabel = new QLabel(source);

        auto ctrRemove = new QPushButton("-");
        ctrRemove->setFixedWidth(20);
        ctrRemove->connect(ctrRemove, &QPushButton::clicked, [this, layout, ctrRemove] {
            int row, column, rowSpan, colSpan;
            layout->getItemPosition(layout->indexOf(ctrRemove), &row, &column, &rowSpan, &colSpan);
            _sources.removeAt(row - 1);

            for (auto j = 0; j < layout->columnCount(); ++j) {
                delete layout->itemAtPosition(row, j)->widget();
            }

            for (auto i = row, end = _sources.count() + 1; i < end; ++i) {
                for (auto j = 0; j < layout->columnCount(); ++j) {
                    layout->addWidget(layout->itemAtPosition(i + 1, j)->widget(), i, j);
                }
            }
        });

        layout->addWidget(ctrLabel, _sources.count(), 0);
        layout->addWidget(ctrRemove, _sources.count(), 1);
        layout->addWidget(ctrAdd, _sources.count() + 1, 0, 1, 2);
    });

    layout->addWidget(ctrAdd, _sources.count() + 1, 0, 1, 2);
}

void SourceFilter::accept(std::shared_ptr<LogItem> item)
{
    if (item->Type == LogItemType::Log) {
        if (!_allSources.contains(item->Source)) {
            addSource(item->Source);
        }
        if (_sources.empty()) {
            ChainElement::accept(item);
        } else {
            foreach (auto source, _sources) {
                if (item->Source.startsWith(source)) {
                    ChainElement::accept(item);
                    return;
                }
            }
        }
    } else if (item->Type == LogItemType::Clear) {
        _allSources.clear();
        _menu->clear();

        foreach (auto source, _allSources) {
            _allSources.insert(source);
            addSource(source);
        }

        ChainElement::accept(item);
    }
}

void SourceFilter::addSource(QString source)
{
    _allSources.insert(source);

    int index = 0;
    QMenu* menu = _menu;
    for (;;) {
        int pos = source.indexOf('.', index);
        int last = pos == -1;
        int end = last ? source.count() : pos;
        auto chunk = source.mid(index, end - index);

        QMenu* inner = nullptr;

        foreach (auto child, menu->actions()) {
            if (child->text() == chunk) {
                inner = child->menu();
                break;
            }
        }

        if (inner != nullptr) {
            menu = inner;
        } else {
            menu = menu->addMenu(chunk);
            auto action = menu->addAction("this");
            action->setData(source.left(end));
        }

        if (last)
            break;
        index = pos + 1;
    }
}
