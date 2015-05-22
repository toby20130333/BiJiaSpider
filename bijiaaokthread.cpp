#include "BiJiaAOKThread.h"
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))

#else
#define QStringLiteral(str)  QString::fromUtf8(str)
#endif
BiJiaAOKThread::BiJiaAOKThread(QObject *parent) :
    QThread(parent),isFirst(true),manager(NULL)
{
  connect(this,SIGNAL(finished()),this,SLOT(deleteLater()));
}

BiJiaAOKThread::~BiJiaAOKThread()
{
    if(manager){
        manager->deleteLater();
        manager = NULL;
    }
}

void BiJiaAOKThread::setCode(const QString &code)
{
    m_Code = code;
}

void BiJiaAOKThread::startSplider(const QUrl& url)
{
    qDebug()<<QStringLiteral("ARKFM开始请求数据:请求地址为: ")<<url;
//    manager->get(QNetworkRequest(url));
    /*
     * submit2.x:26
submit2.y:8
submit2:psearch
*/
    QByteArray content;
    content.clear();
    content.append("keytype2=0&product=");
    content.append(m_Code);
    content.append("&submit2.x=26&submit2.y=8&submit2=psearch");
    int contentLength = content.length();
    QNetworkRequest req;
    QUrl url2(url);
    req.setUrl(QUrl(url2));
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
    req.setHeader(QNetworkRequest::ContentLengthHeader,contentLength);
    qDebug()<<QStringLiteral(" AOK Post的内容为:  ")<<content;
    manager->post(req,content);
}

void BiJiaAOKThread::initManager()
{
    connect(this,SIGNAL(signalStart()),this,SLOT(slotStart()));
}

void BiJiaAOKThread::replyFinished(QNetworkReply *reply)
{
    QByteArray arr = reply->readAll();
    //qDebug()<<" BiJiaAOKThread replyFinished "<<arr;
    //网页重定向
    QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    qDebug()<<"BiJiaAOKThread redirectionTarget "<<redirectionTarget;
    if(reply->error() ==QNetworkReply::NoError ){
        emit signalSendFinalData(arr);
    }
}

void BiJiaAOKThread::slotStart()
{
    if(manager == NULL){
        manager = new QNetworkAccessManager(0);
       connect(manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));
    }
    QString url = AOK_URL;
    startSplider(QUrl(url));
}

void BiJiaAOKThread::run()
{
    initManager();
    emit signalStart();
    exec();
}

