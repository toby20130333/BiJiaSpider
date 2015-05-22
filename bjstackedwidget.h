#ifndef BJSTACKEDWIDGET_H
#define BJSTACKEDWIDGET_H

#include <QStackedWidget>
#include "bjwidget.h"
#include "chat/bjchatwidget.h"

namespace Ui {
class BJStackedWidget;
}

class BJStackedWidget : public QStackedWidget
{
    Q_OBJECT

public:
    explicit BJStackedWidget(QWidget *parent = 0);
    ~BJStackedWidget();
    BJWidget* bjWidget;
    BJChatWidget* bjChatWid;
private:
    Ui::BJStackedWidget *ui;
};

#endif // BJSTACKEDWIDGET_H
