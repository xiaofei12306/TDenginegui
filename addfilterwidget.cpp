#include "addfilterwidget.h"

#include "filterwidget.h"

#include <QLabel>
#include <QListWidget>
#include <QToolButton>

AddFilterWidget::AddFilterWidget(QTableView *tableView, QWidget *parent)
    : QWidget{parent}, tableView{tableView}
{
    QToolButton *addFiltetButton =  new QToolButton(this);
    addFiltetButton->setIcon(QIcon(":/icon/icon/addFilter.png"));
    addFiltetButton->setGeometry(5, 1, 20, 20);
    addFiltetButton->setStyleSheet("QToolButton { border: none; }");
    QLabel *tipLable = new QLabel("点击\"+\"号添加筛选", this);
    tipLable->setGeometry(addFiltetButton->geometry().left() + addFiltetButton->width() + 5, 1, 150, 20);
    tipLable->setStyleSheet("color: gray;");
    QListWidget *queryList = qobject_cast<QListWidget *>(parent);
    connect(addFiltetButton, &QToolButton::clicked, [queryList, tableView](){
        QListWidgetItem *item = queryList->takeItem(queryList->count() - 1);
        if (item != nullptr) {
            delete item;
        }
        QListWidgetItem *filterItem = new QListWidgetItem();
        queryList->addItem(filterItem);
        QModelIndex index = queryList->model()->index(queryList->count() - 1, 0);
        FilterWidget *filterWidget = new FilterWidget(filterItem, tableView, queryList);
        queryList->setIndexWidget(index, filterWidget);
    });
}
