#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget),m_View(NULL),
    isFirstBe(false),
    m_ARK(NULL),
    m_BEH(NULL),
    m_COMB(NULL)
{
    ui->setupUi(this);
    arkIsOK = false;
    behIsOk  = false;
    comBlOk = false;
    m_View =new  QWebView();
    QStringList lst;
    initThreeThreads();
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setHostName("acidalia");
    db.setDatabaseName("bigarkdb");
    db.setUserName("sarah");
    db.setPassword("123456tb");
    bool ok = db.open();
    qDebug()<<" opened? "<<ok;
    connect(ui->comboBox,SIGNAL(currentIndexChanged(QString)),this,SLOT(setCurrentCas(QString)));
    QSqlQuery query("SELECT cas FROM arkdata order by id limit 100");
      while (query.next()) {
          QString country = query.value(0).toString();
          lst.append(country);
      }
      ui->comboBox->addItems(lst);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::initThreeThreads()
{
    m_ARK =  new BiJiaARKThread(0);
    connect(m_ARK,SIGNAL(signalSendFinalData(QByteArray)),this,SLOT(parseArk(QByteArray)));
    m_BEH = new BiJiaBehThread(0);
    connect(m_BEH,SIGNAL(signalSendFinalData(QByteArray,bool)),this,SLOT(parseBeh(QByteArray,bool)));
    connect(m_BEH,SIGNAL(signalMessageShow(QString)),this,SLOT(appendStatus(QString)));

    m_COMB = new BiJiaCOMBThread(0);
    connect(m_COMB,SIGNAL(signalSendFinalData(QByteArray,bool)),this,SLOT(parseComBlock(QByteArray,bool)));


}
void Widget::setUrl(const QUrl &url)
{
    m_View->setUrl(url);
}

void Widget::setWebSiteType(Widget::WEBSITE_TYPE type)
{
    iType = type;
}

void Widget::startLoad(const QUrl &url)
{
    qDebug()<<" startLoad "<<url;
}

void Widget::startPost(const QUrl &url)
{
    QByteArray content(ui->lineEdit->text().toUtf8());
    int contentLength = content.length();
    QNetworkRequest req;
    req.setUrl(QUrl(url));
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
    req.setHeader(QNetworkRequest::ContentLengthHeader,contentLength);
    //      manager->post(req,content);
}

void Widget::parseArk(const QByteArray &arr)
{
    QWebFrame *frame =m_View->page()->mainFrame();
    frame->setHtml(QString(arr));
    QWebElement document = frame->documentElement();
    QWebElementCollection introSpans = document.findAll("input[type=hidden]");
    QList<QWebElement> elst = introSpans.toList();
    ui->textEdit->clear();
    for(int i = 0;i<elst.count();i++){
        QString value = elst.at(i).attribute("value");
        QString id = elst.at(i).attribute("id");
        qDebug()<<"-------------ARKPHARM data--------------- "<<value<< " \n"<<id;
        if(id.contains("Hidden") || id.contains("WeightValue") || value.isEmpty()){
            continue;
        }
        ui->textEdit->append(value);
    }
    if(elst.isEmpty()){
        ui->textEdit->append(tr("未找到相关产品"));
    }
    arkIsOK = true;
    if(arkIsOK && behIsOk && comBlOk){
        ui->pushButton->setEnabled(true);
        ui->label_5->setText(tr("搜索完成,请仔细查看搜索结果"));
    }
}

void Widget::parseBeh(const QByteArray &arr, bool isFirst)
{
    QWebFrame *frame =m_View->page()->mainFrame();
    frame->setHtml(QString(arr));
    QWebElement document = frame->documentElement();
    if(isFirst){
        ui->textEdit_2->clear();
        examineChildElements(document);
    }else{
        QWebElementCollection introSpans = document.findAll("input[type=hidden]");
        QList<QWebElement> elst = introSpans.toList();
        QStringList secondResult;
        secondResult.clear();
        for(int i = 0;i<elst.count();i++){
            QString value = elst.at(i).attribute("value");
            QString id = elst.at(i).attribute("id");
            qDebug()<<"-------------BEPHARM data--------------- "<<value<< " \n"<<id;
            if(id.isEmpty() || value.isEmpty() || id.contains("WeightValue")){
                continue;
            }else{
//                ui->textEdit_2->append(tr("未找到该产品价格"));
//                ui->label_5->setText(tr("未找到该产品价格"));
            }
            ui->textEdit_2->append(value);
        }
        behIsOk = true;
        if(arkIsOK && behIsOk && comBlOk){
            ui->pushButton->setEnabled(true);
            ui->label_5->setText(tr("搜索完成,请仔细查看搜索结果"));
        }
    }
}

void Widget::parseComBlock(const QByteArray &arr, bool isFirst)
{
    qDebug()<<"parseComBlock "<<arr.size();
//    ui->textEdit_3->setText(QString(arr));
    QWebFrame *frame =m_View->page()->mainFrame();
    frame->setHtml(QString(arr));
    QWebElement document = frame->documentElement();
    QWebElementCollection introSpans = document.findAll("TD[ALIGN=right]");
    QList<QWebElement> elst = introSpans.toList();
    ui->textEdit_3->clear();
    for(int i = 0;i<elst.count();i++){
        comBlOk = true;
        QString value = elst.at(i).toInnerXml();
        if(value.contains("<input") || value.isEmpty() || value.contains("Others")){
            continue;
        }
        ui->textEdit_3->append(elst.at(i).toInnerXml());
    }
    if(elst.isEmpty()){
        ui->textEdit_3->append(tr("未找到相关产品"));
    }
    if(arkIsOK && behIsOk && comBlOk){
        ui->pushButton->setEnabled(true);
        ui->label_5->setText(tr("搜索完成,请仔细查看搜索结果"));
    }
}

void Widget::appendStatus(QString msg)
{
    ui->label_5->setText(msg);
}

void Widget::operatorData(DATA_M stu)
{
    QSqlQuery query;
    query.exec("create table pricedata(id int primary key,cid int,price varchar,rate varchar)");

    query.prepare("INSERT INTO pricedata (id, cid,price,rate) "
                  "VALUES (:id, :cid,:price,:rate)");
    query.bindValue(":cid", stu.id);
    query.bindValue(":price",stu.price);
    query.bindValue(":rate", stu.value);
    qDebug()<<" exec status is :"<<query.exec();
}
void Widget::on_pushButton_clicked()
{
    ui->pushButton->setEnabled(false);
    m_ARK->setCode(ui->lineEdit->text());
    m_BEH->setCode(ui->lineEdit->text());
    m_COMB->setCode(ui->lineEdit->text());
    if(!isFirstBe){
        m_ARK->start();
        m_BEH->start();
        m_COMB->start();
        isFirstBe = true;
    }else{
        m_ARK->slotStart();
        m_BEH->slotStart();
        m_COMB->slotStart();
    }
}

void Widget::setCurrentCas(QString cas)
{
    ui->lineEdit->setText(cas);
}
//! [traverse document]
//! 解析beh第一次请求回来的内容
void Widget::examineChildElements(const QWebElement &parentElement)
{
    QWebElement firstTextInput = parentElement.findFirst("img[class=cart_img1]");
    QString storedText = firstTextInput.attribute("src");
    QStringList lst = storedText.split("/");
    storedText = lst.at(lst.count()-1);
    storedText = storedText.mid(0,storedText.count()-4);
    qDebug()<<"in it.....Widget::examineChildElements......."<<storedText;
    QString beUrl = "/en/productview.html?catalogno=";
    beUrl.prepend("http://www.bepharm.com");
    beUrl.append(storedText);
    if(storedText.isEmpty()){
        ui->textEdit_2->append(tr("未找到相关产品信息"));
        ui->label_5->setText(tr("未找到相关产品信息"));
        return;
    }
    if(m_BEH){
        m_BEH->secondStart(QUrl(beUrl));
    }
}
