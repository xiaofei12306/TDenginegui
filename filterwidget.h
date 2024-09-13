#ifndef FILTERWIDGET_H
#define FILTERWIDGET_H

#include <QComboBox>
#include <QListWidgetItem>
#include <QTableWidget>
#include <QWidget>


class FilterWidget : public QWidget
{
    Q_OBJECT
public:
    FilterWidget();
    FilterWidget(QListWidgetItem *item, QTableView *tableView, QWidget *parent = nullptr);

    QToolButton *getButton() const;
    void setButton(QToolButton *newButton);

    QComboBox *getComBox() const;
    void setComBox(QComboBox *newComBox);

private slots:
    void onSubtractFilterClicked();
    void onAddFilterClicked();
    void onFieldActivated(int index);

private:
    QListWidgetItem *listItem;
    QListWidget *queryList;
    QTableView *tableView;
    QToolButton *button;
    QToolButton *addFiltetButton;
    QToolButton *subtractFiltetButton;
    QComboBox *comBox;

};

#endif // FILTERWIDGET_H
