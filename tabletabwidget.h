// TableTabWidget.h
#ifndef TABLETABWIDGET_H
#define TABLETABWIDGET_H

#include <QWidget>
#include <QStandardItem>
#include <QVBoxLayout>
#include <QTableView>
#include <QToolBar>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include "sqlClient.h"
#include "treeitemmodel.h"

class TableTabWidget : public QWidget {
    Q_OBJECT

public:
    TableTabWidget(sqlClient &clientData, Level level, QString dataBaseName, QString tableName, QWidget *parent = nullptr);

private:
    void setupUI(sqlClient &clientData);
    void setupFilterButton(QToolBar *topToolBar, QVBoxLayout *vBox);
    void initQueryTableToolButtons(QToolBar *toolBar);
    void refreshTable(QList<QString> conditionList);
    void handleFilterButtonClicked(QVBoxLayout *vbox);
    void setupApplyToolBar(QToolBar *applyToolBar, QListWidget *queryList);
    void moveQueryListItem(QListWidget *queryList, int direction);
    void refreshTableEvent2(QList<QString> conditionList, QPair<QString, QString> orderPair);
    void removeQueryListAndToolBar(QVBoxLayout *vBox);
    void applyFilterQuery();
    void handleQueryTableButtonClicked(QToolButton *queryTableButton);

    QTableView *tableView;
    QLineEdit *pageEdit;
    QToolBar *toolBar;
    QTabWidget *tabWidget;
    QToolButton *filterButton;
    QVBoxLayout *vBox;
    bool isHandlingSort = false;
    QString dataBaseName;
    QString tableName;
    Level level;
    QString filterSql;

private slots:
    void onFirstButtonClicked();
    void onLeftButtonClicked();
    void onRightButtonClicked();
    void onEndButtonClicked();
    void onSubtractButtonClicked();
    void onSubTableDataRequestMenu(const QPoint &pos);
    void onHelpButtonClicked();
};

#endif // TABLETABWIDGET_H
