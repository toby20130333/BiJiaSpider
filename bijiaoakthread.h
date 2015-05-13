#ifndef BIJIAOAKTHREAD_H
#define BIJIAOAKTHREAD_H

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
#define OAKCHEMIL_URL    "https://www.oakwoodchemical.com/ProductsList.aspx?txtSearch="

class BiJiaOAKThread : public QThread
{
    Q_OBJECT
public:
    explicit BiJiaOAKThread(QObject *parent = 0);
    ~BiJiaOAKThread();
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

#endif // BIJIAOAKTHREAD_H
