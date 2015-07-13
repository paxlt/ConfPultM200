#ifndef CSMPCLIENT_H
#define CSMPCLIENT_H

#include <QtGui>
#include <QtNetwork>
//#ifdef ANDROID
#include <QtWidgets>
//#endif

#include "../ConfPultMessage.h"

class CConfPult;
class CConfCore;

const int m_nTimerBase = 1000;

class CConfSMPClient : public QWidget
{
    Q_OBJECT

    bool fOk;
    int m_nTimerSkip, m_nTimes;
    CConfPult *m_pOwner;
    CConfCore *m_pConfCore;
    QTcpSocket *m_pTcpSocket;
    QProgressDialog *m_pProgress;

    bool m_fBinaryRead, m_fBinaryWrite;
    int m_recvLen;
    char m_recvBuf[8192];
    char m_sendBuf[4096];

    void onBinaryModeOk( void );
    void receivePacket( char *data, quint16 len );
    void onConfPultMessage( CConfPultMessage *pMes );
    void updateConfPartys( int count, CConfPultMessage *pMes );

private:

    virtual void timerEvent( QTimerEvent* ev );

public:

    void sendPacket( const char* data, quint16 len );
    CConfPult *pOwner() const { return m_pOwner; }
    void onConnetionKazus( const QString& sReason, bool fReinit = true, bool fCritical = true );
    explicit CConfSMPClient( QWidget *parent );
    ~CConfSMPClient();

signals:

private slots:

     void slotReadyRead();
     void slotError( QAbstractSocket::SocketError socketError );
     void slotConnected();
     void slotDisconnected();
     void slotCanceled();
};

#endif // CSMPCLIENT_H
