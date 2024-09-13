#ifndef CUSTOMITEMDELEGATE_H
#define CUSTOMITEMDELEGATE_H

#include <QStyledItemDelegate>


class CustomItemDelegate : public QStyledItemDelegate
{
public:
    CustomItemDelegate();
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // CUSTOMITEMDELEGATE_H
