#include "BiJiaCOMBThread.h"
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))

#else
#define QStringLiteral(str)  QString::fromUtf8(str)
#endif
BiJiaCOMBThread::BiJiaCOMBThread(QObject *parent) :
    QThread(parent),manager(NULL),isFirst(true),cookie(""),isSecond(false)
{
    isLogin = false;
}

BiJiaCOMBThread::~BiJiaCOMBThread()
{
    if(manager){
        manager->deleteLater();
        manager = NULL;
    }
}

void BiJiaCOMBThread::setCode(const QString &code, const QString &username, const QString &passwd)
{
    m_Code = code;
    m_username = username;
    m_passwd = passwd;
}

void BiJiaCOMBThread::startSplider(const QUrl& url)
{
    //https://www.combi-blocks.com/cgi-bin/cbp.cgi
    qDebug()<<QStringLiteral("COMBLOCKS开始请求数据:请求地址为: ")<<url<<QStringLiteral(" 是否支持ssl?")<<QSslSocket::supportsSsl();
//判断是否已经登录，如果未登录就登录，若是已经登录则跳出一下代码
    if(!isLogin && !m_username.isEmpty() && !m_passwd.isEmpty()){
        QByteArray content;
        content.clear();
        content.append("MY_ACTION:LOGIN&THE_USERNAME:");
        content.append(m_username);
        content.append("&THE_PWD:");
        content.append(m_passwd);
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
        isFirst = true;
        qDebug()<<" content:  "<<content;
        manager->post(req,content);
    }else if(!m_username.isEmpty() && !m_passwd.isEmpty()){
        //第二次
        startEnd();
    }else{
        //发现用户名和密码为空，将本次请求终止
        emit signalSendFinalData("",false);
    }
}

void BiJiaCOMBThread::startEnd()
{
    //https://www.combi-blocks.com/cgi-bin/cbp.cgi
    qDebug()<<" 第二次请求 "<<QSslSocket::supportsSsl();
    isSecond = false;
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

    //qDebug()<<" contentLength:  "<<content<<" post url"<<url2;
    manager->post(req,content);
}

void BiJiaCOMBThread::replyFinished(QNetworkReply *reply)
{
    //    qDebug()<<"BiJiaCOMBThread  replyFinished "<<reply->rawHeaderList();
    foreach (QByteArray arr, reply->rawHeaderList()) {
        //qDebug()<<" header "<<arr << "-------> " <<reply->rawHeader(arr);
    }
    //判断是否为第一次请求，若是则取出cookie，并将是否登录的标识置为true
    if(reply->error() ==QNetworkReply::NoError && isFirst){
        cookie = reply->rawHeader("Set-Cookie");
        isLogin = true;
//        cookie = "CBCART=Paradise**100983028";
        qDebug()<<"BiJiaCOMBThread第一次返回cookie "<<cookie;
        if(!cookie.isEmpty()){
            startEnd();
            isFirst = false;
        }else{
            emit signalSendFinalData("",false);
        }
    }
    if(!isFirst){
        QByteArray arr = reply->readAll();
        qDebug()<<"BiJiaCOMBThread第二次返回 ";
        //emit signalMessageShow(QStringLiteral("正在返回最终的数据............"));
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

