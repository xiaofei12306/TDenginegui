#include "taosexecuteresult.h"

#include <QHeaderView>
#include <QStandardItemModel>
#include <QTableView>

TaosExecuteResult::TaosExecuteResult() {}

TaosExecuteResult::TaosExecuteResult(const QString &errorMsg, int row, const QList<QStringList> &rowData, const QList<TaosField> &fieldList) : errorMsg(errorMsg),
    row(row),
    rowData(rowData),
    fieldList(fieldList)
{}

QString TaosExecuteResult::getErrorMsg() const
{
    return errorMsg;
}

void TaosExecuteResult::setErrorMsg(const QString &newErrorMsg)
{
    errorMsg = newErrorMsg;
}

int TaosExecuteResult::getRow() const
{
    return row;
}

void TaosExecuteResult::setRow(int newRow)
{
    row = newRow;
}

QList<QStringList> TaosExecuteResult::getRowData() const
{
    return rowData;
}

void TaosExecuteResult::setRowData(const QList<QStringList> &newRowData)
{
    rowData = newRowData;
}

QList<TaosField> TaosExecuteResult::getFieldList() const
{
    return fieldList;
}

void TaosExecuteResult::setFieldList(const QList<TaosField> &newFieldList)
{
    fieldList = newFieldList;
}

qint64 TaosExecuteResult::getConsumingTime() const
{
    return consumingTime;
}

void TaosExecuteResult::setConsumingTime(qint64 newConsumingTime)
{
    consumingTime = newConsumingTime;
}

void TaosExecuteResult::renderingTable(QTableView *tableView)
{
    QStandardItemModel *tableModel = new QStandardItemModel(tableView);

    for (int i = 0; i < getFieldList().size(); ++i) {
        QStandardItem *headerItem = new QStandardItem(getFieldList()[i].getName());
        headerItem->setData(QVariant::fromValue(getFieldList()[i]), Qt::UserRole);
        tableModel->setHorizontalHeaderItem(i, headerItem);

    }

    for (int i = 0; i < getRowData().size(); ++i) {
        QStringList lineDataList = getRowData()[i];
        for (int j = 0; j < lineDataList.size(); ++j) {
            tableModel->setItem(i, j, new QStandardItem(lineDataList[j]));
        }
    }

    tableView->setModel(tableModel);
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}
