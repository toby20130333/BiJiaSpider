#include "BiJiaCOMBThread.h"

BiJiaCOMBThread::BiJiaCOMBThread(QObject *parent) :
    QThread(parent),manager(NULL),isFirst(true),cookie("")
{
    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));
}

BiJiaCOMBThread::~BiJiaCOMBThread()
{
    if(manager){
        manager->deleteLater();
        manager = NULL;
    }
}

void BiJiaCOMBThread::setCode(const QString &code)
{
    m_Code = code;
}

void BiJiaCOMBThread::startSplider(const QUrl& url)
{
    //https://www.combi-blocks.com/cgi-bin/cbp.cgi
    qDebug()<<" BiJiaCOMBThread startLoad "<<QSslSocket::supportsSsl();
    QByteArray content;
    content.clear();
    content.append("MY_ACTION:LOGIN&THE_USERNAME:toby520&THE_PWD:13873864");
    int contentLength = content.length();
    QNetworkRequest req;
    QUrl url2(url);
    url2.setPort(443);
    req.setUrl(QUrl(url2));
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
    req.setHeader(QNetworkRequest::ContentLengthHeader,contentLength);

    QSslConfiguration conf = req.sslConfiguration();
    conf.setProtocol(QSsl::SslV3);
    req.setSslConfiguration(conf);

    qDebug()<<" contentLength:  "<<contentLength;
    manager->post(req,content);    
}

void BiJiaCOMBThread::startEnd()
{
    //https://www.combi-blocks.com/cgi-bin/cbp.cgi
    qDebug()<<" 第二次请求 "<<QSslSocket::supportsSsl();
    QByteArray content;
    content.clear();
    content.append("Input=");
    content.append(m_Code);
    int contentLength = content.length();
    QNetworkRequest req;
    QUrl url2(COMBLOCK_UURL);
    req.setUrl(QUrl(url2));
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
    req.setHeader(QNetworkRequest::ContentLengthHeader,contentLength);
    req.setHeader(QNetworkRequest::SetCookieHeader,QVariant(cookie));

    qDebug()<<" contentLength:  "<<content<<" post url"<<url2;
    manager->post(req,content);
}

void BiJiaCOMBThread::replyFinished(QNetworkReply *reply)
{
//    qDebug()<<"BiJiaCOMBThread  replyFinished "<<reply->rawHeaderList();
//    foreach (QByteArray arr, reply->rawHeaderList()) {
//        qDebug()<<" header "<<arr << "-------> " <<reply->rawHeader(arr);
//    }
    //网页重定向
    QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

    if(reply->error() ==QNetworkReply::NoError && isFirst){
        cookie = reply->rawHeader("Set-Cookie");
        qDebug()<<"第一次返回 "<<cookie;
        startEnd();
        isFirst = false;
    }
    if(!isFirst){       
        QByteArray arr = reply->readAll();
        qDebug()<<"第二次返回 "<<arr;
        //emit signalMessageShow(tr("正在返回最终的数据............"));
        emit signalSendFinalData(arr,false);
        QFile file("./com.html");
        if(file.exists()){
            //qDebug()<<"BiJiaCOMBThread  remove "<<reply->rawHeaderList();
            file.remove();
        }
        if(file.open(QIODevice::WriteOnly)){
            file.write(arr);
        }
        file.close();
    }
}
void BiJiaCOMBThread::initManager()
{
    connect(this,SIGNAL(signalStart()),this,SLOT(slotStart()));
}

void BiJiaCOMBThread::slotStart()
{
    if(manager == NULL){
    manager = new QNetworkAccessManager(0);
    connect(manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));
    }
    QString url = COMBLOCK_URL;
//    url.append(m_Code);
//    url.append("&searchName=1");
    startSplider(QUrl(url));
}
void BiJiaCOMBThread::run()
{
    initManager();
    emit signalStart();
    exec();
}

