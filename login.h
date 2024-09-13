#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include <MainWindow.h>
#include "sqlClient.h"
#include <QKeyEvent>

namespace Ui {
class login;
}

class login : public QWidget
{
    Q_OBJECT

public:
    explicit login(QWidget *parent = nullptr);
    explicit login(QStandardItem *item, QWidget *parent = nullptr);
    ~login();

signals:
    void newClinet(sqlClient newClient);
    void editClient(QStandardItem *item, sqlClient newClient);

private slots:
    void on_pushButton_clicked();

    void on_sureButton_clicked();

    void on_cancelButton_clicked();

private:
    Ui::login *ui;
    QStandardItem *item = nullptr;
    void setupUi();

protected:
    void keyPressEvent(QKeyEvent *event) override;
};

#endif // LOGIN_H
