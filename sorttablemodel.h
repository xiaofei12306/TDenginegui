#ifndef SORTTABLEMODEL_H
#define SORTTABLEMODEL_H
#include <QSortFilterProxyModel>


class SortTableModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit SortTableModel();
    explicit SortTableModel(QObject *parent = nullptr);
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;
    signals:
        void sortingStart();

};

#endif // SORTTABLEMODEL_H
