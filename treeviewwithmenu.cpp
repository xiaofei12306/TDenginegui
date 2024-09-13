#include "treeviewwithmenu.h"
#include "login.h"
#include "tdclient.h"
#include "tooltipwidget.h"

#include <QApplication>
#include <QClipboard>
#include <QMenu>
#include <QStandardItemModel>
#include <TableTabWidget.h>

void removeAllChildren(QStandardItem *item);

TreeViewWithMenu::TreeViewWithMenu(QWidget *parent)
{
    // 连接右击事件
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QTreeView::customContextMenuRequested, this, &TreeViewWithMenu::showContextMenu);
}

CustomTabWidget *TreeViewWithMenu::getTabWidget() const
{
    return tabWidget;
}

void TreeViewWithMenu::setTabWidget(CustomTabWidget *newTabWidget)
{
    tabWidget = newTabWidget;
}

void TreeViewWithMenu::showContextMenu(const QPoint &pos)
{
    // 获取点击位置的全局坐标
    const QPoint globalPos = mapToGlobal(pos);
    // 创建菜单
    QMenu contextMenu(this);
    // 获取点击位置对应的索引
    QModelIndex index = indexAt(pos);

    QStandardItem *item = dynamic_cast<QStandardItemModel *>(this->model())->itemFromIndex(index);
    TreeItemModel itemData = item->data(Qt::UserRole).value<TreeItemModel>();
    switch (itemData.getLevel()) {
    case CLIENT:
    {
        // 添加菜单项
        QAction *openClient = contextMenu.addAction("打开连接");
        QAction *closeClient = contextMenu.addAction("关闭连接");
        contextMenu.addSeparator();
        QAction *editClient = contextMenu.addAction("编辑连接");
        QAction *removeClient = contextMenu.addAction("删除连接");
        // 连接菜单项的触发信号
        connect(openClient, &QAction::triggered, this, [this, item, &itemData, index]() {
            handleClientDoubleClicked(item, itemData);
            this->setExpanded(index, true);
        });
        connect(closeClient, &QAction::triggered, this, [this, index, item, &itemData]() {
            handleCloseClicked(index, item, itemData);
        });
        connect(removeClient, &QAction::triggered, this, [this, index, item, &itemData](){
            handleRemoveClicked(index, item, itemData);
        });
        connect(editClient, &QAction::triggered, this, [this, index, item, itemData](){
            handleEditClicked(index, item, itemData);
        });
    }
    break;
    case DATABASE:
    {
        QAction *openDataBase = contextMenu.addAction("打开数据库");
        QAction *closeDataBase = contextMenu.addAction("关闭数据库");
        contextMenu.addSeparator();
        QAction *refreshDataBase= contextMenu.addAction("刷新");
        QAction *createSqlAction = contextMenu.addAction("获取创建数据库sql");
        QAction *removeDataBase = contextMenu.addAction("删除数据库");
        connect(openDataBase, &QAction::triggered, this, [this, item](){
            handleDatabaseDoubleClicked(item);
        });
        connect(closeDataBase, &QAction::triggered, this, [this, index, item](){
            handleCloseDataBase(index, item);
        });
        connect(removeDataBase, &QAction::triggered, this, [this, index, item, itemData](){
            handleRemoveDataBase(index, item, itemData);
        });
        connect(refreshDataBase, &QAction::triggered, this, [this, item](){
            removeAllChildren(item);
            handleDatabaseDoubleClicked(item);
        });
        connect(createSqlAction, &QAction::triggered, this, [this, item, itemData](){
            hanldeGetCreateSql(item, itemData.getLevel());
        });
    }
    break;
    case TABLE:
    case SUBTABLE:
    {
        QAction *createSqlAction = contextMenu.addAction("获取创建数据库sql");
        connect(createSqlAction, &QAction::triggered, this, [this, item, itemData](){
            hanldeGetCreateSql(item, itemData.getLevel());
        });
    }
        break;
    default:
        break;
    }
    // 显示菜单
    contextMenu.exec(globalPos);
}

void TreeViewWithMenu::handleClientDoubleClicked(QStandardItem *item, TreeItemModel &itemData)
{
    if (item->rowCount() > 0) {
        return;
    }
    QVariant data = itemData.getValue();
    if (data.isValid()) {
        sqlClient clientData = data.value<sqlClient>();
        TAOS *taos = testClient(clientData);
        clientData.setTaos(taos);
        itemData.setValue(QVariant::fromValue(clientData));
        item->setData(QVariant::fromValue(itemData), Qt::UserRole);
        item->setIcon(QIcon(":/icon/icon/openTDengine.png"));
        getDataBase(taos, item);
    } else {
        qDebug() << "Failed to retrieve client data.";
    }
}

void TreeViewWithMenu::handleDatabaseDoubleClicked(QStandardItem *item)
{
    if (item->rowCount() > 0) {
        return;
    }
    QStandardItem *clientItem = item->parent();
    sqlClient clientData = clientItem->data(Qt::UserRole).value<TreeItemModel>().getValue().value<sqlClient>();
    getTables(clientData.getTaos(), item);
    item->setIcon(QIcon(":/icon/icon/openDataBase.png"));
}

void TreeViewWithMenu::handleTableDoubleClicked(QStandardItem *item)
{
    QStandardItem *dataBaseItem = item->parent();
    QStandardItem *clientItem = dataBaseItem->parent();
    QString tabTitle = item->text() + "@" + dataBaseItem->text() + "(" + clientItem->text() + ")";
    int index = tabWidget->findTabByTitle(tabTitle);
    if (index > -1) {
        tabWidget->setCurrentIndex(index);
        return;
    }
    TreeItemModel itemMode = item->data(Qt::UserRole).value<TreeItemModel>();
    sqlClient clientData = clientItem->data(Qt::UserRole).value<TreeItemModel>().getValue().value<sqlClient>();

    // 创建 TableTabWidget 实例并添加到 tabWidget
    TableTabWidget *tableTabWidget = new TableTabWidget(clientData,  itemMode.getLevel(), dataBaseItem->text(), item->text(), tabWidget);
    tabWidget->addTab(tableTabWidget, tabTitle);
    tabWidget->setCurrentIndex(tabWidget->count() - 1);
}

void TreeViewWithMenu::handleCloseClicked(QModelIndex index, QStandardItem *item, TreeItemModel itemData)
{
    this->setExpanded(index, false);
    removeAllChildren(item);
    item->setIcon(QIcon(":/icon/icon/closeTDengine.png"));
    QVariant data = itemData.getValue();
    if (data.isValid()) {
        sqlClient clientData = data.value<sqlClient>();
        handleCloseTabWidget(clientData.getClientName());
        if(clientData.getTaos() != nullptr){
            taos_close(clientData.getTaos());
            clientData.setTaos(nullptr);
            itemData.setValue(QVariant::fromValue(clientData));
            item->setData(QVariant::fromValue(itemData), Qt::UserRole);
        }
    }
}

void TreeViewWithMenu::handleRemoveClicked(QModelIndex index, QStandardItem *item, TreeItemModel itemData)
{
    int result = QMessageBox::warning(
        this,
        "警告",
        "你确定要删除\"" % item->text() % "\"吗?",
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
        );
    if(result == QMessageBox::Yes){
        removeAllChildren(item);
        this->model()->removeRow(index.row());
        QVariant data = itemData.getValue();
        if (data.isValid()) {
            sqlClient clientData = data.value<sqlClient>();
            handleCloseTabWidget(clientData.getClientName());
            if(clientData.getTaos() != nullptr){
                taos_close(clientData.getTaos());
                clientData.setTaos(nullptr);
            }
        }
    }
}

void TreeViewWithMenu::handleEditClicked(QModelIndex index, QStandardItem *item, TreeItemModel itemData)
{
    QVariant data = itemData.getValue();
    if (data.isValid()) {
        sqlClient clientData = data.value<sqlClient>();
        if(clientData.getTaos() != nullptr){
            int result = QMessageBox::question(
                this,
                "询问",
                "必须关闭服务器连接，需要关闭吗？",
                QMessageBox::Yes | QMessageBox::No,
                QMessageBox::No
                );
            if (result == QMessageBox::Yes) {
                handleCloseClicked(index, item, itemData);
                login *loginWindow = new login(item);
                connect(loginWindow, &login::editClient, this, &TreeViewWithMenu::onEditSqlClient);
                loginWindow->show();
            }
        } else {
            login *loginWindow = new login(item);
            connect(loginWindow, &login::editClient, this, &TreeViewWithMenu::onEditSqlClient);
            loginWindow->show();
        }
    }
}

void TreeViewWithMenu::handleCloseTabWidget(QString clientName)
{
    for (int i = 0; i < tabWidget->count(); ++i) {
        QWidget *widget = tabWidget->widget(i); // 获取当前索引下的QWidget
        if(widget->property("clientName") == clientName){
            tabWidget->closeTab(i);
            i--;
        }
    }
}

void TreeViewWithMenu::handleCloseDataBase(QModelIndex index, QStandardItem *item)
{
    for (int i = 0; i < tabWidget->count(); ++i) {
        QString dataBase = "@" % item->text() % "(" % item->parent()->text() % ")";
        if(tabWidget->tabText(i).endsWith(dataBase)){
            tabWidget->closeTab(i);
            i--;
        }
    }
    this->setExpanded(index, false);
    removeAllChildren(item);
    item->setIcon(QIcon(":/icon/icon/closeDataBase.png"));
}

void TreeViewWithMenu::handleRemoveDataBase(QModelIndex index, QStandardItem *item, TreeItemModel itemData)
{
    int result = QMessageBox::warning(
        this,
        "警告",
        "你确定要删除\"" % item->text() % "\"吗?",
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
        );
    if(result == QMessageBox::Yes){
        handleCloseDataBase(index, item);
        TreeItemModel parentItemData = item->parent()->data(Qt::UserRole).value<TreeItemModel>();
        QVariant data = parentItemData.getValue();
        if (data.isValid()) {
            sqlClient clientData = data.value<sqlClient>();
            if(clientData.getTaos() != nullptr){
                TaosExecuteResult taosResult = deleteDataBase(item->text(), clientData.getTaos());
                if(!taosResult.getErrorMsg().isNull()){
                    QMessageBox::critical(
                        this,
                        "错误",
                        taosResult.getErrorMsg()
                        );
                    return;
                }
                item->parent()->removeRow(item->row());
            }
        }
    }
}

void TreeViewWithMenu::hanldeGetCreateSql(QStandardItem *item, Level level)
{
    TaosExecuteResult taosResult;
    switch (level) {
    case DATABASE:
    {
        QStandardItem *parentItem = item->parent();
        sqlClient clientData = parentItem->data(Qt::UserRole).value<TreeItemModel>().getValue().value<sqlClient>();
        taosResult = getDataBaseCreateSql(item->text(), clientData.getTaos());
    }
        break;
    case TABLE:
    {
        QStandardItem *dataBaseItem =  item->parent();
        QStandardItem *clientItem = dataBaseItem->parent();
        sqlClient clientData = clientItem->data(Qt::UserRole).value<TreeItemModel>().getValue().value<sqlClient>();
        taosResult = getSuperTableCreateSql(dataBaseItem->text() % "." % item->text(), clientData.getTaos());
    }
        break;
    default:
        break;
    }

    if(!taosResult.getErrorMsg().isNull()){
        QMessageBox::critical(
            this,
            "错误",
            taosResult.getErrorMsg()
            );
        return;
    }
    if(taosResult.getRowData().isEmpty() || taosResult.getRowData().size() != 1
        || taosResult.getRowData()[0].isEmpty() || taosResult.getRowData()[0].size() != 2){
        QMessageBox::critical(
            this,
            "错误",
            "数据解析异常"
            );
        return;
    }
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText( taosResult.getRowData()[0][1]);
    TooltipWidget *tipWidget = new TooltipWidget(QApplication::activeWindow());
    tipWidget->showTooltip("已经复制到粘贴板");
}

void TreeViewWithMenu::onTreeViewDoubleClicked(const QModelIndex &index)
{
    if (!index.isValid()) {
        qDebug() << "Invalid index selected.";
        return;
    }

    QStandardItem *item = dynamic_cast<QStandardItemModel *>(this->model())->itemFromIndex(index);
    TreeItemModel itemData = item->data(Qt::UserRole).value<TreeItemModel>();
    switch (itemData.getLevel()) {
    case CLIENT:
        handleClientDoubleClicked(item, itemData);
        break;
    case DATABASE:
        handleDatabaseDoubleClicked(item);
        break;
    case TABLE:
    case SUBTABLE:
        handleTableDoubleClicked(item);
        break;
    default:
        break;
    }
    this->setExpanded(index, true);
}

void TreeViewWithMenu::onEditSqlClient(QStandardItem *item, sqlClient sqlClient)
{
    TreeItemModel itemData = item->data(Qt::UserRole).value<TreeItemModel>();
    itemData.setValue(QVariant::fromValue(sqlClient));
    item->setData(QVariant::fromValue(itemData), Qt::UserRole);
    item->setData(sqlClient.getClientName(), Qt::DisplayRole);
}

// 删除 QStandardItem 下面的所有子项
void removeAllChildren(QStandardItem *item)
{
    if (!item) {
        return;
    }

    // 获取子项的数量
    int childCount = item->rowCount();

    // 从最后一个子项开始删除，避免删除过程中影响索引
    for (int i = childCount - 1; i >= 0; --i) {
        QStandardItem *childItem = item->child(i);
        if (childItem) {
            // 递归删除子项的子项
            removeAllChildren(childItem);

            // 从父项中移除子项
            item->removeRow(i);
        }
    }
}
