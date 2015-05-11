#ifndef BIJIACOMBTHREAD_H
#define BIJIACOMBTHREAD_H

#include <QObject>
#include <QThread>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QDebug>
#include <QFile>
#include <QVariant>
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QUrlQuery>
#else
#include <QtGui>
#endif

#define COMBLOCK_URL     "https://www.combi-blocks.com/cgi-bin/cbp.cgi"
#define COMBLOCK_UURL   "http://www.combi-blocks.com/cgi-bin/find.cgi"

class BiJiaCOMBThread : public QThread
{
    Q_OBJECT
public:
    explicit BiJiaCOMBThread(QObject *parent = 0);
    ~BiJiaCOMBThread();
    QNetworkAccessManager* manager;
    void setCode(const QString& code);
    void startSplider(const QUrl &url);
    void startEnd();
    QString m_Code;
    bool isFirst;
    QByteArray cookie;
signals:
    void signalStart();
    void signalSendFinalData(const QByteArray& arr,bool isFirst);
public slots:
    void replyFinished(QNetworkReply*);
    void initManager();
    void slotStart();
protected:
    void run();
};

#endif // BIJIACOMBTHREAD_H
