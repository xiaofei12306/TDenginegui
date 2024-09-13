#include "tdclient.h"
#include <QDateTime>
#include <QLineEdit>
#include <QStandardItemModel>
#include <QTableWidget>
#include <QTimeZone>
#include <QToolBar>
#include "qdebug.h"
#include "taos.h"
#include "treeitemmodel.h"
#include <MainWindow.h>
#include <sqlclient.h>
#include <taosfield.h>
#include "taosexecuteresult.h"
using namespace std;
#define VARSTR_HEADER_SIZE 2

const QStringList QUERY_SQL_LIST = {"update", "UPDATE", "DELETE", "delete", "DROP", "drop", "ALTER", "alter"};

int32_t varDataLen(const char *ptr)
{
    uint16_t len;
    memcpy(&len, ptr, sizeof(len)); // 读取头部的长度信息
    return len;
}

TAOS *testClient(sqlClient client)
{
    return taos_connect(client.getIp().toStdString().c_str(),
                        client.getUsername().toStdString().c_str(),
                        client.getPassword().toStdString().c_str(),
                        NULL,
                        client.getPort());
}

void taos_retrieve_call_back(void *param, TAOS_RES *tres, int numOfRows)
{
    // struct timeval systemTime;

    if (numOfRows > 0) {
        // 将 void * 转换为 Ui::MainWindow *
        QStandardItem *rootItem = static_cast<QStandardItem *>(param);
        for (int i = 0; i < numOfRows; ++i) {
            // synchronous API to retrieve a row from batch of records
            TAOS_ROW row = taos_fetch_row(tres);
            // 创建一个列表，包含多个子项
            int32_t charLen = varDataLen((char *) row[0] - VARSTR_HEADER_SIZE);
            QStandardItem *childItem1 = new QStandardItem(
                QString::fromLocal8Bit((char *) row[0], charLen));
            childItem1->setIcon(QIcon(":/icon/icon/closeDataBase.png"));
            TreeItemModel itemModel(Level::DATABASE);
            childItem1->setData(QVariant::fromValue(itemModel), Qt::UserRole);
            // 将子节点添加到根节点
            rootItem->appendRow(childItem1);
        }
        // process row
        taos_fetch_rows_a(tres, taos_retrieve_call_back, param);

    } else {
        taos_free_result(tres);
    }
}

void taos_select_call_back(void *param, TAOS_RES *tres, int code)
{
    if (code == 0 && tres) {
        // asynchronous API to fetch a batch of records
        taos_fetch_rows_a(tres, taos_retrieve_call_back, param);
    } else {
        taos_free_result(tres);
    }
}

void getDataBase(TAOS *taos, QStandardItem *item)
{
    taos_query_a(taos, "SHOW DATABASES;", taos_select_call_back, item);
}

void taos_retrieve_table_call_back(void *param, TAOS_RES *tres, int numOfRows)
{
    if (numOfRows > 0) {
        auto pairPtr = *static_cast<std::shared_ptr<std::pair<Level, QStandardItem *>> *>(param);

        Level level = pairPtr->first;
        QStandardItem *rootItem = pairPtr->second;
        for (int i = 0; i < numOfRows; ++i) {
            // synchronous API to retrieve a row from batch of records
            TAOS_ROW row = taos_fetch_row(tres);
            // 创建一个列表，包含多个子项
            int32_t charLen = varDataLen((char *) row[0] - VARSTR_HEADER_SIZE);
            // 使用QString来创建QStandardItem
            QStandardItem *childItem1 = new QStandardItem(
                QString::fromLocal8Bit((char *) row[0], charLen));
            if (level == Level::TABLE) {
                childItem1->setIcon(QIcon(":/icon/icon/superTable.png"));
            } else {
                childItem1->setIcon(QIcon(":/icon/icon/childTable.png"));
            }
            TreeItemModel itemModel(level);
            childItem1->setData(QVariant::fromValue(itemModel), Qt::UserRole);
            // 将子节点添加到根节点
            rootItem->appendRow(childItem1);
        }
        // process row
        taos_fetch_rows_a(tres, taos_retrieve_table_call_back, param);

    } else {
        taos_free_result(tres);
    }
}

void taos_select_table_back(void *param, TAOS_RES *tres, int code)
{
    if (code == 0 && tres) {
        // asynchronous API to fetch a batch of records
        taos_fetch_rows_a(tres, taos_retrieve_table_call_back, param);
    } else {
        qDebug() << taos_errstr(tres);
        taos_free_result(tres);
    }
}

void getTables(TAOS *taos, QStandardItem *item)
{
    string sql = "SHOW " + item->text().toStdString() + ".STABLES;";
    auto pair = std::make_shared<std::pair<Level, QStandardItem *>>(Level::TABLE, item);
    taos_query_a(taos,
                 sql.c_str(),
                 taos_select_table_back,
                 new std::shared_ptr<std::pair<Level, QStandardItem *>>(pair));

    sql = "select table_name from information_schema.ins_tables where db_name='" + item->text().toStdString() + "' and stable_name is null";
    auto pair1 = std::make_shared<std::pair<Level, QStandardItem *>>(Level::SUBTABLE, item);
    taos_query_a(taos,
                 sql.c_str(),
                 taos_select_table_back,
                 new std::shared_ptr<std::pair<Level, QStandardItem *>>(pair1));
}

long getCount(TAOS *taos, string table, string dataBase)
{
    string sql = "select count(*) from " + dataBase + "." + table;
    TAOS_RES *result = taos_query(taos, sql.c_str());
    if (result == NULL || taos_errno(result) != 0) {
        qDebug() << "failed to select, reason:%s\n" << taos_errstr(result);
        taos_free_result(result);
        return 0;
    }
    TAOS_ROW row = taos_fetch_row(result);
    long pages =  static_cast<long>(*(int64_t *) row[0]);
    taos_free_result(result);
    return pages;
}

QList<QString> getDataBase(TAOS *taos)
{
    QList<QString>dataBaseList;
    string sql = "SHOW DATABASES;";
    TAOS_RES *result = taos_query(taos, sql.c_str());
    if (result == NULL || taos_errno(result) != 0) {
        qDebug() << "failed to select, reason:" << taos_errstr(result);
        taos_free_result(result);
        return dataBaseList;
    }
    TAOS_ROW row;
    while ((row = taos_fetch_row(result))) {
        int32_t charLen = varDataLen((char *) row[0] - VARSTR_HEADER_SIZE);
        dataBaseList<< QString::fromLocal8Bit((char *) row[0], charLen);
    }
    taos_free_result(result);
    return dataBaseList;
}

TaosExecuteResult syncExecute(QString sql, QString dataBase, TAOS *taos) {
    if(!dataBase.isNull()){
        QString switchDatabase = "use " % dataBase;
        taos_query(taos, switchDatabase.toStdString().c_str());
    }
    TaosExecuteResult executeResult;
    qint64 beginTimestamp = QDateTime::currentDateTime().toMSecsSinceEpoch();
    TAOS_RES *result = taos_query(taos, sql.toStdString().c_str());
    qint64 endTimestamp = QDateTime::currentDateTime().toMSecsSinceEpoch();
    executeResult.setConsumingTime(endTimestamp - beginTimestamp);

    if (result == NULL || taos_errno(result) != 0) {
        const char *errorMsg= taos_errstr(result);
        qDebug() << "failed to select, reason:" << errorMsg;
        executeResult.setErrorMsg(errorMsg);
        taos_free_result(result);
        return executeResult;
    }
    TAOS_ROW row;
    int num_fields = taos_field_count(result);
    TAOS_FIELD *fields = taos_fetch_fields(result);
    QList<TaosField> fieldList;
    for (int var = 0; var < num_fields; ++var) {
        fieldList << TaosField (fields[var].type, fields[var].bytes, QString::fromStdString(string((char *) fields[var].name)));
    }
    executeResult.setFieldList(fieldList);
    int updateRow = taos_affected_rows(result);
    executeResult.setRow(updateRow);
    QList<QStringList> dataList;
    while ((row = taos_fetch_row(result))) {
        QStringList lineList;
        for (int var = 0; var < num_fields; ++var) {
            if (row[var] == nullptr) {
                continue;
            }
            switch (fields[var].type) {
            case TSDB_DATA_TYPE_TINYINT:
                lineList<< QString::number(*((int8_t *) row[var]));
                break;

            case TSDB_DATA_TYPE_UTINYINT:
                lineList<< QString::number(*((uint8_t *) row[var]));
                break;

            case TSDB_DATA_TYPE_SMALLINT:
                lineList<< QString::number(*((int16_t *) row[var]));
                break;

            case TSDB_DATA_TYPE_USMALLINT:
                lineList<< QString::number(*((uint16_t *) row[var]));
                break;

            case TSDB_DATA_TYPE_INT:
                lineList<< QString::number(*((int32_t *) row[var]));
                break;

            case TSDB_DATA_TYPE_UINT:
                lineList<< QString::number(*((uint32_t *) row[var]));
                break;

            case TSDB_DATA_TYPE_BIGINT:
                lineList<< QString::number(*((int64_t *) row[var]), 10);
                break;

            case TSDB_DATA_TYPE_UBIGINT:
                lineList<< QString::number(*((uint64_t *) row[var]), 10);
                break;

            case TSDB_DATA_TYPE_FLOAT: {
                lineList<< QString::number(*(float *) row[var]);
            } break;

            case TSDB_DATA_TYPE_DOUBLE: {
                lineList<< QString::number(*(double *) row[var]);
            } break;

            case TSDB_DATA_TYPE_BINARY:
            case TSDB_DATA_TYPE_NCHAR: {
                // 读取字符串长度
                int32_t charLen = varDataLen((char *) row[var] - VARSTR_HEADER_SIZE);
                lineList<< QString::fromLocal8Bit((char *) row[var], charLen);
            } break;

            case TSDB_DATA_TYPE_TIMESTAMP: {
                // 从指针中获取值
                int64_t timestampValue = *(int64_t *) row[var];
                // 转换为 quint64
                quint64 quint64Timestamp = static_cast<quint64>(timestampValue);
                QDateTime dateTime = QDateTime::fromMSecsSinceEpoch(quint64Timestamp);
                // 格式化日期时间字符串
                QString formattedDateTime = dateTime.toString("yyyy-MM-dd HH:mm:ss");
                lineList<< formattedDateTime;
            } break;

            case TSDB_DATA_TYPE_BOOL:
                lineList<< QString::number(*((int8_t *) row[var]));
                break;

            default:
                // 对于未知类型，您可以选择忽略或者抛出异常
                break;
            }
        }
        dataList << lineList;
    }
    executeResult.setRowData(dataList);
    taos_free_result(result);
    return executeResult;
}

TaosExecuteResult deleteRow(QString dataBase, QString table, QStringList rowData, QList<TaosField> fieldList, TAOS *taos){
    QString sql = "delete from " % dataBase;
    if(!table.isNull() && !table.isEmpty()){
        sql.append("." % table);
    }
    sql.append(" where ");
    for (int i = 0; i < rowData.size(); ++i) {
        TaosField taosField = fieldList[i];
        sql.append(taosField.getName() % "=");
        sql.append(isNeedQuotation(taosField.getType()) ? "'" % rowData[i] % "'" : rowData[i]);
        if(i < rowData.size() - 1){
            sql.append(" and ");
        }
    }
    qDebug()<< sql;
    return syncExecute(sql, nullptr, taos);
}

TaosExecuteResult deleteDataBase(QString dataBase, TAOS *taos) {
    QString sql = "DROP DATABASE " % dataBase;
    return syncExecute(sql, nullptr, taos);
}


bool isNeedQuotation(int8_t type){
    switch (type) {
    case TSDB_DATA_TYPE_VARCHAR:
    case TSDB_DATA_TYPE_NCHAR:
    case TSDB_DATA_TYPE_JSON:
    case TSDB_DATA_TYPE_VARBINARY:
    case TSDB_DATA_TYPE_BLOB:
    case TSDB_DATA_TYPE_TIMESTAMP:
        return true;
    default:
        return false;
    }
}

bool isQuerySql(QString sql){
    foreach (QString prefixSql, QUERY_SQL_LIST) {
        if(sql.startsWith(prefixSql)){
            return false;
        }
    }
    return true;
}

TaosExecuteResult getDataBaseCreateSql(QString dataBaseName, TAOS *taos){
    QString sql = "SHOW CREATE DATABASE " % dataBaseName;
    return syncExecute(sql, nullptr, taos);
}

TaosExecuteResult getSuperTableCreateSql(QString tableName, TAOS *taos){
    QString sql = "SHOW CREATE STABLE " % tableName;
    return syncExecute(sql, nullptr, taos);
}
