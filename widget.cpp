#include "widget.h"
#include "ui_widget.h"
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))

#else
#define QStringLiteral(str)  QString::fromUtf8(str)
#endif
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget),m_View(NULL),
    isFirstBe(false),
    m_ARK(NULL),
    m_BEH(NULL),
    m_COMB(NULL),
    m_OAK(NULL)
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    //初始化为未按下鼠标左键
    mouse_press = false;
    arkIsOK = false;
    behIsOk  = false;
    comBlOk = false;
    oakOK = false;
    aokOK = false;
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
    QSqlQuery query("SELECT cas FROM arkdata order by id limit 200");
    while (query.next()) {
        QString country = query.value(0).toString();
        lst.append(country);
    }
    ui->comboBox->addItems(lst);
}

Widget::~Widget()
{
    if(m_View){
        m_View->deleteLater();
        m_View = NULL;
    }
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
    m_OAK = new BiJiaOAKThread(0);
    connect(m_OAK,SIGNAL(signalSendFinalData(QByteArray,bool)),this,SLOT(parseOAkChemical(QByteArray,bool)));
    connect(m_OAK,SIGNAL(signalMessageShow(QString)),this,SLOT(appendStatus(QString)));

    m_AOK =  new BiJiaAOKThread(0);
    connect(m_AOK,SIGNAL(signalSendFinalData(QByteArray)),this,SLOT(parseAoK(QByteArray)));

    m_Astatech = new BiJiaAstatechThread(0);
    connect(m_Astatech,SIGNAL(signalSendFinalData(QByteArray,bool)),this,SLOT(parseAstatech(QByteArray,bool)));
    connect(m_Astatech,SIGNAL(signalMessageShow(QString)),this,SLOT(appendStatus(QString)));
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
    frame->setHtml((arr));
    QWebElement document = frame->documentElement();
    QWebElementCollection introSpans = document.findAll("input[type=hidden]");
    QList<QWebElement> elst = introSpans.toList();
    ui->textEdit->clear();
    for(int i = 0;i<elst.count();i++){
        QString value = elst.at(i).attribute("value");
        QString id = elst.at(i).attribute("id");
        //qDebug()<<"-------------ARKPHARM data--------------- "<<value<< " \n"<<id;
        if(id.contains("Hidden") || id.contains("WeightValue") || value.isEmpty()){
            continue;
        }
        ui->textEdit->append(value);
    }
    if(elst.isEmpty()){
        ui->textEdit->append(QStringLiteral("未找到相关产品"));
    }
    arkIsOK = true;
    qDebug()<<QStringLiteral("ARK价格获取完毕--------------------------------");

    if(arkIsOK && behIsOk && comBlOk && oakOK && aokOK && astateChOK){
        ui->pushButton->setEnabled(true);
        ui->label_5->setText(QStringLiteral("ARK最后搜索完成,请仔细查看搜索结果"));
    }
}

void Widget::parseBeh(const QByteArray &arr, bool isFirst)
{
    QWebFrame *frame =m_View->page()->mainFrame();
    frame->setHtml(arr);
    QWebElement document = frame->documentElement();
    if(isFirst){
        ui->textEdit_2->clear();
        examineChildElements(document,true);
    }else{
        QWebElementCollection introSpans = document.findAll("input[type=hidden]");
        QList<QWebElement> elst = introSpans.toList();
        QStringList secondResult;
        secondResult.clear();
        for(int i = 0;i<elst.count();i++){
            QString value = elst.at(i).attribute("value");
            QString id = elst.at(i).attribute("id");
            //qDebug()<<"-------------BEPHARM data--------------- "<<value<< " \n"<<id;
            if(id.isEmpty() || value.isEmpty() || id.contains("WeightValue")){
                continue;
            }else{
                //                ui->textEdit_2->append(tr("未找到该产品价格"));
                //                ui->label_5->setText(tr("未找到该产品价格"));
                ui->textEdit_2->append(value);
            }

        }
        behIsOk = true;
        qDebug()<<QStringLiteral("BEHM价格获取完毕--------------------------------");

        if(arkIsOK && behIsOk && comBlOk && oakOK && aokOK && astateChOK){
            ui->pushButton->setEnabled(true);
            ui->label_5->setText(QStringLiteral("BEHM最后搜索完成,请仔细查看搜索结果"));
        }
    }
}

void Widget::parseComBlock(const QByteArray &arr, bool isFirst)
{
    //    qDebug()<<"parseComBlock "<<arr.size();
    //    ui->textEdit_3->setText(QStringLiteral(arr));
    QWebFrame *frame =m_View->page()->mainFrame();
    frame->setHtml((arr));
    QWebElement document = frame->documentElement();
    QWebElementCollection introSpans = document.findAll("TD[ALIGN=right]");
    QList<QWebElement> elst = introSpans.toList();
    ui->textEdit_3->clear();
    for(int i = 0;i<elst.count();i++){
        QString value = elst.at(i).toInnerXml();
        if(value.contains("<input") || value.isEmpty() || value.contains("Others")){
            continue;
        }
        ui->textEdit_3->append(elst.at(i).toInnerXml());
    }
    if(elst.isEmpty()){
        ui->textEdit_3->append(QStringLiteral("未找到相关产品"));
    }
    qDebug()<<QStringLiteral("COMBLOCKS价格获取完毕--------------------------------");
    comBlOk = true;
    if(arkIsOK && behIsOk && comBlOk && oakOK && aokOK && astateChOK){
        ui->pushButton->setEnabled(true);
        ui->label_5->setText(QStringLiteral("COM最后搜索完成,请仔细查看搜索结果"));
    }
}

void Widget::parseOAkChemical(const QByteArray &arr, bool isFirst)
{
    QWebFrame *frame =m_View->page()->mainFrame();
    frame->setHtml((arr));
    QWebElement document = frame->documentElement();
    if(isFirst){
        ui->textEdit_4->clear();
        examineChildElements(document,false);
    }else{
        QWebElementCollection introSpans = document.findAll("input[type=hidden]");
        QList<QWebElement> elst = introSpans.toList();
        QStringList secondResult;
        secondResult.clear();
        for(int i = 0;i<elst.count();i++){
            QString value = elst.at(i).attribute("value");
            QString id = elst.at(i).attribute("id");
            //qDebug()<<"-------------BEPHARM data--------------- "<<value<< " \n"<<id;
            if(id.isEmpty() || value.isEmpty() || id.contains("WeightValue")){
                continue;
            }else{
            }
            ui->textEdit_4->append(value);
        }
        oakOK = true;
        qDebug()<<QStringLiteral("OAKCHEMICAL价格获取完毕--------------------------------");
        if(arr.isEmpty()){
            ui->textEdit_4->setText(QStringLiteral("未找到相关产品"));
        }
        if(arkIsOK && behIsOk && comBlOk && oakOK && aokOK && astateChOK){
            ui->pushButton->setEnabled(true);
            ui->label_5->setText(QStringLiteral("OAK最后搜索完成,请仔细查看搜索结果"));
        }
    }
}

void Widget::parseAoK(const QByteArray &arr)
{
    QWebFrame *frame =m_View->page()->mainFrame();
    frame->setHtml((arr));
    QWebElement document = frame->documentElement();
    QWebElementCollection introSpans = document.findAll("input[type=hidden]");
    QList<QWebElement> elst = introSpans.toList();
    ui->textEdit_5->clear();
    for(int i = 0;i<elst.count();i++){
        QString value = elst.at(i).attribute("value");
        QString name = elst.at(i).attribute("name");
        //qDebug()<<"-------------ARKPHARM data--------------- "<<value<< " \n"<<name;
        if(name.contains("pno") || name.contains("producte") || value.isEmpty()){
            continue;
        }
        if(name == "cas"){
            value.prepend("cas:");
        }
        ui->textEdit_5->append(value);
    }
    if(elst.isEmpty()){
        ui->textEdit_5->append(QStringLiteral("未找到相关产品"));
    }
    aokOK = true;
    qDebug()<<QStringLiteral("AOK价格获取完毕--------------------------------");

    if(arkIsOK && behIsOk && comBlOk && oakOK && aokOK && astateChOK){
        ui->pushButton->setEnabled(true);
        ui->label_5->setText(QStringLiteral("AOK最后搜索完成,请仔细查看搜索结果"));
    }
}

void Widget::parseAstatech(const QByteArray &arr, bool isFirst)
{
    QWebFrame *frame =m_View->page()->mainFrame();
    frame->setHtml(arr);
    QWebElement document = frame->documentElement();
    if(isFirst){
        ui->textEdit_6->clear();
        examineChildElementsOfAstatech(document,true);
    }else{
        QWebElementCollection introSpans = document.findAll("tr.bg_tr td");
        QList<QWebElement> elst = introSpans.toList();
        qDebug()<<"-------------parseAstatech data----------count----- "<<elst.count();
        QStringList secondResult;
        secondResult.clear();
        for(int i = 0;i<elst.count();i++){
            if(elst.at(i).hasAttributes()){
                qDebug()<<"I have some attributes"<<elst.at(i).toPlainText();
                continue;
            }
            if(elst.at(i).hasAttribute("A")){
                qDebug()<<"I have A attribute"<<elst.at(i).toPlainText();
                continue;
            }
            QString value = elst.at(i).toPlainText();
            value  = value.trimmed();
            qDebug()<<"-------------parseAstatech data------i--------- "<<i<<" "<<value;
            if(value.isEmpty() || value.contains("%") || (value.contains("acid")) || value.contains("Please") || value.contains(ui->lineEdit->text())){
                continue;
            }else{
                ui->textEdit_6->append(value);
            }

        }
        astateChOK = true;
        qDebug()<<QStringLiteral("AstateCh价格获取完毕--------------------------------");

        if(arkIsOK && behIsOk && comBlOk && oakOK && aokOK && astateChOK){
            ui->pushButton->setEnabled(true);
            ui->label_5->setText(QStringLiteral("AstateCh最后搜索完成,请仔细查看搜索结果"));
        }
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
///
/// \brief Widget::on_pushButton_clicked
/// 点击请求按钮进行数据请求
/// 同时情况之前的请求和返回的数据
///
void Widget::on_pushButton_clicked()
{
    ui->pushButton->setEnabled(false);
    if(ui->lineEdit->text().isEmpty()){
        return;
    }
    m_ARK->setCode(ui->lineEdit->text());
    m_BEH->setCode(ui->lineEdit->text());
    m_OAK->setCode(ui->lineEdit->text());
    m_AOK->setCode(ui->lineEdit->text());
    m_Astatech->setCode(ui->lineEdit->text());
    m_COMB->setCode(ui->lineEdit->text(),ui->lineEdit_3->text(),ui->lineEdit_2->text());
    //每次重新请求，都得清空之前的内容
    ui->textEdit->clear();
    ui->textEdit_2->clear();
    ui->textEdit_3->clear();
    ui->textEdit_4->clear();
    ui->textEdit_5->clear();
    ui->label_5->setText("开始请求数据...");
    if(!isFirstBe){
        m_ARK->start();
        m_BEH->start();
        m_COMB->start();
        m_OAK->start();
        m_AOK->start();
        m_Astatech->start();
        isFirstBe = true;
    }else{
        m_COMB->setCode(ui->lineEdit->text(),ui->lineEdit_3->text(),ui->lineEdit_2->text());
        m_ARK->slotStart();
        m_BEH->slotStart();
        m_COMB->slotStart();
        m_OAK->slotStart();
        m_AOK->slotStart();
        m_Astatech->slotStart();
    }
}

void Widget::setCurrentCas(QString cas)
{
    ui->lineEdit->setText(cas);
}
//! [traverse document]
//! 解析beh第一次请求回来的内容
void Widget::examineChildElements(const QWebElement &parentElement,bool isBeh)
{
    if(isBeh){
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
            ui->textEdit_2->append(QStringLiteral("未找到相关产品信息"));
            ui->label_5->setText(QStringLiteral("未找到相关产品信息"));
            return;
        }
        if(m_BEH){
            m_BEH->secondStart(QUrl(beUrl));
        }
    }else{
        //解析oakchemical数据
        m_OAK->secondStart(QUrl(""));
    }
}

void Widget::examineChildElementsOfAstatech(const QWebElement &parentElement, bool isBeh)
{

    QWebElementCollection introSpans = parentElement.findAll("DIV.cpbh a");
    QList<QWebElement> elst = introSpans.toList();
    qDebug()<<"-------------examineChildElementsOfAstatech data------in--------- "<<elst.count();
    QStringList secondResult;
    secondResult.clear();
    for(int i = 0;i<elst.count();i++){
        QString value = elst.at(i).attribute("href");
        QString id = elst.at(i).attribute("title");
        qDebug()<<"-------------examineChildElementsOfAstatech data--------------- "<<value<< " \n"<<id;
        if(id.isEmpty() || value.isEmpty()){
            continue;
        }else{
            secondResult.append(value);
        }

    }
    if(m_Astatech){
        if(secondResult.count() >=2){
            m_Astatech->secondStart(secondResult.at(1));
        }else{
            ui->textEdit_6->setText(QStringLiteral("未找到相关产品信息"));
        }
    }

}
void Widget::paintEvent(QPaintEvent *)
{
    QPainterPath path;
    path.setFillRule(Qt::WindingFill);
    int rate = 8;
    path.addRect(rate, rate, this->width()-rate*2, this->height()-rate*2);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillPath(path, QBrush(Qt::white));

    QColor color(0, 0, 0, 50);
    for(int i=0; i<rate; i++)
    {
        QPainterPath path;
        path.setFillRule(Qt::WindingFill);
        path.addRect(rate-i, rate-i, this->width()-(rate-i)*2, this->height()-(rate-i)*2);
        color.setAlpha(150 - qSqrt(i)*50);
        painter.setPen(color);
        painter.drawPath(path);
    }
}

void Widget::mousePressEvent(QMouseEvent *event)
{
    //只能是鼠标左键移动和改变大小
    if(event->button() == Qt::LeftButton)
    {
        mouse_press = true;
    }

    //窗口移动距离
    move_point = event->globalPos() - pos();
}

void Widget::mouseReleaseEvent(QMouseEvent *)
{
    mouse_press = false;
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    //移动窗口
    if(mouse_press)
    {
        QPoint move_pos = event->globalPos();
        move(move_pos - move_point);
    }
}
void Widget::on_closeBtn_clicked()
{
    this->close();
    qApp->quit();
}
