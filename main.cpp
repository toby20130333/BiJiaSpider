#include "bjmainwidget.h"
#include "bjwidget.h"
#include <QApplication>
#include "fvupdater.h"
#define UPDATE_URL "http://qtclub.qiniudn.com/update3.xml"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // Prerequisite for the Fervor updater
    QApplication::setOrganizationName("pypt");
    QApplication::setOrganizationDomain("pypt.lt");
    QApplication::setApplicationVersion("2.0");
    QApplication::setApplicationName("BijiaTools");

    BJMainWidget* stackView = new BJMainWidget;
    stackView->show();

    // Set feed URL before doing anything else
    FvUpdater::sharedUpdater()->SetFeedURL(UPDATE_URL);

    // Check for updates automatically
//    FvUpdater::sharedUpdater()->CheckForUpdatesSilent();
    return a.exec();
}
