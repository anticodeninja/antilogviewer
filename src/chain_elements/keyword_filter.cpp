#include "keyword_filter.h"

#include <QGridLayout>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>

KeywordFilter::KeywordFilter() {
}

void KeywordFilter::createUI(QGridLayout *layout)
{
    auto ctrInput = new QLineEdit();

    auto ctrAdd = new QPushButton("+");
    ctrAdd->connect(ctrAdd, &QPushButton::clicked, [this, layout, ctrInput, ctrAdd] {
        auto text = ctrInput->text().trimmed();
        ctrInput->clear();

        if (text.count() == 0 || _keywords.contains(text))
            return;
        _keywords.append(text);

        auto ctrLabel = new QLabel(text);
        auto ctrRemove = new QPushButton("-");
        ctrRemove->connect(ctrRemove, &QPushButton::clicked, [this, layout, ctrRemove] {
            int row, column, rowSpan, colSpan;
            layout->getItemPosition(layout->indexOf(ctrRemove), &row, &column, &rowSpan, &colSpan);
            _keywords.removeAt(row - 1);

            for (auto j = 0; j < layout->columnCount(); ++j) {
                delete layout->itemAtPosition(row, j)->widget();
            }

            for (auto i = row, end = _keywords.count() + 1; i < end; ++i) {
                for (auto j = 0; j < layout->columnCount(); ++j) {
                    layout->addWidget(layout->itemAtPosition(i + 1, j)->widget(), i, j);
                }
            }
        });

        layout->addWidget(ctrLabel, _keywords.count(), 0);
        layout->addWidget(ctrRemove, _keywords.count(), 1);
        layout->addWidget(ctrInput, _keywords.count() + 1, 0);
        layout->addWidget(ctrAdd, _keywords.count() + 1, 1);
    });

    layout->addWidget(ctrInput, _keywords.count() + 1, 0);
    layout->addWidget(ctrAdd, _keywords.count() + 1, 1);
}

void KeywordFilter::accept(std::shared_ptr<LogItem> item)
{
    if (item->Type != LogItemType::Log || _keywords.count() == 0) {
        ChainElement::accept(item);
    } else {
        foreach (auto keyword, _keywords) {
            if (item->Message.contains(keyword)) {
                ChainElement::accept(item);
                return;
            }
        }
    }
}