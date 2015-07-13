#include "CConfParty.h"

#include "CConfPult.h"
#include "CConfSMPClient.h"
#include "CConfCore.h"

#include <QVBoxLayout>

#ifdef ANDROID
const int label_stste_size = 42;
const int button_size = 48;
const int button_icon_size = 44;
#else
const int label_stste_size = 26;
const int button_size = 28;
const int button_icon_size = 18;
#endif


CConfParty::CConfParty( QWidget* parent ) : QGroupBox(parent)
{
    m_pOwner = qobject_cast<CConfCore*>(parent);
    if(!m_pOwner)
        QCoreApplication::exit(EXIT_FAILURE);

    m_idx = 0;
    m_nState = 0;

    setAlignment(Qt::AlignHCenter);

    m_pNumber = new QLineEdit();
    m_pNumber->setAlignment(Qt::AlignHCenter);
    QRegExp regexp = QRegExp("[0-9]+");
    QRegExpValidator* pValid = new QRegExpValidator(regexp, m_pNumber);
    m_pNumber->setValidator(pValid);

    m_pState = new QPushButton();
    connect(m_pState, SIGNAL(clicked()), SLOT(slotStateClicked()));
    m_pState->setIcon(m_pOwner->pOwner()->pOwner()->pix_state_free);
    m_pState->setFixedSize(label_stste_size, label_stste_size);
    m_pState->setFixedSize(button_size + 2, button_size);
    m_pState->setIconSize(QSize(label_stste_size, label_stste_size));
    m_pState->setFlat(true);
    m_pState->setCheckable(true);

    m_pCall = new QPushButton();
    connect(m_pCall, SIGNAL(clicked()), SLOT(slotCallClicked()));
    m_pCall->setIcon(m_pOwner->pOwner()->pOwner()->pix_call_up);
    m_pCall->setFixedSize(button_size + 2, button_size);
    m_pCall->setIconSize(QSize(button_icon_size + 2, button_icon_size + 2));
    m_pCall->setToolTip(tr("Вызвать участника"));

    m_pRel = new QPushButton();
    connect(m_pRel, SIGNAL(clicked()), SLOT(slotRelClicked()));
    m_pRel->setIcon(m_pOwner->pOwner()->pOwner()->pix_call_down);
    m_pRel->setFixedSize(button_size + 2, button_size);
    m_pRel->setIconSize(QSize(button_icon_size, button_icon_size));
    m_pRel->setToolTip(tr("Отбить участника"));

    m_pActpas = new QPushButton();
    connect(m_pActpas, SIGNAL(clicked()), SLOT(slotActPasClicked()));
    m_pActpas->setIcon(m_pOwner->pOwner()->pOwner()->pix_microphone);
    m_pActpas->setFixedSize(button_size + 2, button_size);
    m_pActpas->setIconSize(QSize(button_icon_size, button_icon_size));
    m_pActpas->setCheckable(true);

    QVBoxLayout* pLayoutV = new QVBoxLayout;
    pLayoutV->addWidget(m_pNumber);

    QHBoxLayout* pLayoutH = new QHBoxLayout;

    pLayoutH->addWidget(m_pState);
    pLayoutH->addWidget(m_pCall);
    pLayoutH->addWidget(m_pRel);
    pLayoutH->addWidget(m_pActpas);
    pLayoutV->addLayout(pLayoutH);
#ifdef ANDROID
    pLayoutH->setSpacing(6);
    pLayoutV->setMargin(6);
#else
    pLayoutH->setSpacing(1);
    pLayoutV->setMargin(5);
#endif
    setLayout(pLayoutV);

    setAutoFillBackground(true);
    m_Palete = palette();
    m_nState = -1;

    pTimerPreRelease = new QTimer(this);
    pTimerAnswerAsk = new QTimer(this);
    pTimerInPreanswer = new QTimer(this);

    connect(pTimerPreRelease, SIGNAL(timeout()), SLOT(slotTimerPreRelease()));
    connect(pTimerAnswerAsk, SIGNAL(timeout()), SLOT(slotTimerAskAtive()));
    connect(pTimerInPreanswer, SIGNAL(timeout()), SLOT(slotTimerInPreanswer()));
}

CConfParty::~CConfParty()
{
}

void CConfParty::changeTooltip( void )
{
    QString tooltip = QString::number(m_idx);
    tooltip += ". Участник";
    if(!m_name.isEmpty())
    {
        tooltip += tr(" с именем \"");
        tooltip += m_name;
        tooltip += '\"';
    }
    if(!m_pNumber->text().isEmpty())
    {
        tooltip += tr(" с номером ");
        tooltip += m_pNumber->text();
    }
    if(tooltip != toolTip())
        setToolTip(tooltip);
}

void CConfParty::initNumber( const char *sNumber )
{
    QString number(sNumber);
    m_pNumber->setText(number);
    if(!number.isEmpty())
    {
        m_pNumber->setDisabled(true);
        m_pNumber->setToolTip(tr("Номер участника"));
        QPalette p = m_pNumber->palette();
        p.setColor(QPalette::Text, Qt::black);
        m_pNumber->setPalette(p);
    }
    else
        m_pNumber->setToolTip(tr("Введите номер участника"));
    changeTooltip();
}

void CConfParty::setNumber( const char* sNumber )
{
    if(m_pNumber->isEnabled())
        return;

    QString number(sNumber);
    if(number != m_pNumber->text())
    {
        m_pNumber->setText(number);
        changeTooltip();
    }
}

void CConfParty::setName( const char* sName )
{
    QTextCodec *codec = QTextCodec::codecForName("Windows-1251");
    QString name = codec->toUnicode(sName);
    if(name != m_name)
    {
        m_name = name;
        changeTooltip();
        QString title = QString::number(m_idx);
        title += ". ";
        title += name;
        setTitle(title);
    }
}

void CConfParty::setState( int nState )
{
    if(nState == m_nState)
        return;

    int nOldState = m_nState;
    m_nState = nState;

    switch (m_nState)
    {
        case CConfPultMessage::PORT_STATE_FREE:
        {
            m_pState->setIcon(m_pOwner->pOwner()->pOwner()->pix_state_free);
            m_pState->setToolTip(tr("Состояние: свободен"));

            enableButton(m_pCall);
            disableButton(m_pRel);

            if(nOldState != CConfPultMessage::PORT_STATE_BLOCKED)
            {
                QPalette Pal(palette());
                Pal.setColor(QPalette::Background, Qt::magenta);
                setPalette(Pal);

                pTimerPreRelease->start(250);
            }
            else
            {
                QPalette Pal(m_Palete);
                setPalette(Pal);
            }
        }
        break;

        case CConfPultMessage::PORT_STATE_OUT_PREANSWER:
        {
            QPalette Pal(palette());
            Pal.setColor(QPalette::Background, Qt::yellow);
            setPalette(Pal);

            m_pState->setIcon(m_pOwner->pOwner()->pOwner()->pix_state_preanswer);
            m_pState->setToolTip(tr("Состояние: предответное"));

            disableButton(m_pCall);
            enableButton(m_pRel);
        }
        break;

        case CConfPultMessage::PORT_STATE_IN_PREANSWER:
        {
            QPalette Pal(palette());
            Pal.setColor(QPalette::Background, Qt::yellow);
            setPalette(Pal);

            m_pState->setIcon(m_pOwner->pOwner()->pOwner()->pix_state_preanswer);
            m_pState->setToolTip(tr("Состояние: предответное (входящее)"));

            enableButton(m_pCall);
            enableButton(m_pRel);

            pTimerInPreanswer->start(250);
        }
        break;

        case CConfPultMessage::PORT_STATE_ANSWER_ASK:
            pTimerAnswerAsk->start(250);
        case CConfPultMessage::PORT_STATE_ANSWER:
        {
            QPalette Pal(palette());
            Pal.setColor(QPalette::Background, Qt::green);
            setPalette(Pal);

            m_pState->setIcon(m_pOwner->pOwner()->pOwner()->pix_state_answer);
            m_pState->setToolTip(tr("Состояние: ответное"));

            disableButton(m_pCall);
            enableButton(m_pRel);
        }
        break;

        case CConfPultMessage::PORT_STATE_PRERELEASE:
        {
            QPalette Pal(palette());
            Pal.setColor(QPalette::Background, Qt::magenta);
            setPalette(Pal);

            /*
            m_effect = new QGraphicsColorizeEffect;
            m_effect->setColor(Qt::magenta);
            m_effect->setStrength(1);
            m_effect->setEnabled(true);
            setGraphicsEffect(m_effect);

            m_animation = new QPropertyAnimation(m_effect, "color", this);
            m_animation->setDuration(100);
            m_animation->setEasingCurve(QEasingCurve::InOutSine);
            m_animation->setStartValue(palette().color(QPalette::Background));
            m_animation->setEndValue(Qt::magenta);
            m_animation->setDirection(QAbstractAnimation::Forward);
            m_animation->start();

            m_pState->setIcon(m_pOwner->pOwner()->pOwner()->pix_state_release);
            m_pState->setToolTip(tr("Состояние: отбойное"));

            disableButton(m_pCall);
            disableButton(m_pRel);
            */
        }
        break;

        case CConfPultMessage::PORT_STATE_BLOCKED:
        {
            QPalette Pal(palette());
            Pal.setColor(QPalette::Background, Qt::red);
            setPalette(Pal);

            m_pState->setIcon(m_pOwner->pOwner()->pOwner()->pix_state_release);
            m_pState->setToolTip(tr("Состояние: заблокирован"));

            disableButton(m_pCall);
            disableButton(m_pRel);
        }
        break;

        default:;
    }

    if(m_nState != CConfPultMessage::PORT_STATE_ANSWER && m_nState != CConfPultMessage::PORT_STATE_ANSWER_ASK)
        disableButton(m_pActpas);
    else
        enableButton(m_pActpas);
}

void CConfParty::initState(int nState)
{
    if(nState == CConfPultMessage::PORT_STATE_FREE)
    {
        m_nState = nState;
        QPalette Pal(m_Palete);
        setPalette(Pal);

        m_pState->setIcon(m_pOwner->pOwner()->pOwner()->pix_state_free);
        m_pState->setToolTip(tr("Состояние: свободен"));

        enableButton(m_pCall);
        disableButton(m_pRel);
    }
    else
        setState(nState);
}

void CConfParty::initActive( bool fActive )
{
    if(fActive)
    {
        m_pActpas->setChecked(true);
        m_pActpas->setToolTip(tr("Активный участник"));
    }
    else
    {
        m_pActpas->setChecked(false);
        m_pActpas->setToolTip(tr("Пассивный участник"));
    }
}

void CConfParty::setActive( bool fActive )
{
    if(fActive)
    {
        if(!m_pActpas->isChecked())
        {
            m_pActpas->setChecked(true);
            m_pActpas->setToolTip(tr("Активный участник"));
        }
    }
    else
    {
        if(m_pActpas->isChecked())
        {
            m_pActpas->setChecked(false);
            m_pActpas->setToolTip(tr("Пассивный участник"));
        }
    }
}

void CConfParty::slotUpdate()
{
    CConfPultMessage mes(m_pOwner->pOwner()->pOwner()->m_nVirtLink, CConfPultMessage::GET_STATE);
    mes << (unsigned char)m_idx;
    mes << (unsigned char)1;
    if(!mes.error())
        m_pOwner->pOwner()->sendPacket(mes.constThis(), mes.size());
}

void CConfParty::slotCallClicked()
{
    if(m_nState == CConfPultMessage::PORT_STATE_FREE || m_nState == CConfPultMessage::PORT_STATE_IN_PREANSWER)
    {
        CConfPultMessage mes(m_pOwner->pOwner()->pOwner()->m_nVirtLink, CConfPultMessage::CALL);
        mes << (unsigned char)m_idx;
        mes << m_pActpas->isChecked();
        QByteArray ba = m_pNumber->text().toLatin1();
        mes << ba.constData();
        if(!mes.error())
        {
            m_pOwner->pOwner()->sendPacket(mes.constThis(), (quint16)mes.size());
            QTimer::singleShot(50, this, SLOT(slotUpdate()));
        }
    }
}

void CConfParty::slotRelClicked()
{
    if(m_nState != CConfPultMessage::PORT_STATE_FREE && m_nState != CConfPultMessage::PORT_STATE_PRERELEASE)
    {
        CConfPultMessage mes(m_pOwner->pOwner()->pOwner()->m_nVirtLink, CConfPultMessage::RELEASE);
        mes << (unsigned char)m_idx;
        if(!mes.error())
        {
            m_pOwner->pOwner()->sendPacket(mes.constThis(), (quint16)mes.size());
            QTimer::singleShot(50, this, SLOT(slotUpdate()));
        }
    }
}

void CConfParty::slotActPasClicked()
{
    CConfPultMessage mes(m_pOwner->pOwner()->pOwner()->m_nVirtLink, CConfPultMessage::SET_ACTIVE);
    mes << (unsigned char)m_idx;
    mes << m_pActpas->isChecked();
    if(!mes.error())
    {
        m_pOwner->pOwner()->sendPacket(mes.constThis(), (quint16)mes.size());
        QTimer::singleShot(50, this, SLOT(slotUpdate()));
    }
}

void CConfParty::slotStateClicked()
{
    CConfPultMessage mes(m_pOwner->pOwner()->pOwner()->m_nVirtLink, CConfPultMessage::SET_BLOCKED);
    mes << (unsigned char)m_idx;
    mes << m_pState->isChecked();
    if(!mes.error())
    {
        m_pOwner->pOwner()->sendPacket(mes.constThis(), (quint16)mes.size());
        QTimer::singleShot(50, this, SLOT(slotUpdate()));
    }
}

void CConfParty::slotTimerPreRelease()
{
    pTimerPreRelease->stop();
    QPalette Pal(m_Palete);
    setPalette(Pal);
}

void CConfParty::slotTimerAskAtive()
{
    if(m_nState != CConfPultMessage::PORT_STATE_ANSWER_ASK)
    {
        pTimerAnswerAsk->stop();
        return;
    }

    if(nTimerAnswerAskCounter++ % 2)
    {
        QPalette Pal(palette());
        Pal.setColor(QPalette::Background, Qt::green);
        setPalette(Pal);
    }
    else
    {
        QPalette Pal(m_Palete);
        setPalette(Pal);
    }
}

void CConfParty::slotTimerInPreanswer()
{
    if(m_nState != CConfPultMessage::PORT_STATE_IN_PREANSWER)
    {
        pTimerInPreanswer->stop();
        return;
    }

    if(nTimerInPreanswerCounter++ % 2)
    {
        QPalette Pal(palette());
        Pal.setColor(QPalette::Background, Qt::yellow);
        setPalette(Pal);
    }
    else
    {
        QPalette Pal(m_Palete);
        setPalette(Pal);
    }
}
