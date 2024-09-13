#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "treeitemmodel.h"
#include <QListWidget>
#include <QMainWindow>
#include <QStandardItem>
#include <QTableView>
#include <QTextEdit>
#include <QVBoxLayout>
#include <sqlClient.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    bool findTabByTitle(const QString &title);
    void initQueryTableToolButtons(QToolBar *toolBar);
    void refreshTableEvent();
    void initQueryAction(QAction *query);
    void setupActions();
    void setupTreeView();
    void setupConnections();

private slots:
    void on_tabWidget_tabCloseRequested(int index);
    void onClientActionTriggered();
    void onNewClient(sqlClient newClient);
    void onAboutActionTriggered();
private:
    void saveLoad();
    void readLoad();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
