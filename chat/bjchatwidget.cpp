#include "bjchatwidget.h"
#include "ui_bjchatwidget.h"

BJChatWidget::BJChatWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BJChatWidget)
{
    ui->setupUi(this);
    this->resize(850,550);
    ui->textEdit->setFocusPolicy(Qt::StrongFocus);
    ui->textBrowser->setFocusPolicy(Qt::NoFocus);

    ui->textEdit->setFocus();
    ui->textEdit->installEventFilter(this);//设置完后自动调用其eventFilter函数
    privatechat = NULL;
    privatechat1 = NULL;

    udpSocket = new QUdpSocket(this);
    port = 45454;
    bb = 0;
    udpSocket->bind(port,QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    connect(udpSocket,SIGNAL(readyRead()),this,SLOT(processPendingDatagrams()));
    sendMessage(NewParticipant);

    server = new TcpServer(this);
    connect(server,SIGNAL(sendFileName(QString)),this,SLOT(sentFileName(QString)));
    connect(ui->textEdit,SIGNAL(currentCharFormatChanged(QTextCharFormat)),this,SLOT(currentFormatChanged(const QTextCharFormat)));

}

void BJChatWidget::currentFormatChanged(const QTextCharFormat &format)
{//当编辑器的字体格式改变时，我们让部件状态也随之改变
    ui->fontComboBox->setCurrentFont(format.font());

    if(format.fontPointSize()<9)  //如果字体大小出错，因为我们最小的字体为9
    {
        ui->fontsizecomboBox->setCurrentIndex(3); //即显示12
    }
    else
    {
        ui->fontsizecomboBox->setCurrentIndex(ui->fontsizecomboBox->findText(QString::number(format.fontPointSize())));

    }

    ui->textbold->setChecked(format.font().bold());
    ui->textitalic->setChecked(format.font().italic());
    ui->textUnderline->setChecked(format.font().underline());
    color = format.foreground().color();
}

void BJChatWidget::processPendingDatagrams()   //接收数据UDP
{
    while(udpSocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(),datagram.size());
        QDataStream in(&datagram,QIODevice::ReadOnly);
        int messageType;
        in >> messageType;
        QString userName,localHostName,ipAddress,message;
        QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        switch(messageType)
        {
        case Message:
        {
            in >>userName >>localHostName >>ipAddress >>message;
            ui->textBrowser->setTextColor(Qt::blue);
            ui->textBrowser->setCurrentFont(QFont("Times New Roman",12));
            ui->textBrowser->append("[ " +localHostName+" ] "+ time);
            ui->textBrowser->append(message);
            break;
        }
        case NewParticipant:
        {
            in >>userName >>localHostName >>ipAddress;
            newParticipant(userName,localHostName,ipAddress);

            break;
        }
        case ParticipantLeft:
        {
            in >>userName >>localHostName;
            participantLeft(userName,localHostName,time);
            break;
        }
        case FileName:
        {
            in >>userName >>localHostName >> ipAddress;
            QString clientAddress,fileName;
            in >> clientAddress >> fileName;
            hasPendingFile(userName,ipAddress,clientAddress,fileName);
            break;
        }
        case Refuse:
        {
            in >> userName >> localHostName;
            QString serverAddress;
            in >> serverAddress;
            QString ipAddress = getIP();

            if(ipAddress == serverAddress)
            {
                server->refused();
            }
            break;
        }
        case Xchat:
        {
            in >>userName >>localHostName >>ipAddress;
            showxchat(localHostName,ipAddress);//显示与主机名聊天中，不是用户名
            break;
        }
        }
    }
}

//处理新用户加入
void BJChatWidget::newParticipant(QString userName,QString localHostName,QString ipAddress)
{
    bool bb = ui->tableWidget->findItems(localHostName,Qt::MatchExactly).isEmpty();
    if(bb)
    {
        QTableWidgetItem *user = new QTableWidgetItem(userName);
        QTableWidgetItem *host = new QTableWidgetItem(localHostName);
        QTableWidgetItem *ip = new QTableWidgetItem(ipAddress);
        ui->tableWidget->insertRow(0);
        ui->tableWidget->setItem(0,0,user);
        ui->tableWidget->setItem(0,1,host);
        ui->tableWidget->setItem(0,2,ip);
        ui->textBrowser->setTextColor(Qt::gray);
        ui->textBrowser->setCurrentFont(QFont("Times New Roman",10));
        ui->textBrowser->append(tr("%1 在线！").arg(localHostName));
        ui->onlineUser->setText(tr("在线人数：%1").arg(ui->tableWidget->rowCount()));
        sendMessage(NewParticipant);
    }
}

//处理用户离开
void BJChatWidget::participantLeft(QString /*userName*/,QString localHostName,QString time)
{
    int rowNum = ui->tableWidget->findItems(localHostName,Qt::MatchExactly).first()->row();
    ui->tableWidget->removeRow(rowNum);
    ui->textBrowser->setTextColor(Qt::gray);
    ui->textBrowser->setCurrentFont(QFont("Times New Roman",10));
    ui->textBrowser->append(tr("%1 于 %2 离开！").arg(localHostName).arg(time));
    ui->onlineUser->setText(tr("在线人数：%1").arg(ui->tableWidget->rowCount()));
}

BJChatWidget::~BJChatWidget()
{
    delete ui;
    //	delete privatechat;
    //	privatechat = NULL;
    //udpSocket
    //server
}

void BJChatWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

QString BJChatWidget::getIP()  //获取ip地址
{
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    foreach (QHostAddress address, list)
    {
        if(address.protocol() == QAbstractSocket::IPv4Protocol) //我们使用IPv4地址
            return address.toString();
    }
    return 0;
}

void BJChatWidget::sendMessage(MessageType type, QString serverAddress)  //发送信息
{
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    QString localHostName = QHostInfo::localHostName();
    QString address = getIP();
    out << type << getUserName() << localHostName;


    switch(type)
    {
    case ParticipantLeft:
    {
        break;
    }
    case NewParticipant:
    {
        out << address;
        break;
    }

    case Message :
    {
        if(ui->textEdit->toPlainText() == "")
        {
            QMessageBox::warning(0,tr("警告"),tr("发送内容不能为空"),QMessageBox::Ok);
            return;
        }
        out << address << getMessage();
        ui->textBrowser->verticalScrollBar()->setValue(ui->textBrowser->verticalScrollBar()->maximum());
        break;

    }
    case FileName:
    {
        int row = ui->tableWidget->currentRow();
        QString clientAddress = ui->tableWidget->item(row,2)->text();
        out << address << clientAddress << fileName;
        break;
    }
    case Refuse:
    {
        out << serverAddress;
        break;
    }
    }
    udpSocket->writeDatagram(data,data.length(),QHostAddress::Broadcast, port);

}

QString BJChatWidget::getUserName()  //获取用户名
{
    QStringList envVariables;
    envVariables << "USERNAME.*" << "USER.*" << "USERDOMAIN.*"
                 << "HOSTNAME.*" << "DOMAINNAME.*";
    QStringList environment = QProcess::systemEnvironment();
    foreach (QString string, envVariables)
    {
        int index = environment.indexOf(QRegExp(string));
        if (index != -1)
        {

            QStringList stringList = environment.at(index).split('=');
            if (stringList.size() == 2)
            {
                return stringList.at(1);
                break;
            }
        }
    }
    return "";
}

QString BJChatWidget::getMessage()  //获得要发送的信息
{
    QString msg = ui->textEdit->toHtml();

    ui->textEdit->clear();
    ui->textEdit->setFocus();
    return msg;
}

void BJChatWidget::closeEvent(QCloseEvent *)
{
    sendMessage(ParticipantLeft);
}

void BJChatWidget::sentFileName(QString fileName)
{
    this->fileName = fileName;
    sendMessage(FileName);
}

void BJChatWidget::hasPendingFile(QString userName,QString serverAddress,  //接收文件
                                  QString clientAddress,QString fileName)
{
    QString ipAddress = getIP();
    if(ipAddress == clientAddress)
    {
        int btn = QMessageBox::information(this,tr("接受文件"),
                                           tr("来自%1(%2)的文件：%3,是否接收？")
                                           .arg(userName).arg(serverAddress).arg(fileName),
                                           QMessageBox::Yes,QMessageBox::No);
        if(btn == QMessageBox::Yes)
        {
            QString name = QFileDialog::getSaveFileName(0,tr("保存文件"),fileName);
            if(!name.isEmpty())
            {
                TcpClient *client = new TcpClient(this);
                client->setFileName(name);
                client->setHostAddress(QHostAddress(serverAddress));
                client->show();

            }

        }
        else{
            sendMessage(Refuse,serverAddress);
        }
    }
}

void BJChatWidget::on_send_clicked()//发送
{
    sendMessage(Message);
}

void BJChatWidget::on_sendfile_clicked()
{
    if(ui->tableWidget->selectedItems().isEmpty())
    {
        QMessageBox::warning(0,tr("选择用户"),tr("请先从用户列表选择要传送的用户！"),QMessageBox::Ok);
        return;
    }
    server->show();
    server->initServer();
}

void BJChatWidget::on_close_clicked()//关闭
{
    this->close();
}

bool BJChatWidget::eventFilter(QObject *target, QEvent *event)
{
    if(target == ui->textEdit)
    {
        if(event->type() == QEvent::KeyPress)//回车键
        {
            QKeyEvent *k = static_cast<QKeyEvent *>(event);
            if(k->key() == Qt::Key_Return)
            {
                on_send_clicked();
                return true;
            }
        }
    }
    return QWidget::eventFilter(target,event);
}

void BJChatWidget::on_fontComboBox_currentFontChanged(QFont f)//字体设置
{
    ui->textEdit->setCurrentFont(f);
    ui->textEdit->setFocus();
}

//字体大小设置
void BJChatWidget::on_fontsizecomboBox_currentIndexChanged(QString size)
{
    ui->textEdit->setFontPointSize(size.toDouble());
    ui->textEdit->setFocus();
}

void BJChatWidget::on_textbold_clicked(bool checked)
{
    if(checked)
        ui->textEdit->setFontWeight(QFont::Bold);
    else
        ui->textEdit->setFontWeight(QFont::Normal);
    ui->textEdit->setFocus();
}

void BJChatWidget::on_textitalic_clicked(bool checked)
{
    ui->textEdit->setFontItalic(checked);
    ui->textEdit->setFocus();
}

void BJChatWidget::on_textUnderline_clicked(bool checked)
{
    ui->textEdit->setFontUnderline(checked);
    ui->textEdit->setFocus();
}

void BJChatWidget::on_textcolor_clicked()
{
    color = QColorDialog::getColor(color,this);
    if(color.isValid())
    {
        ui->textEdit->setTextColor(color);
        ui->textEdit->setFocus();
    }
}

void BJChatWidget::on_save_clicked()//保存聊天记录
{
    if(ui->textBrowser->document()->isEmpty())
        QMessageBox::warning(0,tr("警告"),tr("聊天记录为空，无法保存！"),QMessageBox::Ok);
    else
    {
        //获得文件名,注意getSaveFileName函数的格式即可
        QString fileName = QFileDialog::getSaveFileName(this,tr("保存聊天记录"),tr("聊天记录"),tr("文本(*.txt);;All File(*.*)"));
        if(!fileName.isEmpty())
            saveFile(fileName);
    }
}

bool BJChatWidget::saveFile(const QString &fileName)//保存文件
{
    QFile file(fileName);
    if(!file.open(QFile::WriteOnly | QFile::Text))

    {
        QMessageBox::warning(this,tr("保存文件"),
                             tr("无法保存文件 %1:\n %2").arg(fileName)
                             .arg(file.errorString()));
        return false;
    }
    QTextStream out(&file);
    out << ui->textBrowser->toPlainText();

    return true;
}

void BJChatWidget::on_clear_clicked()//清空聊天记录
{
    ui->textBrowser->clear();
}


void BJChatWidget::on_tableWidget_doubleClicked(QModelIndex index)
{
    if(ui->tableWidget->item(index.row(),0)->text() == getUserName() &&
            ui->tableWidget->item(index.row(),2)->text() == getIP())
    {
        QMessageBox::warning(0,tr("警告"),tr("你不可以跟自己聊天！！！"),QMessageBox::Ok);
    }
    else
    {
        //	else
        if(!privatechat){
            //  chat *privatechatTemp;
            privatechat = new chat(ui->tableWidget->item(index.row(),1)->text(), //接收主机名
                                   ui->tableWidget->item(index.row(),2)->text()) ;//接收用户IP
        }
        //		if( privatechat->is_opened )delete privatechat;//如果其曾经显示过则删除掉
        QByteArray data;
        QDataStream out(&data,QIODevice::WriteOnly);
        QString localHostName = QHostInfo::localHostName();
        QString address = getIP();
        out << Xchat << getUserName() << localHostName << address;
        udpSocket->writeDatagram(data,data.length(),QHostAddress(ui->tableWidget->item(index.row(),2)->text()), port);

        //		privatechat->xchat->writeDatagram(data,data.length(),QHostAddress::QHostAddress(ui->tableBJChatWidget->item(index.row(),2)->text()), 45456);
        //  if(!privatechat->is_opened)
        privatechat->show();
        privatechat->is_opened = true;
        //	(privatechat->a) = 0;
    }

}

void BJChatWidget::showxchat(QString name, QString ip)
{
    //	if(!privatechat){
    // chat *privatechatTemp;
    if(!privatechat1)
        privatechat1 = new chat(name,ip);
    //	privatechat = privatechatTemp;}
    //	chat privatechat(name,ip);//如果不用new函数，则程序运行时只是闪烁显示一下就没了，因为类的生命周期结束了
    //	privatechat->is_opened = false;
    // privatechat->show();
    //privatechat.textBrowser.show();
    //privatechat->is_opened = true;
    bb++;
    //delete privatechat;

}
