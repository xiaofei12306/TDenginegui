#ifndef TREEVIEWWITHMENU_H
#define TREEVIEWWITHMENU_H

#include "customtabwidget.h"
#include "sqlclient.h"
#include "treeitemmodel.h"

#include <QStandardItem>
#include <QTreeView>


class TreeViewWithMenu : public QTreeView
{
    Q_OBJECT
public:
    explicit TreeViewWithMenu(QWidget *parent = nullptr);

    CustomTabWidget *getTabWidget() const;
    void setTabWidget(CustomTabWidget *newTabWidget);

private:
    CustomTabWidget *tabWidget;
    void handleClientDoubleClicked(QStandardItem *item, TreeItemModel &itemData);
    void handleDatabaseDoubleClicked(QStandardItem *item);
    void handleTableDoubleClicked(QStandardItem *item);
    void handleCloseClicked(QModelIndex index, QStandardItem *item, TreeItemModel itemData);
    void handleRemoveClicked(QModelIndex index, QStandardItem *item, TreeItemModel itemData);
    void handleEditClicked(QModelIndex index, QStandardItem *item, TreeItemModel itemData);
    void handleCloseTabWidget(QString clientName);
    void handleCloseDataBase(QModelIndex index, QStandardItem *item);
    void handleRemoveDataBase(QModelIndex index, QStandardItem *item, TreeItemModel itemData);
    void hanldeGetCreateSql(QStandardItem *item, Level level);
public slots:
    void onTreeViewDoubleClicked(const QModelIndex &index);
    void onEditSqlClient(QStandardItem *item, sqlClient sqlClient);
private slots:
    void showContextMenu(const QPoint &pos);
};

#endif // TREEVIEWWITHMENU_H
