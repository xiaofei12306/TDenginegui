#include "customheaderview.h"

#include <QContextMenuEvent>
#include <QMenu>

CustomHeaderView::CustomHeaderView(Qt::Orientation orientation, QWidget *parent)
    : QHeaderView(orientation, parent)
{
    // 确保排序功能启用
    setSectionsClickable(true);
    setSortIndicatorShown(true);
}

void CustomHeaderView::contextMenuEvent(QContextMenuEvent *event)
{
    int index = logicalIndexAt(event->pos());
    if (index != -1) {
        QMenu menu(this);

        QAction *ascOrderAction = menu.addAction("升序");
        QAction *descOrderAction = menu.addAction("降序");
        QAction *removeAction = menu.addAction("移除排序");

        connect(ascOrderAction, &QAction::triggered, this, [=]() {
            this->setSortIndicator(index, Qt::AscendingOrder);
            emit requestContextMenu();
        });

        connect(descOrderAction, &QAction::triggered, this, [=]() {
            this->setSortIndicator(index, Qt::DescendingOrder);
            emit requestContextMenu();
        });

        connect(removeAction, &QAction::triggered, this, [=]() {
            this->setSortIndicator(-1, Qt::AscendingOrder);
            emit requestContextMenu();
        });
        menu.exec(event->globalPos());
    }
}
