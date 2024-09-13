#include "login.h"
#include <QMessageBox>
#include "tdclient.h"
#include "ui_login.h"

login::login(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::login)
{
    setupUi();
}

login::login(QStandardItem *item, QWidget *parent): QWidget(parent)
    , ui(new Ui::login), item(item)
{
    setupUi();
    TreeItemModel itemData = item->data(Qt::UserRole).value<TreeItemModel>();
    sqlClient editClient = itemData.getValue().value<sqlClient>();
    ui->clientName->setText(editClient.getClientName());
    ui->username->setText(editClient.getUsername());
    ui->ip->setText(editClient.getIp());
    ui->port->setText(QString::number(editClient.getPort()));
    ui->password->setText(editClient.getPassword());
}

login::~login()
{
    delete ui;
}

void login::on_pushButton_clicked()
{
    sqlClient loginInfo = sqlClient(ui->clientName->text(),
                                    ui->ip->text(),
                                    ui->port->text().toInt(),
                                    ui->username->text(),
                                    ui->password->text());
    TAOS *taos = testClient(loginInfo);
    taos_close(taos);
    if (taos != nullptr) {
        ui->line->setStyleSheet("QFrame { background-color: green; solid green;}");
        QMessageBox::information(this, "提示", "连接成功");
    } else {
        ui->line->setStyleSheet("QFrame { background-color: red;  solid red;}");
        QMessageBox::critical(this,
                              "错误",
                              "连接失败:" + QString::fromStdString(string(taos_errstr(taos))));
    }
}

void login::on_sureButton_clicked()
{
    sqlClient loginInfo = sqlClient(ui->clientName->text(),
                                    ui->ip->text(),
                                    ui->port->text().toInt(),
                                    ui->username->text(),
                                    ui->password->text());
    if(item != nullptr){
        emit editClient(item, loginInfo);
    } else{
       emit newClinet(loginInfo);
    }
    this->close();
}

void login::on_cancelButton_clicked()
{
    this->close();
}

void login::setupUi()
{
    ui->setupUi(this);
    this->setWindowTitle("TDengine 连接");
    this->setLayout(ui->formLayout);
    ui->line->setStyleSheet(
        "QFrame { background-color: green; border-width: 2px; border-style: solid; }");
}

void login::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        close();
    } else {
        // 调用基类的方法处理其他按键
        QWidget::keyPressEvent(event);
    }
}
