#include "widget.h"
#include <QApplication>
#include "fvupdater.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // Prerequisite for the Fervor updater
    QApplication::setOrganizationName("pypt");
    QApplication::setOrganizationDomain("pypt.lt");

    Widget w;
    w.show();
    // Set feed URL before doing anything else
    FvUpdater::sharedUpdater()->SetFeedURL("http://pypt.github.com/fervor/Appcast.xml");

    // Check for updates automatically
    FvUpdater::sharedUpdater()->CheckForUpdatesSilent();
    return a.exec();
}
