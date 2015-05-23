#ifndef BIJIAAOKTHREAD_H
#define BIJIAAOKTHREAD_H

#include <QObject>
#include <QThread>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QDebug>
#include <QtGlobal>
#include <QFile>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QUrlQuery>
#else
#include <QtGui>
#endif
#define AOK_URL   ""
#define AOKFINAL_URL    ""

class BiJiaAOKThread : public QThread
{
    Q_OBJECT
public:
    explicit BiJiaAOKThread(QObject *parent = 0);
    ~BiJiaAOKThread();
    QNetworkAccessManager* manager;
    void setCode(const QString& code);
    void startSplider(const QUrl &url);
    void initManager();
    QString m_Code;
    bool isFirst;
signals:
    void signalSendFinalData(const QByteArray& arr);
    void signalStart();
public slots:
    void replyFinished(QNetworkReply*);
    void slotStart();
protected:
    void run();
};

#endif // BIJIAARKTHREAD_H
