#ifndef QUERYSQLWIDGET_H
#define QUERYSQLWIDGET_H

#include "sqlhighlighter.h"
#include <QComboBox>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QToolBar>
#include <QWidget>
#include <TAOS.h>

class QuerySqlWidget: public QWidget
{
    Q_OBJECT
public:
    explicit QuerySqlWidget(QAbstractItemModel *itemModel);
    ~QuerySqlWidget();
private:
    QTextEdit *textEdit;
    QToolButton *button;
    QComboBox *clientComboBox;
    QComboBox *dataBaseComboBox;
    QToolBar *toolBar;
    QVBoxLayout *vBox;
    QHBoxLayout *hBox;
    SqlHighlighter *highlighter;
    QWidget *toolWidget;
    TAOS *originalTaos = nullptr;
    QTabWidget *tabWidget = nullptr;
    QFont font;
    void setupClientComboBox(QAbstractItemModel *itemModel);
private slots:
    void onCurrentIndexChanged(int index);
    void executeQuery();

};

#endif // QUERYSQLWIDGET_H
