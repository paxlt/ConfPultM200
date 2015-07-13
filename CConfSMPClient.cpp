#include "CConfSMPClient.h"

#include "CConfPult.h"
#include "CConfCore.h"
#include "CConfParty.h"

extern unsigned int SMP_VERSION;

CConfSMPClient::CConfSMPClient( QWidget *parent ) : QWidget( parent )
{
    m_pOwner = qobject_cast<CConfPult*>(parent);
    if(!m_pOwner)
        QCoreApplication::exit(EXIT_FAILURE);

    m_fBinaryRead = m_fBinaryWrite = false;
    m_recvLen = 0;

    m_nTimerSkip = m_nTimes = 0;
    m_pConfCore = NULL;

    m_pTcpSocket = new QTcpSocket(this);
    connect(m_pTcpSocket, SIGNAL(connected()), SLOT(slotConnected()));
    connect(m_pTcpSocket, SIGNAL(readyRead()), SLOT(slotReadyRead()));
    connect(m_pTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotError(QAbstractSocket::SocketError)));

    m_pProgress = new QProgressDialog(tr("Подключаемся к АТС..."), tr("Отмена"), 0, 5, this);
    connect(m_pProgress, SIGNAL(canceled()), SLOT(slotCanceled()));
    m_pProgress->setWindowTitle(tr("Пульт конференцсвязи"));
    m_pProgress->setMinimumDuration(0);
    m_pProgress->setAutoClose(true);
    m_pProgress->show();

    QVBoxLayout* pTopLayout = new QVBoxLayout;
    pTopLayout->setMargin(4);
    setLayout(pTopLayout);

    fOk = true;
    m_pTcpSocket->connectToHost(m_pOwner->m_sHost, m_pOwner->m_nPort);
    m_pProgress->setValue(1);
}

CConfSMPClient::~CConfSMPClient()
{
}

void CConfSMPClient::receivePacket( char *data, quint16 len )
{
    if (!m_fBinaryRead)
        return;

    CConfPultMessage *pMes = reinterpret_cast<CConfPultMessage*>(data);
    if(!len || pMes->size() != (int)len)
    {
        onConnetionKazus(tr("Ошибка длинны сообщения"));
        return;
    }
    onConfPultMessage(pMes);
}

void CConfSMPClient::sendPacket( const char *data, quint16 len )
{
    if (m_fBinaryWrite)
    {
        memcpy(m_sendBuf, &len, sizeof(quint16));
        memcpy(m_sendBuf + sizeof(quint16), data, len);
        len += sizeof(quint16);
        data = m_sendBuf;
    }

    if((quint64)m_pTcpSocket->write(data, (quint64)len) != (quint64)len)
    {
        onConnetionKazus(tr("Ошибка при отправке сообщения"));
        return;
    }
}

void CConfSMPClient::onBinaryModeOk()
{
    CConfPultMessage mes(m_pOwner->m_nVirtLink, CConfPultMessage::GET_ALL);
    if(!mes.error())
        sendPacket(mes.constThis(), mes.size());
    m_pProgress->setValue(3);
}

void CConfSMPClient::timerEvent( QTimerEvent* ev )
{
    Q_UNUSED(ev);
    if(!m_pConfCore)
        return;

    CConfPultMessage mes(m_pOwner->m_nVirtLink, CConfPultMessage::GET_STATE);

    int nPart = m_nTimerSkip++ % m_nTimes;
    unsigned char nPartyStartIdx = (unsigned char)m_pConfCore->pParty((31 * nPart))->nIdx();
    unsigned char nPartyCount = m_pConfCore->nPartySize() - (unsigned char)(31 * nPart);
    if(nPartyCount > 31)
        nPartyCount = 31;

    mes << nPartyStartIdx;
    mes << nPartyCount;
    if(!mes.error())
        sendPacket(mes.constThis(), mes.size());
}

void CConfSMPClient::updateConfPartys( int count, CConfPultMessage* pMes )
{
    for(int i = 0; i < count; i++)
    {

        unsigned char nIdx;
        *pMes >> nIdx;

        CConfParty *party =  m_pConfCore->pPartyByIdx((int)nIdx);
        if(!party)
            continue;

        unsigned char nState;
        *pMes >> nState;
        party->setState((int)nState);

        bool fActive;
        *pMes >> fActive;
        party->setActive(fActive);

        char sName[100];
        *pMes >> sName;
        party->setName(sName);

        char sNumber[100];
        *pMes >> sNumber;
        party->setNumber(sNumber);
    }
}

void CConfSMPClient::onConfPultMessage( CConfPultMessage *pMes )
{
   switch (pMes->com())
   {
       case CConfPultMessage::TAKE_ALL:
       {
            m_pProgress->setValue(4);
            pMes->start();

            unsigned int nVer;
            *pMes >> nVer;

            if(nVer != SMP_VERSION)
            {
                QString sMyVer;
                sMyVer.sprintf(" %d.%d%d.%d%d ",
                        SMP_VERSION / 10000 % 10,
                        SMP_VERSION / 1000 % 10,
                        SMP_VERSION / 100 % 10,
                        SMP_VERSION / 10 % 10,
                        SMP_VERSION / 1 % 10);
                QString sVer;
                sVer.sprintf(" %d.%d%d.%d%d",
                        nVer / 10000 % 10,
                        nVer / 1000 % 10,
                        nVer / 100 % 10,
                        nVer / 10 % 10,
                        nVer / 1 % 10);
                QString sMes;
                sMes = tr("Версия");
                sMes += " motor ";
                sMes += tr("пульта");
                sMes += sMyVer;
                sMes += tr("отличается от версии");
                sMes += " motor ";
                sMes += tr("АТС");
                sMes += sVer;
                QMessageBox::warning(this, tr("Пульт конференцсвязи"), sMes);
            }

            char sDev[10];
            *pMes >> sDev;

            char sName[100];
            *pMes >> sName;

            char sNumber[100];
            *pMes >> sNumber;

            unsigned char nMixPlata;
            *pMes >> nMixPlata;

            unsigned char nMaxActivePorts;
            *pMes >> nMaxActivePorts;

            unsigned char nPartys;
            *pMes >> nPartys;

            m_pConfCore = new CConfCore(sDev, sName, sNumber, nMixPlata, nMaxActivePorts, nPartys, this);

            for(int i = 0; i < (int)nPartys; i++)
            {
                CConfParty *party =  m_pConfCore->pParty(i);
                if(!party)
                    continue;

                unsigned char nIdx;
                *pMes >> nIdx;
                party->initIdx((int)nIdx);

                unsigned char nState;
                *pMes >> nState;
                party->initState((int)nState);

                bool fActive;
                *pMes >> fActive;
                party->initActive(fActive);

                *pMes >> sName;
                party->setName(sName);

                *pMes >> sNumber;
                party->initNumber(sNumber);
            }

            connect(m_pConfCore, SIGNAL(reconnectClicked()), SLOT(slotCanceled()));
            layout()->addWidget(m_pConfCore);

            m_pProgress->setValue(5);

            m_nTimes = nPartys / 31;
            if (nPartys % 31 || !m_nTimes)
                m_nTimes++;
            startTimer(m_nTimerBase / m_nTimes);
       }
       break;

       case CConfPultMessage::TAKE_STATE:
       {
            pMes->start();

            unsigned char nPartyCount;
            *pMes >> nPartyCount;

            updateConfPartys((int)nPartyCount, pMes);
       }
       break;

       case CConfPultMessage::VIRT_LINK_ERROR:
       {
            onConnetionKazus(tr("Неверный номер виртуального слота или слот не настроен. Проверьте настройки соединения"));
       }
       break;

       case CConfPultMessage::RECONFIGURED:
       {
            onConnetionKazus(tr("Изменена конфигурация АТС"), false, false);
       }
       break;

       default:;
   }
}

void CConfSMPClient::onConnetionKazus( const QString& sReason, bool fReinit , bool fCritical )
{
    if(!fOk)
        return;
    fOk = false;

    if(m_pProgress)
        m_pProgress->hide();

    if(m_pTcpSocket)
        m_pTcpSocket->close();

    QApplication::alert(m_pOwner);

    if(!sReason.isEmpty())
    {
        if(fCritical)
            QMessageBox::critical(this, tr("Пульт конференцсвязи"), sReason);
        else
            QMessageBox::information(this, tr("Пульт конференцсвязи"), sReason);
    }

    if(fReinit)
        QTimer::singleShot(100, m_pOwner, SLOT(slotConnectionReinit()));
    else
        QTimer::singleShot(100, m_pOwner, SLOT(slotConnectionClosed()));
}

void CConfSMPClient::slotReadyRead()
{
    qint64 len = m_pTcpSocket->read(m_recvBuf + m_recvLen, (qint64)((int)sizeof(m_recvBuf) - m_recvLen));
    if (!len)
    {
        onConnetionKazus(tr("Соединение закрыто"));
        return;
    }

    m_recvLen += (int)len;

    if (!m_fBinaryRead)
    {
        int done = 0;
        for (int p = 0; p < m_recvLen && !m_fBinaryRead; )
        {
            if (m_recvBuf[p] == 0xA)
            {
                memmove(m_recvBuf + p, m_recvBuf + p + 1, m_recvLen - p - 1);
                m_recvLen--;
                continue;
            }
            if (m_recvBuf[p] == 0xD)
            {
                if (!strncmp((char *)(m_recvBuf + done), "BINARYMODE-OK", 10 + 1 + 2))
                {
                    m_fBinaryRead = true;
                    onBinaryModeOk();
                }
                else if (!strncmp((char *)(m_recvBuf + done), "BINARYMODE-ER", 10 + 1 + 2))
                {
                    onConnetionKazus(tr("Неверный пароль! Проверьте настройки соединения"));
                    return;
                }

                p++;
                if (m_recvBuf[p] == 0xA)
                    p++;
                done = p;
            }
            else
                p++;
        }

        if (done < m_recvLen)
        {
            memmove(m_recvBuf, m_recvBuf  + done, m_recvLen - done);
            m_recvLen -= done;
        }
        else
            m_recvLen = 0;
    }

    if (m_fBinaryRead)
    {
        while (m_recvLen >= 2)
        {
            quint16 size;
            ((unsigned char*)&size)[0] = ((unsigned char*)m_recvBuf)[0];
            ((unsigned char*)&size)[1] = ((unsigned char*)m_recvBuf)[1];

            if (size + 2 > (quint16)m_recvLen)
                return;

            receivePacket(m_recvBuf + 2, size);

            if (m_recvLen > size + 2)
            {
                memmove(m_recvBuf, m_recvBuf + size + 2, m_recvLen - size - 2);
                m_recvLen -= size + 2;
            }
            else
                m_recvLen = 0;
        }
    }
}

void CConfSMPClient::slotConnected()
{
    m_pProgress->setValue(2);
    QString str = "BINARYMODE>";
    str += m_pOwner->m_sPassword;
    str += "\r\n";
    QByteArray ba = str.toLatin1();
    sendPacket(ba.constData(), ba.size());
    m_fBinaryWrite = true;
}

void CConfSMPClient::slotDisconnected()
{
    onConnetionKazus(QString(tr("Соединение потеряно")));
}

void CConfSMPClient::slotCanceled()
{
    onConnetionKazus(QString(""));
}

void CConfSMPClient::slotError( QAbstractSocket::SocketError socketError )
{
    switch (socketError)
    {
        case QAbstractSocket::RemoteHostClosedError:
        onConnetionKazus(tr("Ошибка соединения"));
        break;
        case QAbstractSocket::HostNotFoundError:
        onConnetionKazus(tr("АТС не найдена. Проверьте настройки соединения"));
        break;
        case QAbstractSocket::ConnectionRefusedError:
        onConnetionKazus(tr("Отказ подключения. Проверьте корректность настроек соединения"));
        break;
        default:
        onConnetionKazus(tr("Ошибка соединения: %1.").arg(m_pTcpSocket->errorString()));
    }
}
