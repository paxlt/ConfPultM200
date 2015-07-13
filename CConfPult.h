#ifndef CCONFPULT_H
#define CCONFPULT_H

#include <QtGui>
//#ifdef ANDROID
#include <QtWidgets>
//#endif

class CInputDialog;
class CConfSMPClient;

class CConfPult : public QDialog
{
    Q_OBJECT

    CConfSMPClient *m_pSmpClient;

    void closeEvent(QCloseEvent *bar);
    void saveSettings( void );
    void loadSettings( void );

public:

    QString m_sHost, m_sPassword;
    int m_nPort, m_nVirtLink;

    QPixmap pix_call_up, pix_call_down, pix_microphone, pix_connection, pix_gather;
    QPixmap pix_state_free, pix_state_preanswer, pix_state_answer, pix_state_release;

    void doConnect( void );
    void doDisconnect( void );
    bool updateConnectionParams( void );
    bool reinitConnection( void );
    void doReconnect( void );

    explicit CConfPult( QWidget *parent = 0 );
    ~CConfPult();

signals:

public slots:

    void slotConnectionClosed();
    void slotConnectionReinit();

};

#endif // CCONFPULT_H
