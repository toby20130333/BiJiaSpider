#ifndef BJCHATWIDGET_H
#define BJCHATWIDGET_H

#include <QWidget>
#include <QtNetwork>
#include <QtGui>
#include <QMessageBox>
#include <QtWidgets>
#include <QHostAddress>
#include "tcpclient.h"
#include "tcpserver.h"
#include "chat.h"

namespace Ui {
 class BJChatWidget;
}

class BJChatWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BJChatWidget(QWidget *parent = 0);
    ~BJChatWidget();
    QString getUserName();
    QString getMessage();
    chat* privatechat;
    chat* privatechat1;

protected:
    void changeEvent(QEvent *e);
    void sendMessage(MessageType type,QString serverAddress="");
    void newParticipant(QString userName,QString localHostName,QString ipAddress);
    void participantLeft(QString userName,QString localHostName,QString time);
    void closeEvent(QCloseEvent *);
    void hasPendingFile(QString userName,QString serverAddress,
                        QString clientAddress,QString fileName);

     bool eventFilter(QObject *target, QEvent *event);//事件过滤器
private:
    QUdpSocket *udpSocket;
    qint32 port;
    qint32 bb;
    QString fileName;
    TcpServer *server;
    //chat *privatechat;

    QString getIP();

    QColor color;//颜色

    bool saveFile(const QString& fileName);//保存聊天记录
    void showxchat(QString name, QString ip);

private slots:
    void on_tableWidget_doubleClicked(QModelIndex index);
    void on_textUnderline_clicked(bool checked);
    void on_clear_clicked();
    void on_save_clicked();
    void on_textcolor_clicked();
    void on_textitalic_clicked(bool checked);
    void on_textbold_clicked(bool checked);
    void on_fontComboBox_currentFontChanged(QFont f);
    void on_fontsizecomboBox_currentIndexChanged(QString );
    void on_close_clicked();
    void on_sendfile_clicked();
    void on_send_clicked();
    void processPendingDatagrams();
    void sentFileName(QString);
    void currentFormatChanged(const QTextCharFormat &format);
private:
    Ui::BJChatWidget *ui;
};

#endif // BJCHATWIDGET_H
