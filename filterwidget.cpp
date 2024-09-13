#include "filterwidget.h"

#include "addfilterwidget.h"

#include <QComboBox>
#include <QDateTimeEdit>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMenu>
#include <QStandardItem>
#include <QToolButton>
#include <taosfield.h>

FilterWidget::FilterWidget() {}

FilterWidget::FilterWidget(QListWidgetItem *item, QTableView *tableView, QWidget *parent): QWidget(parent), listItem(item), tableView(tableView)
{
    queryList = qobject_cast<QListWidget *>(parent);
    comBox = new QComboBox(this);
    for (int col = 0; col < tableView->model()->columnCount(); ++col) {
        QVariant headerData = tableView->model()->headerData(col, Qt::Horizontal, Qt::DisplayRole);
        comBox->addItem(headerData.toString());
        comBox->setItemData(col, tableView->model()->headerData(col, Qt::Horizontal, Qt::UserRole));
    }
    comBox->setGeometry(0, 1, 70, 20);
    comBox->setCurrentIndex(-1);
    connect(comBox, &QComboBox::activated, this, &FilterWidget::onFieldActivated);

    button = new QToolButton(this);
    button->setText("=");
    button->setGeometry(75, 1, 25, 20);
    button->setPopupMode(QToolButton::InstantPopup);
    // 创建一个 QMenu 实例
    QMenu *menu = new QMenu(button);

    // 向菜单中添加 QAction 项
    menu->addAction(">");
    menu->addAction(">=");
    menu->addAction("=");
    menu->addAction("<");
    menu->addAction("<=");
    button->setMenu(menu);
    connect(menu, &QMenu::triggered, [this](QAction *action){
        button->setText(action->text());
    });

    addFiltetButton =  new QToolButton(this);
    addFiltetButton->setIcon(QIcon(":/icon/icon/addFilter.png"));
    addFiltetButton->setGeometry(105, 1, 20, 20);
    addFiltetButton->setStyleSheet("QToolButton { border: none; }");
    connect(addFiltetButton, &QToolButton::clicked, this, &FilterWidget::onAddFilterClicked);

    subtractFiltetButton =  new QToolButton(this);
    subtractFiltetButton->setIcon(QIcon(":/icon/icon/subtractFilter.png"));
    subtractFiltetButton->setGeometry(addFiltetButton->geometry().left() + addFiltetButton->geometry().width() + 5, 1, 20, 20);
    subtractFiltetButton->setStyleSheet("QToolButton { border: none; }");
    connect(subtractFiltetButton, &QToolButton::clicked, this, &FilterWidget::onSubtractFilterClicked);
}

void FilterWidget::onSubtractFilterClicked()
{
    // 获取当前项的小部件
    QWidget *widget = queryList->itemWidget(listItem);
    queryList->removeItemWidget(listItem);
    if (widget != nullptr) {
        widget->deleteLater();
    }
    if (listItem != nullptr) {
        delete listItem;
    }
    if(queryList->model()->rowCount() == 0){
        queryList->addItem(new QListWidgetItem(queryList));
        AddFilterWidget *itemWidget = new AddFilterWidget(tableView, queryList);
        QModelIndex index = queryList->model()->index(0, 0);
        queryList->setIndexWidget(index, itemWidget);
    }
}

void FilterWidget::onAddFilterClicked()
{
    QListWidgetItem *item = new QListWidgetItem(queryList);
    queryList->addItem(item);
    FilterWidget *itemWidget = new FilterWidget(item, tableView, queryList);
    QModelIndex index = queryList->model()->index(queryList->model()->rowCount() - 1, 0);
    queryList->setIndexWidget(index, itemWidget);
}

void FilterWidget::onFieldActivated(int index)
{
    TaosField taosField = comBox->currentData(Qt::UserRole).value<TaosField>();
    QDateTimeEdit *dateTime = this->findChild<QDateTimeEdit *>("dateTime");
    if(dateTime != nullptr) dateTime->deleteLater();
    QLineEdit *input = this->findChild<QLineEdit *>("input");
    if(input != nullptr) input->deleteLater();
    if (taosField.getType() == TSDB_DATA_TYPE_TIMESTAMP) {
        QDateTimeEdit *dateTime = new QDateTimeEdit(this);
        dateTime->setDate(QDate::currentDate());
        dateTime->setObjectName("dateTime");
        dateTime->setDisplayFormat("yyyy-MM-dd HH:mm:ss"); // 设置显示格式
        dateTime->setCalendarPopup(true); // 使用弹出式日历
        dateTime->setGeometry(105, 1, 145, 20);
        dateTime->show();
        addFiltetButton->move(dateTime->geometry().width() + dateTime->geometry().left() + 5, 0);
        subtractFiltetButton->move(addFiltetButton->geometry().width() + addFiltetButton->geometry().left() + 5, 0);
    } else {
        QLineEdit *input = new QLineEdit(this);
        input->setObjectName("input");
        input->setPlaceholderText("<?>");
        input->show();
        input->setStyleSheet("QLineEdit { border: none; color: blue; border-bottom: 1px solid blue;  background-color: transparent;"
                             "text-align: center;"
                             " } "
                             "QLineEdit::placeholder {"
                             "   text-align: center;"
                             "}");
        input->setGeometry(105, 1, 80, 20);
        addFiltetButton->move(input->geometry().width() + input->geometry().left() + 5, 0);
        subtractFiltetButton->move(addFiltetButton->geometry().width() + addFiltetButton->geometry().left() + 5, 0);
    }
}

QComboBox *FilterWidget::getComBox() const
{
    return comBox;
}

void FilterWidget::setComBox(QComboBox *newComBox)
{
    comBox = newComBox;
}

QToolButton *FilterWidget::getButton() const
{
    return button;
}

void FilterWidget::setButton(QToolButton *newButton)
{
    button = newButton;
}
