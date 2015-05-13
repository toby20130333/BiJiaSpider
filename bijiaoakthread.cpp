﻿#include "bijiaoakthread.h"

BiJiaOAKThread::BiJiaOAKThread(QObject *parent) :
    QThread(parent),isFirst(true),manager(NULL)
{

}

BiJiaOAKThread::~BiJiaOAKThread()
{
    if(manager){
        manager->deleteLater();
        manager = NULL;
    }
}

void BiJiaOAKThread::setCode(const QString &code)
{
    m_Code = code;
}
///
/// \brief BiJiaOAKThread::startSplider
/// \param url使用ssl进行get
///
void BiJiaOAKThread::startSplider(const QUrl& url)
{
    qDebug()<<" BiJiaOAKThread startLoad "<<url;
    QUrl url2(url);
//    url2.setPort(443);
    QNetworkRequest req;
    req.setUrl(url2);
//    QSslConfiguration conf = req.sslConfiguration();
//    conf.setProtocol(QSsl::SslV3);
//    req.setSslConfiguration(conf);
    manager->get(req);
    emit signalMessageShow(tr("正在获取数据............"));
}

void BiJiaOAKThread::secondStart(const QUrl &url)
{
    emit signalMessageShow(tr("正在第二次获取最终数据............"));
    startSplider(url);
}

void BiJiaOAKThread::initManager()
{
    connect(this,SIGNAL(signalStart()),this,SLOT(slotStart()));
}

void BiJiaOAKThread::replyFinished(QNetworkReply *reply)
{
    qDebug()<<"BiJiaOAKThread-----  replyFinished "<<reply->readAll();
    //网页重定向
    QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    qDebug()<<"BiJiaBehThread redirectionTarget "<<redirectionTarget;
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
        realUrl.prepend(OAKCHEMIL_URL);
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
void BiJiaOAKThread::slotStart()
{
    isFirst = true;
    if(manager == NULL){
    manager = new QNetworkAccessManager(0);
    connect(manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));
    }
    QString url = OAKCHEMIL_URL;
    url.append(m_Code);
    startSplider(QUrl(url));
}
void BiJiaOAKThread::run()
{
    initManager();
    emit signalStart();
    exec();
}

