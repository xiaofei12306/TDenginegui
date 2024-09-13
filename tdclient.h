#ifndef TDCLIENT_H
#define TDCLIENT_H
#include <QStandardItem>
#include "sqlclient.h"
#include "taos.h"
#include "taosexecuteresult.h"

TAOS *testClient(sqlClient client);
void getDataBase(TAOS *taos, QStandardItem *item);
void getTables(TAOS *taos, QStandardItem *item);
long getCount(TAOS *taos, string table, string dataBase);
QList<QString> getDataBase(TAOS *taos);
TaosExecuteResult syncExecute(QString sql, QString dataBase, TAOS *taos);
TaosExecuteResult deleteRow(QString dataBase, QString table, QStringList rowData, QList<TaosField> fieldList, TAOS *taos);
bool isNeedQuotation(int8_t type);
bool isQuerySql(QString sql);
TaosExecuteResult deleteDataBase(QString dataBase, TAOS *taos);
TaosExecuteResult getDataBaseCreateSql(QString dataBaseName, TAOS *taos);
TaosExecuteResult getSuperTableCreateSql(QString tableName, TAOS *taos);
#endif // TDCLIENT_H
