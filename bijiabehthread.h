#ifndef BIJIABEHTHREAD_H
#define BIJIABEHTHREAD_H

#include <QObject>
#include <QThread>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QDebug>
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QUrlQuery>
#else
#include <QtGui>
#endif
#define BEPHARM_URL    ""

class BiJiaBehThread : public QThread
{
    Q_OBJECT
public:
    explicit BiJiaBehThread(QObject *parent = 0);
    ~BiJiaBehThread();
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

#endif // BIJIABEHTHREAD_H
