#ifndef SYSYTEMTRAY_H
#define SYSYTEMTRAY_H

#include <QWidget>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QApplication>
#include <QWidgetAction>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QPainter>
#include <QDesktopServices>
#include <QUrl>
class SystemTray : public QSystemTrayIcon
{
	Q_OBJECT

public:

	explicit SystemTray(QWidget *parent = 0);
	~SystemTray();
	void translateLanguage();

protected:

	bool eventFilter(QObject *obj, QEvent *event);

signals:

	void showWidget();
public slots:
    void slotSetTrayVisible(bool);
    void slotTrayDownload();
private:
	void createAction();
	void addActions();

private:

	QMenu *tray_menu; //托盘菜单

	QWidget *top_widget;  //顶部菜单项
	QWidgetAction *top_widget_action;  //播放模式：上一首、播放/暂停、下一首
    QLabel *protect_label;  //已保护电脑...天
    QPushButton *open_button;  //木马防火墙已经开启
    QPushButton *enter_button;  //进入
	
	QAction *action_open; //打开360安全卫士
	QAction *action_safe_notice; //安全通知
};

#endif //SYSYTEMTRAY_H
