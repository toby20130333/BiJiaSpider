#include "bjstackedwidget.h"
#include "ui_bjstackedwidget.h"

BJStackedWidget::BJStackedWidget(QWidget *parent) :
    QStackedWidget(parent),
    ui(new Ui::BJStackedWidget),bjWidget(NULL),bjChatWid(NULL)
{
    ui->setupUi(this);
    bjChatWid = new BJChatWidget(this);
    bjWidget = new BJWidget(this);
    this->addWidget(bjWidget);
    this->addWidget(bjChatWid);
    this->setCurrentIndex(0);
    this->show();
}

BJStackedWidget::~BJStackedWidget()
{
    delete ui;
}
