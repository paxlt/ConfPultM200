#include "CConfPult.h"

#include "CInputDialog.h"
#include "CConfSMPClient.h"
#include <QtGui>
#include <QVBoxLayout>

extern unsigned int SMP_VERSION;

CConfPult::CConfPult( QWidget *parent ) : QDialog(parent, Qt::Window | Qt::WindowTitleHint | Qt::WindowMinMaxButtonsHint)
{
    setWindowIcon(QIcon(":resource/images/icon.png"));
    QString sTitle = tr("Пульт конференцсвязи");
    sTitle += " motor ";
    QString sMyVer;
    sMyVer.sprintf(" %d.%d%d.%d%d",
            SMP_VERSION / 10000 % 10,
            SMP_VERSION / 1000 % 10,
            SMP_VERSION / 100 % 10,
            SMP_VERSION / 10 % 10,
            SMP_VERSION / 1 % 10);
    sTitle += sMyVer;
    setWindowTitle(sTitle);

    pix_call_up.load(":resource/images/phone_up.png");
    pix_call_down.load(":resource/images/phone_down.png");
    pix_microphone.load(":resource/images/microphone.png");
    pix_connection.load(":resource/images/connection.png");
    pix_gather.load(":resource/images/gather.png");

    pix_state_free.load(":resource/images/state_free.png");
    pix_state_preanswer.load(":resource/images/state_preanswer.png");
    pix_state_answer.load(":resource/images/state_answer.png");
    pix_state_release.load(":resource/images/state_release.png");

    m_pSmpClient = NULL;

    loadSettings();

    QVBoxLayout* pTopLayout = new QVBoxLayout;
    setLayout(pTopLayout);
    pTopLayout->setMargin(2);
}

void CConfPult::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    saveSettings();
}

CConfPult::~CConfPult()
{
    saveSettings();
}

void CConfPult::saveSettings()
{
    QSettings settings;
    settings.setValue("host", m_sHost);
    settings.setValue("port", m_nPort);
    settings.setValue("password", m_sPassword);
    settings.setValue("slot", m_nVirtLink - 768);
    settings.sync();
}

void CConfPult::loadSettings()
{
    QSettings settings;
    m_sHost = settings.value("host", "192.168.0.1").toString();
    m_nPort = settings.value("port", 10011).toInt();
    m_sPassword = settings.value("password", "100100").toString();
    m_nVirtLink = settings.value("slot", 1).toInt() + 768;
}

void CConfPult::doConnect( void )
{
    m_pSmpClient = new CConfSMPClient(this);
    layout()->addWidget(m_pSmpClient);
}

void CConfPult::doDisconnect( void )
{
    if(m_pSmpClient)
    {
        delete m_pSmpClient;
        m_pSmpClient = NULL;
    }
}

bool CConfPult::updateConnectionParams( void )
{
    CInputDialog* pInputDialog = new CInputDialog(this);

#ifdef ANDROID
    static bool first = true;
    if(first)
    {
        first = false;
        pInputDialog->showMaximized();
    }
#endif

    int ret = pInputDialog->exec();
    if (ret != QDialog::Accepted)
    {
        QCoreApplication::quit();
        return false;
    }

    m_sHost = pInputDialog->m_sHost->text();
    m_sPassword = pInputDialog->m_sPassword->text();
    m_nPort = pInputDialog->m_sPort->text().toInt();
    m_nVirtLink = pInputDialog->m_nSlot->value() + 768;

    delete pInputDialog;
    return true;
}

void CConfPult::doReconnect( void )
{
    doDisconnect();
    doConnect();
}

bool CConfPult::reinitConnection( void )
{
    doDisconnect();
    bool ret = updateConnectionParams();
    doConnect();
    return ret;
}

void CConfPult::slotConnectionClosed( )
{
    doReconnect();
}

void CConfPult::slotConnectionReinit()
{
    reinitConnection();
}

