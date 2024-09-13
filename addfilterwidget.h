#ifndef ADDFILTERWIDGET_H
#define ADDFILTERWIDGET_H

#include <QObject>
#include <QTableWidget>
#include <QWidget>

class AddFilterWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AddFilterWidget(QTableView *tableView, QWidget *parent = nullptr);
private:
    QTableView *tableView;
signals:
};

#endif // ADDFILTERWIDGET_H
