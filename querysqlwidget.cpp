#include "querysqlwidget.h"

#include "sqlClient.h"
#include "tdclient.h"
#include "treeitemmodel.h"

#include <QToolButton>
#include <QTextEdit>
#include <QWidget>
#include <QStandardItemModel>
#include <QTableView>
#include <QHeaderView>
#include <QDateTime>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolBar>
#include <QComboBox>
#include <QTabWidget>
#include <QTextEdit>
#include <QStandardItemModel>

QuerySqlWidget::QuerySqlWidget(QAbstractItemModel *itemModel)
    : textEdit(new QTextEdit(this)),
    button(new QToolButton(this)),
    clientComboBox(new QComboBox(this)),
    dataBaseComboBox(new QComboBox(this)),
    toolBar(new QToolBar(this)),
    vBox(new QVBoxLayout(this)),
    highlighter(new SqlHighlighter(textEdit->document())),
    toolWidget(new QWidget(this)),
    originalTaos(nullptr),
    tabWidget(nullptr),
    font(QFont("仿宋体", 10))
{
    textEdit->setFont(font);

    button->setEnabled(false);
    button->setText("运行");
    button->setIcon(QIcon(":/icon/icon/run.png"));
    button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    button->setStyleSheet(
        "QToolButton {"
        " border: none;"
        " padding-left: 10px;"
        " padding-right: 10px;"
        "}"
        "QToolButton:hover {"
        " background-color: #F0F0F0;"
        "}"
        "QToolButton:pressed {"
        " background-color: #D0D0D0;"
        "}"
        );

    connect(button, &QToolButton::clicked, this, &QuerySqlWidget::executeQuery);

    setupClientComboBox(itemModel);
    hBox = new QHBoxLayout(toolWidget);
    hBox->addWidget(clientComboBox, 2);
    hBox->addWidget(dataBaseComboBox, 2);
    hBox->addWidget(button, 1);
    hBox->addStretch(8);

    toolWidget->setLayout(hBox);
    toolBar->addWidget(toolWidget);

    vBox->setSpacing(0);
    vBox->setContentsMargins(0, 0, 0, 0);
    vBox->addWidget(toolBar, 1);
    vBox->addWidget(textEdit, 3);

    setLayout(vBox);

    connect(clientComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &QuerySqlWidget::onCurrentIndexChanged);
}

QuerySqlWidget::~QuerySqlWidget()
{
    if (originalTaos != nullptr) {
        taos_close(originalTaos);
    }
    if (tabWidget != nullptr) {
        tabWidget->deleteLater();
    }
}

void QuerySqlWidget::setupClientComboBox(QAbstractItemModel *itemModel)
{
    auto model = dynamic_cast<QStandardItemModel*>(itemModel);
    QModelIndex rootIndex = model->invisibleRootItem()->index();
    for (int row = 0; row < model->rowCount(rootIndex); ++row) {
        QModelIndex index = model->index(row, 0, rootIndex);
        QString text = model->data(index).toString();
        clientComboBox->addItem(QIcon(":/icon/icon/openTDengine.png"), text, model->data(index, Qt::UserRole).value<TreeItemModel>().getValue());
    }
    clientComboBox->setCurrentIndex(-1);
}

void QuerySqlWidget::executeQuery()
{
    if (tabWidget) {
        vBox->removeWidget(tabWidget);
        tabWidget->deleteLater();
        tabWidget = nullptr;
    }

    QVariant data = clientComboBox->currentData(Qt::UserRole);
    if (!data.isValid() || !data.canConvert<sqlClient>()) {
        return;
    }

    QString sql = !textEdit->textCursor().selectedText().isEmpty() ? textEdit->textCursor().selectedText() : textEdit->toPlainText();
    if (sql.isEmpty()) {
        return;
    }
    sql = sql.trimmed();
    sqlClient clientData = data.value<sqlClient>();
    TaosExecuteResult taosResult = syncExecute(sql, dataBaseComboBox->currentText(), clientData.getTaos());

    QTextEdit *infoTextEdit = new QTextEdit(tabWidget);
    infoTextEdit->setFont(font);
    infoTextEdit->append(sql);

    if (taosResult.getErrorMsg().isEmpty()) {
        infoTextEdit->append(isQuerySql(sql) ? "> ok" : "> row: " + QString::number(taosResult.getRow()));
    } else {
        infoTextEdit->append("> " + taosResult.getErrorMsg());
    }
    infoTextEdit->append("> 耗时: " + QString::number(taosResult.getConsumingTime()) + "ms");

    tabWidget = new QTabWidget();
    tabWidget->addTab(infoTextEdit, "信息");

    if (isQuerySql(sql) && taosResult.getErrorMsg().isEmpty()) {
        QTableView *tableView = new QTableView(tabWidget);
        tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        taosResult.renderingTable(tableView);
        tabWidget->addTab(tableView, "结果");
        tabWidget->setCurrentIndex(1);
    } else {
        tabWidget->addTab(new QWidget(), "结果");
    }

    vBox->addWidget(tabWidget, 2);
}

void QuerySqlWidget::onCurrentIndexChanged(int index)
{
    this->setProperty("clientName", clientComboBox->currentText());
    button->setEnabled(true);
    if (originalTaos) {
        taos_close(originalTaos);
        originalTaos = nullptr;
    }
    QVariant data = clientComboBox->currentData(Qt::UserRole);
    if (data.isValid() && data.canConvert<sqlClient>()) {
        sqlClient clientData = data.value<sqlClient>();
        TAOS *taos = testClient(clientData);
        originalTaos = taos;
        clientData.setTaos(taos);
        clientComboBox->setItemData(index, QVariant::fromValue(clientData), Qt::UserRole);

        dataBaseComboBox->clear();
        dataBaseComboBox->addItems(getDataBase(taos));
        for (int i = 0; i < dataBaseComboBox->count(); ++i) {
            dataBaseComboBox->setItemIcon(i, QIcon(":/icon/icon/openDataBase.png"));
        }
    }
}
