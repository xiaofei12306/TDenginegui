#ifndef CUSTOMHEADERVIEW_H
#define CUSTOMHEADERVIEW_H

#include <QHeaderView>
#include <QPainter>

class CustomHeaderView : public QHeaderView
{
    Q_OBJECT
public:
    CustomHeaderView(Qt::Orientation orientation, QWidget *parent = nullptr);
signals:
    void requestContextMenu();

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
};

#endif // CUSTOMHEADERVIEW_H
