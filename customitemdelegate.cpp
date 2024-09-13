#include "customitemdelegate.h"

CustomItemDelegate::CustomItemDelegate() {}

QSize CustomItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // 返回固定的项高度
    return QSize(option.rect.width(), 22); // 设置高度为22像素
}
