// TableTabWidget.cpp
#include <QWidget>
#include <QStandardItem>
#include <QVBoxLayout>
#include <QTableView>
#include <QToolBar>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QToolButton>
#include <QDateTimeEdit>
#include <QPushButton>
#include <QTimer>
#include <QHeaderView>
#include <QMenu>
#include <Taos.h>
#include "addfilterwidget.h"
#include "customitemdelegate.h"
#include "filterwidget.h"
#include "sqlClient.h"
#include "tdclient.h"
#include "taosexecuteresult.h"
#include "tabletabwidget.h"
#include "customheaderview.h"
#include <QThread>
optional<QPair<QString, QString>> splitTabTitle(QString string);
const QString BUTTON_STYLE = "QPushButton { border: none; }"
                             "QPushButton:hover { background-color: lightgray; }"
                             "QPushButton:pressed { background-color: gray; }";
const QString HELP_TIP = "由于tdengine默认返回的数据是无序的，所以使用分页功能请先选择排序，这样分页功能才能生效";
TableTabWidget::TableTabWidget(sqlClient &clientData, Level level, QString dataBaseName, QString tableName, QWidget *parent)
    : QWidget(parent), dataBaseName(dataBaseName), tableName(tableName), level(level)
{
    tabWidget = qobject_cast<QTabWidget *>(parent);
    setupUI(clientData);
}

void TableTabWidget::setupUI(sqlClient &clientData) {
    this->setProperty("clientName", clientData.getClientName());
    vBox = new QVBoxLayout(this);
    vBox->setObjectName("vBox");


    tableView = new QTableView(this);

    // 设置自定义表头视图
    CustomHeaderView *header = new CustomHeaderView(Qt::Horizontal, tableView);
    tableView->setHorizontalHeader(header);
    tableView->horizontalHeader()->setSortIndicator(-1,  Qt::AscendingOrder);
    tableView->horizontalHeader()->setSectionsMovable(true);
    tableView->horizontalHeader()->setSortIndicatorShown(true);
    tableView->setSortingEnabled(true);
    tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(header, &CustomHeaderView::requestContextMenu, this, &TableTabWidget::applyFilterQuery);
    connect(header, &CustomHeaderView::sortIndicatorChanged, this, &TableTabWidget::applyFilterQuery);

    tableView->setStyleSheet("QHeaderView::section");
    tableView->setObjectName("tableView");

    QToolBar *topToolBar = new QToolBar(this);
    setupFilterButton(topToolBar, vBox);

    toolBar = new QToolBar(this);
    initQueryTableToolButtons(toolBar);

    vBox->addWidget(topToolBar);
    vBox->addWidget(tableView, 5);
    vBox->addWidget(toolBar);
    vBox->setSpacing(0);
    vBox->setContentsMargins(0, 0, 0, 0);
    setLayout(vBox);

    this->setProperty("client", QVariant::fromValue(clientData.getTaos()));
    if(clientData.getTaos() == nullptr){
        QMessageBox::critical(this,
                              "错误",
                              "连接失败:" + QString::fromStdString(string(taos_errstr(clientData.getTaos()))));
        return;
    }

    if (tableView->model() != nullptr) {
        tableView->model()->removeColumns(0, tableView->model()->columnCount());
        tableView->model()->removeRows(0, tableView->model()->rowCount());
    }
    QLineEdit *edit = toolBar->findChild<QLineEdit *>("pageEdit");
    QString sql = "select * from " % dataBaseName % "." + tableName;

    sql.append(" limit ");
    sql.append(to_string((edit->text().toUInt() - 1) * 1000));
    sql.append(",1000");
    TaosExecuteResult taosResult = syncExecute(sql, nullptr, clientData.getTaos());
    taosResult.renderingTable(tableView);
}

void TableTabWidget::setupFilterButton(QToolBar *topToolBar, QVBoxLayout *vBox) {
    filterButton = new QToolButton(topToolBar);
    filterButton->setText("筛选");
    filterButton->setIcon(QIcon(":/icon/icon/sift.png"));
    filterButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    connect(filterButton, &QToolButton::clicked, [vBox, this]() {
        handleFilterButtonClicked(vBox);
    });
    topToolBar->addWidget(filterButton);
    if (level == Level::TABLE) {
        QToolButton *queryTableButton = new QToolButton(topToolBar);
        queryTableButton->setText("查看子表");
        queryTableButton->setIcon(QIcon(":/icon/icon/queryTable.png"));
        queryTableButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        topToolBar->addWidget(queryTableButton);
        connect(queryTableButton, &QToolButton::clicked, [this, queryTableButton](){
            handleQueryTableButtonClicked(queryTableButton);
        });
    }
    QToolButton *helpButton = new QToolButton(topToolBar);
    helpButton->setText("帮助");
    helpButton->setIcon(QIcon(":/icon/icon/help.png"));
    helpButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    helpButton->setToolTip(HELP_TIP);
    connect(helpButton, &QToolButton::clicked, this, &TableTabWidget::onHelpButtonClicked);
    topToolBar->addWidget(helpButton);
}

void TableTabWidget::initQueryTableToolButtons(QToolBar *toolBar) {
    QWidget *widget = new QWidget(toolBar);
    widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    widget->setMaximumHeight(40);
    QHBoxLayout *hBox = new QHBoxLayout(widget);
    hBox->setContentsMargins(0,0,0,0);

    QPushButton *addButton = new QPushButton(widget);
    addButton->setIcon(QIcon(":/icon/icon/add.png"));
    addButton->setFlat(true);
    addButton->setStyleSheet(BUTTON_STYLE);
    addButton->setEnabled(false);
    hBox->addWidget(addButton);

    QPushButton *subtractButton = new QPushButton(widget);
    subtractButton->setIcon(QIcon(":/icon/icon/subtract.png"));
    subtractButton->setFlat(true);
    subtractButton->setStyleSheet(BUTTON_STYLE);
    hBox->addWidget(subtractButton);
    connect(subtractButton, &QPushButton::clicked, this, &TableTabWidget::onSubtractButtonClicked);

    QPushButton *refreshButton = new QPushButton(widget);
    refreshButton->setIcon(QIcon(":/icon/icon/refresh.png"));
    refreshButton->setFlat(true);
    refreshButton->setStyleSheet(BUTTON_STYLE);
    hBox->addWidget(refreshButton);
    connect(refreshButton, &QPushButton::clicked, this, &TableTabWidget::applyFilterQuery);

    QSpacerItem *spacer = new QSpacerItem(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QWidget *spacerWidget = new QWidget(widget);
    spacerWidget->setLayout(new QVBoxLayout(spacerWidget));
    spacerWidget->layout()->addItem(spacer);
    hBox->addWidget(spacerWidget);

    QPushButton *firstButton = new QPushButton(widget);
    firstButton->setIcon(QIcon(":/icon/icon/leftLeft.png"));
    firstButton->setFlat(true);
    firstButton->setStyleSheet(BUTTON_STYLE);
    hBox->addWidget(firstButton);
    connect(firstButton, &QPushButton::clicked, this, &TableTabWidget::onFirstButtonClicked);

    QPushButton *leftButton = new QPushButton(widget);
    leftButton->setIcon(QIcon(":/icon/icon/left.png"));
    leftButton->setFlat(true);
    leftButton->setStyleSheet(BUTTON_STYLE);
    hBox->addWidget(leftButton);
    connect(leftButton, &QPushButton::clicked, this, &TableTabWidget::onLeftButtonClicked);

    QLineEdit *page = new QLineEdit("1", widget);
    page->setObjectName("pageEdit");
    page->setMaximumWidth(20);
    page->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    page->setAlignment(Qt::AlignCenter);
    QIntValidator *intValidator = new QIntValidator(page);
    intValidator->setBottom(0);
    page->setValidator(intValidator);
    hBox->addWidget(page);

    QPushButton *rightButton = new QPushButton(widget);
    rightButton->setIcon(QIcon(":/icon/icon/right.png"));
    rightButton->setFlat(true);
    rightButton->setStyleSheet(BUTTON_STYLE);
    hBox->addWidget(rightButton);
    connect(rightButton, &QPushButton::clicked, this, &TableTabWidget::onRightButtonClicked);

    QPushButton *endButton = new QPushButton(widget);
    endButton->setIcon(QIcon(":/icon/icon/rightRight.png"));
    endButton->setFlat(true);
    endButton->setStyleSheet(BUTTON_STYLE);
    hBox->addWidget(endButton);
    connect(endButton, &QPushButton::clicked, this, &TableTabWidget::onEndButtonClicked);

    hBox->setStretchFactor(spacerWidget, 2);
    widget->setLayout(hBox);
    toolBar->addWidget(widget);
}

void TableTabWidget::refreshTable(QList<QString> conditionList) {
    applyFilterQuery();
}

void TableTabWidget::handleFilterButtonClicked(QVBoxLayout *vBox)
{
    if (!filterButton->property("isHover").isValid()) {
        filterButton->setProperty("isHover", QVariant::fromValue(true));
        filterButton->setStyleSheet("QToolButton { background: #D3D3D3; }");

        QListWidget *queryList = new QListWidget(this);
        CustomItemDelegate *delegate = new CustomItemDelegate();
        queryList->setItemDelegate(delegate);
        queryList->addItem(new QListWidgetItem(queryList));
        AddFilterWidget *itemWidget = new AddFilterWidget(tableView, queryList);
        QModelIndex index = queryList->model()->index(0, 0);
        queryList->setIndexWidget(index, itemWidget);

        vBox->insertWidget(1, queryList, 1);
        QToolBar *applyToolBar = new QToolBar(queryList);
        setupApplyToolBar(applyToolBar, queryList);
        vBox->insertWidget(2, applyToolBar);
    } else if (filterButton->property("isHover").toBool()) {
        filterButton->setStyleSheet("");
        removeQueryListAndToolBar(vBox);
        filterButton->setProperty("isHover", QVariant());
    }
}

void TableTabWidget::setupApplyToolBar(QToolBar *applyToolBar, QListWidget *queryList)
{
    applyToolBar->setIconSize(QSize(16, 16));
    QAction *upAction = new QAction(QIcon(":/icon/icon/up.png"), "向上", applyToolBar);
    connect(upAction, &QAction::triggered, [queryList, this]() { moveQueryListItem(queryList, -1); });
    QAction *downAction = new QAction(QIcon(":/icon/icon/down.png"), "向下", applyToolBar);
    connect(downAction, &QAction::triggered, [queryList, this]() { moveQueryListItem(queryList, 2); });
    // QAction *applyAction = new QAction(QIcon(":/icon/icon/dui.png"), "应用", applyToolBar);
    // connect(applyAction, &QAction::triggered, [this](){
    //     applyFilterQuery();
    // });
    applyToolBar->addAction(upAction);
    applyToolBar->addAction(downAction);
    // applyToolBar->addAction(applyAction);
}

void TableTabWidget::moveQueryListItem(QListWidget *queryList, int direction)
{
    int row = queryList->currentRow();
    int newRow = row + direction;
    if(direction == -1 && newRow < 0){
        return;
    }else if(newRow >= queryList->count() + 1){
        return;
    }
    QListWidgetItem *item = queryList->currentItem();
    if (!item) return;
    QWidget *widget = queryList->indexWidget(queryList->model()->index(row, 0));
    QListWidgetItem *newItem = new QListWidgetItem(*item);
    queryList->insertItem(newRow, newItem);
    QModelIndex newIndex = queryList->model()->index(newRow, 0);
    queryList->setIndexWidget(newIndex, widget);
    queryList->setCurrentItem(newItem);
    delete item;
}

void TableTabWidget::applyFilterQuery()
{
    if (isHandlingSort) {
        return; // 防止递归触发
    }
    isHandlingSort = true;

    QList<QString> conditionList;
    if (filterButton->property("isHover").isValid() && filterButton->property("isHover").toBool()) {
        QListWidget *queryList = qobject_cast<QListWidget *>(vBox->itemAt(1)->widget());
        for (int i = 0; i < queryList->count(); ++i) {
            QModelIndex index =  queryList->model()->index(i, 0);
            FilterWidget *widget = dynamic_cast<FilterWidget *>(queryList->indexWidget(index));
            if(widget == nullptr){
                break;
            }
            QString conditionString = widget->getComBox()->currentText().append(widget->getButton()->text());
            QDateTimeEdit *dateTime = queryList->findChild<QDateTimeEdit *>("dateTime");
            if(dateTime != nullptr) conditionString.append("'" % dateTime->text() % "'");
            QLineEdit *input = queryList->findChild<QLineEdit *>("input");
            if(input != nullptr) conditionString.append("'" % input->text() % "'");
            conditionList<< conditionString;
        }
    }
    if(!filterSql.isNull()){
        conditionList << filterSql;
    }
    QPair<QString,QString> oderPair;
    QHeaderView *header = tableView->horizontalHeader();
    int logicalIndex = header->sortIndicatorSection();
    Qt::SortOrder order = header->sortIndicatorOrder();
    if (logicalIndex >= 0) {
        // 获取排序列的标题
        QString headerText = tableView->model()->headerData(logicalIndex, Qt::Horizontal, Qt::DisplayRole).toString();
        oderPair.first = headerText;
        oderPair.second = order == Qt::AscendingOrder ? "asc" : "desc";
    }
    refreshTableEvent2(conditionList, oderPair);
}

void TableTabWidget::handleQueryTableButtonClicked(QToolButton *queryTableButton)
{
    QLineEdit *edit = toolBar->findChild<QLineEdit *>("pageEdit");
    edit->setText("1");
    if (queryTableButton->text() == "查看超级表数据") {
        queryTableButton->setText("查看子表");
        disconnect(tableView, &QTableView::customContextMenuRequested, nullptr, nullptr);
        optional<QPair<QString, QString>> result = splitTabTitle(tabWidget->tabText(tabWidget->currentIndex()));
        this->filterSql = nullptr;
        this->dataBaseName = result->second;
        this->tableName = result->first;
        tableView->horizontalHeader()->setSortIndicator(-1,  Qt::AscendingOrder);
    } else {
        isHandlingSort = true;
        tableView->horizontalHeader()->setSortIndicator(-1,  Qt::AscendingOrder);
        isHandlingSort = false;
        queryTableButton->setText("查看超级表数据");
        this->filterSql = QString::fromStdString("stable_name = ") % "'" % this->tableName % "' and db_name = '" % this->dataBaseName % "'";
        this->dataBaseName = "information_schema";
        this->tableName = "ins_tables";

        refreshTable(QList<QString>());
        tableView->horizontalHeader()->setSortIndicator(0,  Qt::AscendingOrder);
        connect(tableView, &QTableView::customContextMenuRequested, this, &TableTabWidget::onSubTableDataRequestMenu);
    }
}


void TableTabWidget::refreshTableEvent2(QList<QString> conditionList, QPair<QString, QString> orderPair)
{
    QWidget *widget = tabWidget->currentWidget();
    TAOS *taos = widget->property("client").value<TAOS *>();
    if(taos == nullptr){
        QMessageBox::critical(this,
                              "错误",
                              "连接失败:" + QString::fromStdString(string(taos_errstr(taos))));
        return;
    }
    QTableView *tableView = widget->findChild<QTableView *>("tableView");
    if (tableView->model() != nullptr) {
        tableView->model()->removeColumns(0, tableView->model()->columnCount());
        tableView->model()->removeRows(0, tableView->model()->rowCount());
    }

    QTimer::singleShot(100, this, [this, conditionList, taos, tableView, orderPair]() {
        QLineEdit *edit = toolBar->findChild<QLineEdit *>("pageEdit");
        QString sql = "select * from " % dataBaseName % "." + tableName;
        if(!conditionList.isEmpty()){
            sql.append(" where ");
            for (int i = 0; i < conditionList.size(); ++i) {
                sql.append(conditionList.at(i).toStdString());
                if(i != conditionList.size() - 1){
                    sql.append("and");
                }
            }
        }
        if(!orderPair.first.isNull()){
            sql.append(" order by " % orderPair.first % " " % orderPair.second);
        }
        sql.append(" limit ");
        sql.append(to_string((edit->text().toUInt() - 1) * 1000));
        sql.append(",1000");
        TaosExecuteResult taosResult = syncExecute(sql, nullptr, taos);
        taosResult.renderingTable(tableView);
        if(!orderPair.first.isNull()){
            for (int i = 0; i < tableView->model()->columnCount(); ++i) {
                if(tableView->model()->headerData(i, Qt::Horizontal) == orderPair.first){
                    tableView->horizontalHeader()->setSortIndicator(i, orderPair.second == "asc" ? Qt::AscendingOrder : Qt::DescendingOrder);
                    break;
                }
            }
        }
        isHandlingSort = false;
    });
}

void TableTabWidget::removeQueryListAndToolBar(QVBoxLayout *vBox)
{
    QWidget *queryList = vBox->itemAt(1)->widget();
    if (queryList) {
        vBox->removeWidget(queryList);
        queryList->deleteLater();
    }

    QWidget *applyToolBar = vBox->itemAt(1)->widget();
    if (applyToolBar) {
        vBox->removeWidget(applyToolBar);
        applyToolBar->deleteLater();
    }
}

void TableTabWidget::onFirstButtonClicked()
{
    QLineEdit *edit = toolBar->findChild<QLineEdit *>("pageEdit");
    edit->setText("1");
    applyFilterQuery();
}

void TableTabWidget::onLeftButtonClicked()
{
    QLineEdit *edit = toolBar->findChild<QLineEdit *>("pageEdit");
    int page = edit->text().toInt();
    if (page <= 1) {
        return;
    }
    edit->setText(QString::number(page - 1));
    applyFilterQuery();
}


void TableTabWidget::onRightButtonClicked()
{
    QLineEdit *edit = toolBar->findChild<QLineEdit *>("pageEdit");
    edit->setText(QString::number(edit->text().toInt() + 1));
    applyFilterQuery();
}

void TableTabWidget::onEndButtonClicked()
{
    QWidget *currentWidget = tabWidget->currentWidget();
    QLineEdit *edit = toolBar->findChild<QLineEdit *>("pageEdit");
    TAOS *taos = currentWidget->property("client").value<TAOS *>();
    long count = getCount(taos, tableName.toStdString(), dataBaseName.toStdString());
    if(count == 0){
        edit->setText("1");
        return;
    }
    edit->setText(QString::number(ceil(count / 1000.0)));
    applyFilterQuery();
}

void TableTabWidget::onSubtractButtonClicked()
{
    QModelIndex currentModel = tableView->currentIndex();
    if(currentModel.row() > 0){
        int result = QMessageBox::information(
            this,
            "询问",
            "由于Tdengine是无序的，所以删除数据操作是根据所有的字段和内容，但还是可能会有删除重复数据的情况",
            QMessageBox::Yes | QMessageBox::No,
            QMessageBox::No
            );
        if (result == QMessageBox::Yes) {
            QAbstractItemModel *model = tableView->model();
            int columnCount = model->columnCount();
            QList<TaosField> headers;
            QStringList contentList;
            for (int i = 0; i < columnCount; ++i) {
                QVariant headerData = model->headerData(i, Qt::Horizontal, Qt::UserRole);
                if (headerData.isValid()) {
                    headers << headerData.value<TaosField>();
                }
                contentList << model->index(currentModel.row(), i).data().toString();
            }
            QWidget *currentWidget = tabWidget->currentWidget();
            TAOS *taos = currentWidget->property("client").value<TAOS *>();
            TaosExecuteResult taosResult = deleteRow(this->dataBaseName, this->tableName, contentList, headers, taos);
            if(taosResult.getErrorMsg().isNull() || taosResult.getErrorMsg().isEmpty()){
                applyFilterQuery();
            }else{
                QMessageBox::critical(this, "错误", taosResult.getErrorMsg());
            }
        }
    }
}

void TableTabWidget::onSubTableDataRequestMenu(const QPoint &pos)
{
    QMenu contextMenu(this);
    QModelIndex index = tableView->indexAt(pos);
    index = tableView->model()->index(index.row(), 0);
    QAction *querySubTableAction = contextMenu.addAction("查看子表数据");
    connect(querySubTableAction, &QAction::triggered, [index, this](){
        QString subTableName = tableView->model()->index(index.row(), 0).data().toString();
        // 创建 TableTabWidget 实例并添加到 tabWidget
        optional<QPair<QString, QString>> result = splitTabTitle(tabWidget->tabText(tabWidget->currentIndex()));
        QString tabTitle = subTableName + "@" + result->second + "(" + this->property("clientName").toString() + ")";
        sqlClient clientData;
        clientData.setClientName(this->property("clientName").toString());
        clientData.setTaos(this->property("client").value<TAOS*>());
        TableTabWidget *tableTabWidget = new TableTabWidget(clientData,  Level::SUBTABLE,
                                                            result->second, subTableName, tabWidget);
        tabWidget->addTab(tableTabWidget, tabTitle);
        tabWidget->setCurrentIndex(tabWidget->count() - 1);
    });
    contextMenu.exec(QCursor::pos());
}

void TableTabWidget::onHelpButtonClicked()
{
    QMessageBox::information(this, "提示", HELP_TIP);
}

optional<QPair<QString, QString>> splitTabTitle(QString string)
{
    QStringList parts = string.split('@');
    if (parts.size() != 2) {
        return std::nullopt;
    }

    QString tableName = parts[0];
    QStringList dbAndClient = parts[1].split('(');
    if (dbAndClient.size() != 2) {
        return std::nullopt;
    }

    QString dbName = dbAndClient[0];
    QString clientName = dbAndClient[1].remove(')');
    return qMakePair(tableName, dbName);
}
