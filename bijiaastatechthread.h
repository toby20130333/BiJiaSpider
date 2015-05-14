#ifndef BIJIAASTATECHTHREAD_H
#define BIJIAASTATECHTHREAD_H

#include <QObject>
#include <QThread>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QFile>
#include <QDebug>
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QUrlQuery>
#else
#include <QtGui>
#endif
#define ASTATECH_URL    "http://astatech.com.cn/english/index.php/Search/index/category/0/kw/"
#define END_URL            "/search_type/1/type/goods/p/0/psize/0/price/0.html"

class BiJiaAstatechThread : public QThread
{
    Q_OBJECT
public:
    explicit BiJiaAstatechThread(QObject *parent = 0);
    ~BiJiaAstatechThread();
    QNetworkAccessManager* manager;
    void setCode(const QString& code);
    void startSplider(const QUrl &url);
    void secondStart(const QUrl &url);
    void initManager();

    QString m_Code;
    bool isFirst;
signals:
    void signalSendFinalData(const QByteArray& arr,bool isFirst);
    void signalStart();
    void signalMessageShow(const QString& msg);
public slots:
    void replyFinished(QNetworkReply*);
    void slotStart();
protected:
    void run();
};

#endif // BIJIAASTATECHTHREAD_H
