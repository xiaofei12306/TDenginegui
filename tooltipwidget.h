#ifndef TOOLTIPWIDGET_H
#define TOOLTIPWIDGET_H

#include <QLabel>
#include <QPropertyAnimation>
#include <QWidget>

class TooltipWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TooltipWidget(QWidget *parent = nullptr);
    void showTooltip(const QString &message);

signals:
private:
    QLabel *label;
    QPropertyAnimation *animation;
    void setupUI();
    void setupAnimation();
    void startAnimation();
    void updatePosition();
};

#endif // TOOLTIPWIDGET_H
