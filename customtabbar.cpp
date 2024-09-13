#include "customtabbar.h"

CustomTabBar::CustomTabBar(QWidget *parent)
    : QTabBar(parent)
{}

QSize CustomTabBar::tabSizeHint(int index) const
{
    // 获取当前可用的总宽度
    int totalWidth = this->width();

    // 获取所有标签的个数
    int numTabs = this->count();

    // 计算每个标签的理想宽度
    int idealWidth = totalWidth / numTabs;

    // 获取原始的大小提示
    QSize originalSize = QTabBar::tabSizeHint(index);

    // 如果理想宽度小于原始宽度，则使用理想宽度
    int width = qMin(idealWidth, originalSize.width());

    // 返回新的大小提示
    return QSize(width, originalSize.height());
}
