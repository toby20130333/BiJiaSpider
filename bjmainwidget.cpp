#include "bjmainwidget.h"
#include "ui_bjmainwidget.h"

BJMainWidget::BJMainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BJMainWidget),bjWidget(NULL),bjChatWid(NULL),stackedWidget(NULL)
{
    ui->setupUi(this);
    //初始化为未按下鼠标左键
    mouse_press = false;
#if defined(Q_OS_WIN)
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    //    ui->closeBtn->setVisible(false);
#endif
#if defined(Q_OS_OSX)
    //    ui->closeBtn->setVisible(false);
#endif

    QFile qss(":/bjmain.qss");
    qss.open(QFile::ReadOnly);
    this->setStyleSheet(qss.readAll());
    qss.close();

    bjChatWid = new BJChatWidget(this);
    bjWidget = new BJWidget(this);
    stackedWidget = new QTabWidget(this);
    closeBtn = new QToolButton(this);
//    closeBtn->setDefaultAction(new QAction("X",0));
    closeBtn->setObjectName("closetool");
    closeBtn->setAutoRaise(true);
    closeBtn->resize(27,22);
    closeBtn->setToolButtonStyle(Qt::ToolButtonIconOnly);
    connect(closeBtn,SIGNAL(clicked()),qApp,SLOT(quit()));

    stackedWidget->addTab(bjChatWid,tr("局域网IM"));
    stackedWidget->addTab(bjWidget,tr("比价"));
    stackedWidget->setCornerWidget(closeBtn, Qt::TopRightCorner);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(stackedWidget);
    layout->setSpacing(0);
    layout->setContentsMargins(5,5,5,5);
    setLayout(layout);
    this->show();
}

BJMainWidget::~BJMainWidget()
{
    delete ui;
}
void BJMainWidget::paintEvent(QPaintEvent *e)
{
#if defined(Q_OS_WIN)
    QPainterPath path;
    path.setFillRule(Qt::WindingFill);
    int rate = 5;
    path.addRect(rate, rate, this->width()-rate*2, this->height()-rate*2);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillPath(path, QBrush(Qt::white));

    QColor color(0, 0, 0, 50);
    for(int i=0; i<rate; i++)
    {
        QPainterPath path;
        path.setFillRule(Qt::WindingFill);
        path.addRect(rate-i, rate-i, this->width()-(rate-i)*2, this->height()-(rate-i)*2);
        color.setAlpha(150 - qSqrt(i)*50);
        painter.setPen(color);
        painter.drawPath(path);
    }
#endif
    QWidget::paintEvent(e);
}

void BJMainWidget::mousePressEvent(QMouseEvent *event)
{
    //只能是鼠标左键移动和改变大小
    if(event->button() == Qt::LeftButton)
    {
        mouse_press = true;
    }

    //窗口移动距离
    move_point = event->globalPos() - pos();
}

void BJMainWidget::mouseReleaseEvent(QMouseEvent *)
{
    mouse_press = false;
}

void BJMainWidget::mouseMoveEvent(QMouseEvent *event)
{
    //移动窗口
    if(mouse_press)
    {
        QPoint move_pos = event->globalPos();
        move(move_pos - move_point);
    }
}
