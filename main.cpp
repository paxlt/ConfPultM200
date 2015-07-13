#include "CConfPult.h"
#include "CInputDialog.h"
#include <QTextCodec>
#include <QtGui>
#include <QApplication>
//#ifdef ANDROID
#include <QtWidgets>
//#endif
unsigned int
#include "../../../Make.versions"
;

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("MTA");
    QCoreApplication::setApplicationName("confpultm200");
#ifdef WIN32
    //QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
#endif
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
#ifdef WIN32
    //QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
#endif

    QApplication a(argc, argv);
    CConfPult Pult;
    if(!Pult.reinitConnection())
        return 0;

    Pult.showMaximized();
    return a.exec();
}
