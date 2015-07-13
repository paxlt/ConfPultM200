#include "CConfCore.h"

#include "CFlowDownLayout.h"
#include "CConfPult.h"
#include "CConfSMPClient.h"
#include "CConfParty.h"

CConfCore::CConfCore( const char *sDev, const char *sName, const char *sAon, int nMixPlata, int nMaxActivePorts, int nPartys,  QWidget *parent ) : QGroupBox(parent)
{
    m_pOwner = qobject_cast<CConfSMPClient*>(parent);
    if(!m_pOwner)
        QCoreApplication::exit(EXIT_FAILURE);

    QTextCodec *codec = QTextCodec::codecForName("Windows-1251");
    QString name = codec->toUnicode(sName);
    if(name.isEmpty())
        name = QString::number(m_pOwner->pOwner()->m_nVirtLink);

    QString title;
    title = name;
    title += '@';

    QString connected = m_pOwner->pOwner()->m_sHost;
    if(m_pOwner->pOwner()->m_nPort != 10011)
    {
        connected += ':';
        connected += QString::number(m_pOwner->pOwner()->m_nPort);
    }
    title += connected;
    title += " (";
    title += sDev;
    title += ')';

    setAlignment(Qt::AlignHCenter);
    setTitle(title);

    QString tooltip;
    tooltip += tr("Название");
    tooltip += ": ";
    tooltip += name;
    tooltip += '\n';

    tooltip += tr("Номер");
    tooltip += ": ";
    tooltip += sAon;
    tooltip += '\n';

    tooltip += tr("Подключен");
    tooltip += ": ";
    tooltip += connected;
    tooltip += '\n';

    tooltip += tr("Слот");
    tooltip += ": ";
    tooltip += QString::number(m_pOwner->pOwner()->m_nVirtLink - 768);
    tooltip += '\n';

    tooltip += tr("Плата");
    tooltip += ": ";
    tooltip += QString::number(nMixPlata);
    tooltip += '\n';

    tooltip += tr("Максимум активных");
    tooltip += ": ";
    tooltip += QString::number(nMaxActivePorts);

    setToolTip(tooltip);

    m_nMaxActivePorts = nMaxActivePorts;
#ifdef ANDROID
    FlowLayout* pLayout = new FlowLayout(6, 10, 7);
#else
    FlowLayout* pLayout = new FlowLayout(8, 8, 6);
#endif
    for(int i = 0; i < nPartys; i++)
    {
        CConfParty* party = new CConfParty(this);
        m_pPartys.push_back(party);
        pLayout->addWidget(party);
    }

    QVBoxLayout* pTopLayout = new QVBoxLayout;

    QScrollArea* scrl = new QScrollArea();
#ifdef ANDROID
    QScroller::grabGesture(scrl, QScroller::LeftMouseButtonGesture);
    QScroller* s = QScroller::scroller(scrl);
    QScrollerProperties p = s->scrollerProperties();
    p.setScrollMetric(QScrollerProperties::HorizontalOvershootPolicy, QScrollerProperties::OvershootAlwaysOff);
    p.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy, QScrollerProperties::OvershootAlwaysOff);
    p.setScrollMetric(QScrollerProperties::OvershootDragDistanceFactor, 0);
    p.setScrollMetric(QScrollerProperties::OvershootScrollDistanceFactor, 0);

    //p.setScrollMetric(QScrollerProperties::OvershootDragResistanceFactor,  0.9);
    //p.setScrollMetric(QScrollerProperties::DragStartDistance,   0.001 );
    //p.setScrollMetric(QScrollerProperties::ScrollingCurve, QEasingCurve::Linear );
    //p.setScrollMetric(QScrollerProperties::AxisLockThreshold, 0.9);

    //Does this help?
    //p.setScrollMetric(QScrollerProperties::SnapTime, 1);
    //p.setScrollMetric(QScrollerProperties::SnapPositionRatio, 1);

    //p.setScrollMetric(QScrollerProperties::AcceleratingFlickSpeedupFactor, 2.5);
    //p.setScrollMetric(QScrollerProperties::AcceleratingFlickMaximumTime, 1.25);
    s->setScrollerProperties(p);
#endif
    QWidget* wgt = new QWidget();
    scrl->setWidget(wgt);
    scrl->setWidgetResizable(true);
    wgt->setLayout(pLayout);
    pTopLayout->addWidget(scrl);

    m_pBtnGather = new QPushButton(tr("Собрать всех"));
    m_pBtnGather->setIcon(m_pOwner->pOwner()->pix_gather);
    connect(m_pBtnGather, SIGNAL(clicked()), SLOT(slotGatherClicked()));
    m_pBtnGather->setToolTip(tr("Вызвать всех участников конференции"));

    m_pBtnDestroy = new QPushButton(tr("Отбить всех"));
    m_pBtnDestroy->setIcon(m_pOwner->pOwner()->pix_call_down);
    connect(m_pBtnDestroy, SIGNAL(clicked()), SLOT(slotDestroyClicked()));
    m_pBtnDestroy->setToolTip(tr("Отбить всех участников конференции"));

    m_pBtnReconnect = new QPushButton(tr("Обновить соединение"));
    m_pBtnReconnect->setIcon(m_pOwner->pOwner()->pix_connection);
    connect(m_pBtnReconnect, SIGNAL(clicked()), SLOT(slotReconnectClicked()));
    m_pBtnReconnect->setToolTip(tr("Обновить соединение с АТС"));

    QHBoxLayout* pLayoutH = new QHBoxLayout;
    pLayoutH->addWidget(m_pBtnGather);
    pLayoutH->addWidget(m_pBtnDestroy);
    pLayoutH->addWidget(m_pBtnReconnect);
    pTopLayout->addLayout(pLayoutH);
    pTopLayout->setMargin(4);
    setLayout(pTopLayout);
}

CConfCore::~CConfCore()
{
}

CConfParty *CConfCore::pPartyByIdx( int idx ) const
{
    foreach (CConfParty* pparty, m_pPartys)
    {
       if(pparty->nIdx() == idx)
           return pparty;
    }
    return NULL;
}

void CConfCore::slotGatherClicked()
{
    CConfPultMessage mes(m_pOwner->pOwner()->m_nVirtLink, CConfPultMessage::CALL);
    mes << (unsigned char)255;
    mes << true;
    mes << "";
    if(!mes.error())
        m_pOwner->sendPacket(mes.constThis(), (quint16)mes.size());
}

void CConfCore::slotDestroyClicked()
{
    CConfPultMessage mes(m_pOwner->pOwner()->m_nVirtLink, CConfPultMessage::RELEASE);
    mes << (unsigned char)255;
    if(!mes.error())
        m_pOwner->sendPacket(mes.constThis(), (quint16)mes.size());
}

void CConfCore::slotReconnectClicked()
{
    emit reconnectClicked();
}
