#ifndef BJMAINWIDGET_H
#define BJMAINWIDGET_H

#include <QWidget>
#include "bjwidget.h"
#include "chat/bjchatwidget.h"
#include <QToolButton>

namespace Ui {
class BJMainWidget;
}

class BJMainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BJMainWidget(QWidget *parent = 0);
    ~BJMainWidget();
    BJWidget* bjWidget;
    BJChatWidget* bjChatWid;
    QTabWidget *stackedWidget;
    QToolButton* closeBtn;
    QPoint move_point; //移动的距离
    bool mouse_press; //按下鼠标左键
private:
    Ui::BJMainWidget *ui;
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    virtual void paintEvent(QPaintEvent *event);
    void closeEvent(QCloseEvent* e);
signals:
    void closeApp();
};

#endif // BJMAINWIDGET_H
