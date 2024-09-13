#ifndef CUSTOMTABBAR_H
#define CUSTOMTABBAR_H

#include <QTabBar>

class CustomTabBar : public QTabBar
{
    Q_OBJECT
public:
    explicit CustomTabBar(QWidget *parent = nullptr);

protected:
    QSize tabSizeHint(int index) const override;
};

#endif // CUSTOMTABBAR_H
