#include "tooltipwidget.h"
#include <QScreen.h>

#include <QApplication>
#include <QTimer>
#include <QVBoxLayout>

TooltipWidget::TooltipWidget(QWidget *parent)
    : QWidget(parent), animation(new QPropertyAnimation(this, "geometry"))
{
    setupUI();
    setupAnimation();
}
void TooltipWidget::showTooltip(const QString &message) {
    label->setText(message);
    startAnimation();
    this->show();
}

void TooltipWidget::setupUI() {
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_DeleteOnClose);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    label = new QLabel(this);
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("background-color: #333; color: white; padding: 10px; border-radius: 5px;");

    layout->addWidget(label);

    // 初始位置设为应用程序窗口中间底部
    updatePosition();
}

void TooltipWidget::setupAnimation() {
    animation->setDuration(500);
    animation->setStartValue(QRect(x(), y(), width(), height()));
    animation->setEndValue(QRect(x(), y() - height(), width(), height()));
    animation->setEasingCurve(QEasingCurve::OutQuad);
}

void TooltipWidget::startAnimation() {
    animation->start();
    QTimer::singleShot(animation->duration() + 2000, this, &QWidget::close); // 关闭提示窗口
}


void TooltipWidget::updatePosition() {
    // 计算应用程序窗口的中心位置
    QWidget *parentWidget1 = parentWidget();
    if (parentWidget1) {
        QRect parentGeometry = parentWidget1->geometry();
        int parentX = parentGeometry.x() + parentGeometry.width() / 2 - width() / 2;
        int parentY = parentGeometry.y() + parentGeometry.height() - this->height();

        move(parentX, parentY);
    }
}
