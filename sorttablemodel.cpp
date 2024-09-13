#include "sorttablemodel.h"

SortTableModel::SortTableModel() {}


SortTableModel::SortTableModel(QObject *parent) : QSortFilterProxyModel(parent)
{

}

void SortTableModel::sort(int column, Qt::SortOrder order)
{
    // 调用基类的排序方法
    // QSortFilterProxyModel::sort(column, order);
    qDebug() << "SortTableModel::sort() called with column" << column << "order" << order;
    QSortFilterProxyModel::sort(column, order);
    // 排序完成后发射信号
    emit sortingStart();


}

bool SortTableModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    qDebug() << "2222222";
    // 排序完成后发射信号
    // emit sortingStart();
    return false;
}
