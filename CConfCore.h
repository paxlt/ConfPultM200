#ifndef CCONFCORE_H
#define CCONFCORE_H

#include <QtGui>
//#ifdef ANDROID
#include <QtWidgets>
//#endif

class CConfSMPClient;
class CConfParty;

class CConfCore : public QGroupBox
{
    Q_OBJECT

    QVector<CConfParty*> m_pPartys;
    int m_nMaxActivePorts;
    CConfSMPClient* m_pOwner;

    QPushButton *m_pBtnGather, *m_pBtnDestroy, *m_pBtnReconnect;

public:

    explicit CConfCore( const char *sDev, const char *sName, const char *sAon, int nMixPlata, int nMaxActivePorts,  int nPartys, QWidget *parent );
    ~CConfCore();

    CConfSMPClient *pOwner() const { return m_pOwner; }
    CConfParty* pParty( int i ) const { return (unsigned int)i < (unsigned int)m_pPartys.size() ? m_pPartys[i] : NULL; }
    CConfParty *pPartyByIdx( int idx ) const;
    unsigned char nPartySize( void ) const { return (unsigned char)m_pPartys.size(); }

signals:

    void reconnectClicked();

private slots:

    void slotGatherClicked();
    void slotDestroyClicked();
    void slotReconnectClicked();
};

#endif // CCONFCORE_H
