#ifndef CUSTOMTABWIDGET_H
#define CUSTOMTABWIDGET_H

#include <QTabWidget>

class CustomTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    CustomTabWidget(QWidget *parent = nullptr);
    int findTabByTitle(const QString &title);

private slots:
    void onTabBarClicked(int index);

public slots:
    void closeTab(int index);
    void closeOtherTabs(int currentIndex);
    void closeAllTabs();
};

#endif // CUSTOMTABWIDGET_H
