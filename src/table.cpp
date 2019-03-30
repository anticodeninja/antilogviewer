#include "table.h"

#include <QEvent>
#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QtDebug>

#include "constants.h"

Table::Table(QWidget *parent)
    : QTableView(parent)
{
    viewport()->installEventFilter(this);
    setMouseTracking(true);

    _popupLabel = new QLabel();
    _popupLabel->setWordWrap(true);

    _popup = new QDialog(this, Qt::Popup | Qt::ToolTip);

    auto layout = new QVBoxLayout(_popup);
    layout->addWidget(_popupLabel);
}

bool Table::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == viewport()) {
        if (event->type() == QEvent::MouseMove) {
            auto mouseEvent = static_cast<QMouseEvent*>(event);
            auto index = QTableView::indexAt(mouseEvent->pos());
            if (index.isValid()) {
                if (index.column() == 2) {
                    auto rect = QTableView::visualRect(index);

                    auto palette = _popupLabel->palette();
                    palette.setColor(QPalette::Window, index.data(Qt::BackgroundColorRole).value<QColor>());
                    palette.setColor(QPalette::WindowText, index.data(Qt::TextColorRole).value<QColor>());

                    // It can look strange, but is tradeoff to get good table perfomance
                    _popupLabel->setText(index.data(Qt::DisplayRole).toString().replace(NEWLINE_CHAR, '\n'));
                    _popupLabel->setPalette(palette);
                    _popupLabel->setFixedWidth(rect.width());
                    _popup->adjustSize();

                    auto position = QTableView::visualRect(index).bottom() + _popup->height() < viewport()->height()
                            ? QTableView::visualRect(index).bottomLeft()
                            : QTableView::visualRect(index).topLeft() - QPoint(0, _popup->height());
                    _popup->move(viewport()->mapToGlobal(position));
                    _popup->show();
                } else {
                    _popup->hide();
                }
            } else {
                _popup->hide();
            }
        } else if(event->type() == QEvent::Leave){
            _popup->hide();
        }
    }
    return QTableView::eventFilter(watched, event);
}

