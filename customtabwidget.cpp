#include "customtabwidget.h"

#include <QApplication>
#include <QMenu>
#include <QTabBar>

CustomTabWidget::CustomTabWidget(QWidget *parent)
    : QTabWidget(parent)
{
    connect(tabBar(), &QTabBar::tabBarClicked, this, &CustomTabWidget::onTabBarClicked);
}

int CustomTabWidget::findTabByTitle(const QString &title)
{
    for (int i = 0; i < count(); ++i) {
        if (tabText(i) == title) {
            return i;
        }
    }
    return -1;
}

void CustomTabWidget::onTabBarClicked(int index)
{
    // 检查是否是右键点击
    if (QApplication::mouseButtons() & Qt::RightButton) {
        // 创建一个上下文菜单
        QMenu contextMenu;
        QAction *closeTabAction = contextMenu.addAction("关闭");
        contextMenu.addSeparator();
        QAction *closeOtherTabsAction = contextMenu.addAction("关闭其他");
        QAction *closeAllTabsAction = contextMenu.addAction("关闭全部");

        // 连接槽函数
        connect(closeTabAction, &QAction::triggered, this, [this, index](){
            closeTab(index);
        });
        connect(closeOtherTabsAction, &QAction::triggered, this, [this, index](){
            closeOtherTabs(index);
        });
        connect(closeAllTabsAction, &QAction::triggered, this, &CustomTabWidget::closeAllTabs);

        // 显示上下文菜单
        contextMenu.exec(QCursor::pos());
    }
}

void CustomTabWidget::closeTab(int index)
{
    QWidget *widget = this->widget(index);
    removeTab(index);
    widget->deleteLater();
}

void CustomTabWidget::closeOtherTabs(int currentIndex)
{
    while (currentIndex != count() - 1) {
        QWidget *widget = this->widget(currentIndex + 1);
        removeTab(currentIndex + 1);
        widget->deleteLater();
    }
    while (currentIndex != 0) {
        QWidget *widget = this->widget(currentIndex - 1);
        removeTab(currentIndex - 1);
        widget->deleteLater();
        currentIndex--;
    }
}

void CustomTabWidget::closeAllTabs()
{
    while (count() > 0) {
        QWidget *widget = this->widget(0);
        removeTab(0);
        widget->deleteLater();
    }
}
