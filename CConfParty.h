#ifndef CCONFPARTY_H
#define CCONFPARTY_H

#include <QtGui>
//#ifdef ANDROID
#include <QtWidgets>
//#endif

class CConfCore;

class CConfParty : public QGroupBox
{
    Q_OBJECT

    int m_idx;
    int m_nState;
    CConfCore* m_pOwner;

    QString m_name;
    QLineEdit *m_pNumber;
    QPushButton *m_pCall;
    QPushButton *m_pRel;
    QPushButton *m_pActpas;
    QPushButton *m_pState;
    QPalette m_Palete;

    QTimer* pTimerPreRelease;
    QTimer* pTimerAnswerAsk;
    QTimer* pTimerInPreanswer;
    int nTimerInPreanswerCounter;
    int nTimerAnswerAskCounter;

    QGraphicsColorizeEffect *m_effect;
    QPropertyAnimation *m_animation;

    void changeTooltip( void );
    void disableButton(  QPushButton* pBtn ) { if(pBtn->isEnabled()) pBtn->setEnabled(false); }
    void enableButton(  QPushButton* pBtn ) { if(!pBtn->isEnabled()) pBtn->setEnabled(true); }

public:

    int nIdx( void ) const { return m_idx; }
    void initIdx( int idx ) { m_idx = idx; m_name = QString::number(idx); m_name += '.'; }

    void initNumber( const char* sNumber );
    void initActive( bool fActive );
    void initState( int nState );
    void setNumber( const char* sNumber );
    void setName( const char* sNumber );
    void setState( int nState );
    void setActive( bool fActive );

    explicit CConfParty( QWidget* parent );
    ~CConfParty();

private slots:

    void slotUpdate();
    void slotCallClicked();
    void slotRelClicked();
    void slotActPasClicked();
    void slotStateClicked();

    void slotTimerPreRelease();
    void slotTimerAskAtive();
    void slotTimerInPreanswer();
};

#endif // CCONFPARTY_H
