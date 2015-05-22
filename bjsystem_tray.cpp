#include "bjsystem_tray.h"

SystemTray::SystemTray(QWidget *parent)
    : QSystemTrayIcon(parent)
{
    this->createAction();
    this->addActions();
    this->translateLanguage();
}

SystemTray::~SystemTray()
{

}

void SystemTray::translateLanguage()
{
    //放在托盘图标上时候显示
    this->setToolTip(tr("比价工具"));
    action_open->setText(tr("检测更新"));
#if defined(Q_OS_WIN)
    action_safe_notice->setText(tr("Mac版本下载"));
#endif
#if defined(Q_OS_OSX)
    action_safe_notice->setText(tr("Windows版本下载"));
#endif
}
void SystemTray::createAction()
{
    this->setIcon(QIcon(":/logo.png"));
    tray_menu = new QMenu();
    action_open = new QAction(this);
    action_open->setIcon(QIcon(":/updater.png"));
    action_safe_notice = new QAction(this);
    action_safe_notice->setIcon(QIcon(":/download.png"));
    tray_menu->setFixedWidth(140);
    connect(action_open, SIGNAL(triggered()), this, SIGNAL(showWidget()));
    connect(action_safe_notice, SIGNAL(triggered()), this, SLOT(slotTrayDownload()));

    this->setContextMenu(tray_menu);
}

void SystemTray::addActions()
{
    //添加菜单项
    tray_menu->addAction(action_open);
    tray_menu->addSeparator();
    tray_menu->addAction(action_safe_notice);
}

bool SystemTray::eventFilter(QObject *obj, QEvent *event)
{
    return QSystemTrayIcon::eventFilter(obj, event);
}

void SystemTray::slotSetTrayVisible(bool Visible)
{
    this->setVisible(Visible);
}

void SystemTray::slotTrayDownload()
{
#if defined(Q_OS_WIN)
    QDesktopServices::openUrl(QUrl("http://qtclub.qiniudn.com/Windows.zip"));
#endif
#if defined(Q_OS_OSX)
    QDesktopServices::openUrl(QUrl("http://qtclub.qiniudn.com/MacOSX.zip"));
#endif
}

