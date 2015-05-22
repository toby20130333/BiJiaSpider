#ifndef BJWIDGET_H
#define BJWIDGET_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDebug>
#include <QWebFrame>
#include <QWebElement>
#include <QWebView>
#include <QUrl>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMouseEvent>
#include "chat/chat.h"
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QUrlQuery>
#include <QtMath>
#else
#include <QtGui>
#endif

#include "bijiaarkthread.h"
#include "bijiabehthread.h"
#include "bijiacombthread.h"
#include "bijiaoakthread.h"
#include "bijiaaokthread.h"
#include "bijiaastatechthread.h"

#include "bjsystem_tray.h"

#define ARKPHARM_URL   "http://www.arkpharminc.com"
#define BEPHARM_URL     "http://www.bepharm.com/en/Search.html?searchValue="
#define COMBLOCKS_URL "http://www.combi-blocks.com/cgi-bin/find.cgi"

#define ARKFINAL_URL "http://www.arkpharminc.com/default.ashx?xmls=lib/stockprice.xmls&method=getstock&GoodsCode="
#define DEFAULT "http://www.arkpharminc.com/default.ashx?xmls=lib/stockprice.xmls&method=getstock&GoodsCode="
namespace Ui {
class BJWidget;
}
typedef struct {
    QString id;
    QString price;
    QString value;
    QUrl url;
}DATA_M;

class BJWidget : public QWidget
{
    Q_OBJECT
    enum WEBSITE_TYPE{
        BEPHARM = 0,
        ARKPHARM,
        COMBLOCKS
    };
public:
    explicit BJWidget(QWidget *parent = 0);
    ~BJWidget();
    QWebView* m_View;
    SystemTray* m_Tray;
    void initThreeThreads();
    void initDatabase();
    void initTrayicon();

    void setUrl(const QUrl &url);
    void setWebSiteType(WEBSITE_TYPE type);
    void startLoad(const QUrl & url);
    void startPost(const QUrl & url);

    BiJiaARKThread* m_ARK;
    BiJiaBehThread* m_BEH;
    BiJiaCOMBThread* m_COMB;
    BiJiaOAKThread* m_OAK;
    BiJiaAOKThread* m_AOK;
    BiJiaAstatechThread* m_Astatech;

    bool arkIsOK;
    bool behIsOk;
    bool comBlOk;
    bool oakOK;
    bool aokOK;
    bool astateChOK;

public slots:
    void examineChildElements(const QWebElement &parentElement, bool isBeh);

    void examineChildElementsOfAstatech(const QWebElement &parentElement, bool isBeh);


    void parseArk(const QByteArray& arr);
    void parseBeh(const QByteArray& arr,bool isFirst);
    void parseComBlock(const QByteArray& arr,bool isFirst);
    void parseOAkChemical(const QByteArray& arr,bool isFirst);
    void parseAoK(const QByteArray& arr);
    void parseAstatech(const QByteArray& arr,bool isFirst);

    void appendStatus(QString);
    void operatorData(DATA_M stu);

    void slotRecevieMsg(MessageType type,const QString& msg);
private slots:
    void on_pushButton_clicked();
    void setCurrentCas(QString);
    void on_closeBtn_clicked();
    void slotActivated(QSystemTrayIcon::ActivationReason);
private:
    Ui::BJWidget *ui;
    WEBSITE_TYPE iType;
    QList<DATA_M> dataStuList;
    bool isFirstBe;
};

#endif // WIDGET_H
