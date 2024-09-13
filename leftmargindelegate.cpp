#include "leftmargindelegate.h"

void LeftMarginDelegate::paint(QPainter *painter,
                               const QStyleOptionViewItem &option,
                               const QModelIndex &index) const
{
    QStyleOptionViewItem opt = option;
    opt.rect.adjust(m_leftMargin, 0, 0, 0);
    QItemDelegate::paint(painter, opt, index);
}
