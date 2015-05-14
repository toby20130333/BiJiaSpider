#include "BiJiaAstatechThread.h"
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))

#else
#define QStringLiteral(str)  QString::fromUtf8(str)
#endif
BiJiaAstatechThread::BiJiaAstatechThread(QObject *parent) :
    QThread(parent),isFirst(true),manager(NULL)
{

}

BiJiaAstatechThread::~BiJiaAstatechThread()
{
    if(manager){
        manager->deleteLater();
        manager = NULL;
    }
}

void BiJiaAstatechThread::setCode(const QString &code)
{
    m_Code = code;
}

void BiJiaAstatechThread::startSplider(const QUrl& url)
{    
    QString urlStr = url.toString();
    urlStr.append(m_Code);
    urlStr.append(END_URL);
    qDebug()<<QStringLiteral("Astatech开始请求数据:请求地址为: ")<<urlStr;
    manager->get(QNetworkRequest(QUrl(urlStr)));
    emit signalMessageShow(QStringLiteral("Astatech 正在获取数据............"));
}

void BiJiaAstatechThread::secondStart(const QUrl &url)
{
    emit signalMessageShow(QStringLiteral("Astatech 正在第二次获取最终数据............"));
    QString urlStr = url.toString();
    urlStr.prepend("http://astatech.com.cn");
    qDebug()<<QStringLiteral("Astatech开始第二次请求数据:请求地址为: ")<<urlStr;
    manager->get(QNetworkRequest(QUrl(urlStr)));
    emit signalMessageShow(QStringLiteral("Astatech 正在获取数据............"));
}

void BiJiaAstatechThread::initManager()
{
    connect(this,SIGNAL(signalStart()),this,SLOT(slotStart()));
}

void BiJiaAstatechThread::replyFinished(QNetworkReply *reply)
{
    qDebug()<<"BiJiaAstatechThread  replyFinished "<<reply->errorString();

    QByteArray arr = reply->readAll();
    if(reply->error() ==QNetworkReply::NoError && isFirst){
        foreach (QByteArray arr, reply->rawHeaderList()) {
            //qDebug()<<" 第一次返回 header "<<arr << "-------> " <<reply->rawHeader(arr);
        }
        emit signalMessageShow(QStringLiteral("Astatech 正在返回准备的数据............"));
        emit signalSendFinalData(arr,true);
        isFirst = false;       
    }
    if(!isFirst){
        emit signalMessageShow(QStringLiteral("Astatech 正在返回最终的数据............"));
        emit signalSendFinalData(arr,false);
    }
    QFile file("./astatech.html");
    if(file.exists()){
        //qDebug()<<"BiJiaCOMBThread  remove "<<reply->rawHeaderList();
        file.remove();
    }
    if(file.open(QIODevice::WriteOnly)){
        file.write(arr);
    }
    file.close();
}
void BiJiaAstatechThread::slotStart()
{
    isFirst = true;
    if(manager == NULL){
    manager = new QNetworkAccessManager(0);
    connect(manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));
    }
    QString url = ASTATECH_URL;
    startSplider(QUrl(url));
}
void BiJiaAstatechThread::run()
{
    initManager();
    emit signalStart();
    exec();
}

