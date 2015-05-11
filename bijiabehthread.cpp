#include "BiJiaBehThread.h"

BiJiaBehThread::BiJiaBehThread(QObject *parent) :
    QThread(parent),isFirst(true),manager(NULL)
{

}

BiJiaBehThread::~BiJiaBehThread()
{
    if(manager){
        manager->deleteLater();
        manager = NULL;
    }
}

void BiJiaBehThread::setCode(const QString &code)
{
    m_Code = code;
}

void BiJiaBehThread::startSplider(const QUrl& url)
{
    qDebug()<<" startLoad "<<url;
    manager->get(QNetworkRequest(url));
    emit signalMessageShow(tr("正在获取数据............"));
}

void BiJiaBehThread::secondStart(const QUrl &url)
{
    emit signalMessageShow(tr("正在第二次获取最终数据............"));
    startSplider(url);
}

void BiJiaBehThread::initManager()
{
    connect(this,SIGNAL(signalStart()),this,SLOT(slotStart()));
}

void BiJiaBehThread::replyFinished(QNetworkReply *reply)
{
    qDebug()<<"BiJiaBehThread  replyFinished "<<reply->errorString();
    //网页重定向
    QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    qDebug()<<"BiJiaBehThread redirectionTarget "<<isFirst;
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
        qDebug()<<"BiJiaBehThread redirectionTarget catalogno "<<realUrl;
        if(realUrl.isEmpty()){
            qDebug()<<"BiJiaBehThread redirectionTarget catalogno22222 "<<realUrl;
            return;
        }
        realUrl.prepend(BEPHARM_URL);
        startSplider(realUrl);
        return ;
    }
    if(reply->error() ==QNetworkReply::NoError && isFirst){
        emit signalMessageShow(tr("正在返回准备的数据............"));
        emit signalSendFinalData(reply->readAll(),true);
        isFirst = false;
    }
    if(!isFirst){
        emit signalMessageShow(tr("正在返回最终的数据............"));
        emit signalSendFinalData(reply->readAll(),false);
    }
}
void BiJiaBehThread::slotStart()
{
    isFirst = true;
    if(manager == NULL){
    manager = new QNetworkAccessManager(0);
    connect(manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));
    }
    QString url = BEPHARM_URL;
    url.append(m_Code);
    url.append("&searchName=1");
    startSplider(QUrl(url));
}
void BiJiaBehThread::run()
{
    initManager();
    emit signalStart();
    exec();
}

