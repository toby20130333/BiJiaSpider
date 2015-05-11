#include "bijiaarkthread.h"

BiJiaARKThread::BiJiaARKThread(QObject *parent) :
    QThread(parent),isFirst(true),manager(NULL)
{
  connect(this,SIGNAL(finished()),this,SLOT(deleteLater()));
}

BiJiaARKThread::~BiJiaARKThread()
{
    if(manager){
        manager->deleteLater();
        manager = NULL;
    }
}

void BiJiaARKThread::setCode(const QString &code)
{
    m_Code = code;
}

void BiJiaARKThread::startSplider(const QUrl& url)
{
    qDebug()<<" BiJiaARKThread startLoad "<<url;
    manager->get(QNetworkRequest(url));
}

void BiJiaARKThread::initManager()
{
    connect(this,SIGNAL(signalStart()),this,SLOT(slotStart()));
}

void BiJiaARKThread::replyFinished(QNetworkReply *reply)
{
    qDebug()<<" BiJiaARKThread replyFinished "<<reply->errorString();
    //网页重定向
    QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    qDebug()<<"BiJiaARKThread redirectionTarget "<<redirectionTarget;
    if (!redirectionTarget.isNull())
    {
        QUrl newUrl = reply->url().resolved(redirectionTarget.toUrl());
        QString realUrl;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        QUrlQuery query(newUrl);
         realUrl =query.queryItemValue("catalogno");
#else
         realUrl =newUrl.queryItemValue("catalogno");
#endif
        qDebug()<<" BiJiaARKThread redirectionTarget catalogno "<<realUrl;
        if(realUrl.isEmpty()){
            qDebug()<<" BiJiaARKThread redirectionTarget catalogno22222 "<<realUrl;
            return;
        }
        realUrl.prepend(ARKFINAL_URL);
        startSplider(realUrl);
        return ;
    }
    if(reply->error() ==QNetworkReply::NoError ){
        emit signalSendFinalData(reply->readAll());
    }
}

void BiJiaARKThread::slotStart()
{
    if(manager == NULL){
        manager = new QNetworkAccessManager(0);
       connect(manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));
    }
    QString url = ARKPHARM_URL;
    url.append(m_Code);
    startSplider(QUrl(url));
}

void BiJiaARKThread::run()
{
    initManager();
    emit signalStart();
    exec();
}

