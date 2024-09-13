#ifndef TAOSEXECUTERESULT_H
#define TAOSEXECUTERESULT_H

#include "taosfield.h"

#include <QString>
#include <QList>
#include <QStringList>
#include <QTableView>



class TaosExecuteResult
{
public:
    TaosExecuteResult();
    TaosExecuteResult(const QString &errorMsg, int row, const QList<QStringList> &rowData, const QList<TaosField> &fieldList);

    QString getErrorMsg() const;
    void setErrorMsg(const QString &newErrorMsg);

    int getRow() const;
    void setRow(int newRow);

    QList<QStringList> getRowData() const;
    void setRowData(const QList<QStringList> &newRowData);

    QList<TaosField> getFieldList() const;
    void setFieldList(const QList<TaosField> &newFieldList);

    qint64 getConsumingTime() const;
    void setConsumingTime(qint64 newConsumingTime);

    void renderingTable(QTableView *tableView);

private:
    QString errorMsg;
    int row = 0;
    QList<QStringList> rowData;
    QList<TaosField> fieldList;
    qint64 consumingTime;
};

#endif // TAOSEXECUTERESULT_H
