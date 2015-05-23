#ifndef BIJIAARKTHREAD_H
#define BIJIAARKTHREAD_H

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
#define ARKPHARM_URL   ""
#define ARKFINAL_URL    ""

class BiJiaARKThread : public QThread
{
    Q_OBJECT
public:
    explicit BiJiaARKThread(QObject *parent = 0);
    ~BiJiaARKThread();
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
