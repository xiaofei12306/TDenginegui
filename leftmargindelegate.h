#ifndef LEFTMARGINDELEGATE_H
#define LEFTMARGINDELEGATE_H

#include <QItemDelegate>

class LeftMarginDelegate : public QItemDelegate
{
public:
    LeftMarginDelegate(int leftMargin, QObject *parent = nullptr)
        : QItemDelegate(parent)
        , m_leftMargin(leftMargin)
    {}
    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

private:
    int m_leftMargin;
};

#endif // LEFTMARGINDELEGATE_H
