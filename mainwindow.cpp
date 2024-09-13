#include "mainwindow.h"
#include <QComboBox>
#include <QItemDelegate>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPainter>
#include <QPushButton>
#include <QSettings>
#include <QSortFilterProxyModel>
#include <QSpinBox>
#include <QStandardItemModel>
#include <QStringListModel>
#include <QStyledItemDelegate>
#include <QTableView>
#include <QTextEdit>
#include <QToolButton>
#include "QAction"
#include "QTabBar"
#include "about.h"
#include "login.h"
#include "qdebug.h"
#include "querysqlwidget.h"
#include "treeitemmodel.h"
#include "ui_mainwindow.h"
#include <taos.h>
#include <QDateTimeEdit>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupActions();
    setupTreeView();
    setupConnections();
    readLoad();
    ui->treeView->setTabWidget(ui->tabWidget);
}

MainWindow::~MainWindow()
{
    saveLoad();
    delete ui;
}

void MainWindow::setupActions()
{
    QAction *client = new QAction(QIcon(":/icon/icon/client.png"), "连接");
    QAction *query = new QAction(QIcon(":/icon/icon/query.png"), "查询");
    QAction *aboutAction = new QAction(QIcon(":/icon/icon/about.png"), "关于");
    ui->toolBar->addAction(client);
    ui->toolBar->addAction(query);
    ui->toolBar->addAction(aboutAction);
    connect(client, &QAction::triggered, this, &MainWindow::onClientActionTriggered);
    connect(aboutAction, &QAction::triggered, this, &MainWindow::onAboutActionTriggered);
    initQueryAction(query);
}

void MainWindow::setupTreeView()
{
    QStandardItemModel *model = new QStandardItemModel(this);
    ui->treeView->setModel(model);
    ui->treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void MainWindow::setupConnections()
{
    connect(ui->treeView, &QTreeView::doubleClicked, ui->treeView, &TreeViewWithMenu::onTreeViewDoubleClicked);
}

void MainWindow::onClientActionTriggered()
{
    login *loginWindow = new login();
    loginWindow->show();
    connect(loginWindow, &login::newClinet, this, &MainWindow::onNewClient);
}

void MainWindow::onNewClient(sqlClient newClient)
{
    QStandardItemModel *model = dynamic_cast<QStandardItemModel *>(ui->treeView->model());
    for (int row = 0; row <  model->rowCount(); ++row) {
        if(model->item(row, 0)->text() == newClient.getClientName()){
            QMessageBox criticalBox;
            QMessageBox::critical(
                this,
                "错误",
                "连接名重复"
                );
            return;
        }
    }
    if (newClient.getPort() == 0){
        newClient.setPort(6030);
    }
    QStandardItem *childItem1 = new QStandardItem(newClient.getClientName());
    childItem1->setIcon(QIcon(":/icon/icon/closeTDengine.png"));
    TreeItemModel itemModel(Level::CLIENT, QVariant::fromValue(newClient));
    childItem1->setData(QVariant::fromValue(itemModel), Qt::UserRole);
    model->appendRow(childItem1);
}

void MainWindow::onAboutActionTriggered()
{
    About *aboutWidget = new About();
    aboutWidget->show();
}

void MainWindow::saveLoad()
{
    QString appPath = QCoreApplication::applicationDirPath();
    QString configFilePath = appPath + "/settings.ini";
    QSettings settings(configFilePath, QSettings::IniFormat);
    settings.beginGroup("MainWindow");
    auto model = dynamic_cast<QStandardItemModel*>(ui->treeView->model());
    QModelIndex rootIndex = model->invisibleRootItem()->index();
    QVariantList variantList;
    for (int row = 0; row < model->rowCount(rootIndex); ++row) {
        QModelIndex index = model->index(row, 0, rootIndex);
        QVariant data = model->data(index, Qt::UserRole);
        variantList <<  data.value<TreeItemModel>().toVariant();
    }
    settings.setValue("client", variantList);
    settings.endGroup(); // 结束组
}

void MainWindow::readLoad ()
{
    QStandardItemModel *model = dynamic_cast<QStandardItemModel *>(ui->treeView->model());
    QString appPath = QCoreApplication::applicationDirPath();
    QString configFilePath = appPath + "/settings.ini";
    QSettings settings(configFilePath, QSettings::IniFormat);
    settings.beginGroup("MainWindow");
    QVariantList variantList = settings.value("client").toList();
    for (QVariant config : variantList){
        TreeItemModel itemModel = TreeItemModel::fromVariant(config);
        sqlClient client = itemModel.getValue().value<sqlClient>();
        QStandardItem *childItem1 = new QStandardItem(client.getClientName());
        childItem1->setIcon(QIcon(":/icon/icon/closeTDengine.png"));
        childItem1->setData(QVariant::fromValue(itemModel), Qt::UserRole);
        model->appendRow(childItem1);
    }
    settings.endGroup(); // 结束组
}


void MainWindow::initQueryAction(QAction *query)
{
    connect(query, &QAction::triggered, [this]() {
        ui->tabWidget->addTab(new QuerySqlWidget(ui->treeView->model()), "新建查询");
        ui->tabWidget->setCurrentIndex(ui->tabWidget->count() - 1);
    });
}


void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    QWidget *widget = ui->tabWidget->widget(index);
    ui->tabWidget->removeTab(index);
    widget->deleteLater();
}
