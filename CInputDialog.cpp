#include "CInputDialog.h"

#include <QCheckBox>
#include <QPushButton>
#include <QFormLayout>
#include "CConfPult.h"

CInputDialog::CInputDialog( QWidget* parent ) : QDialog( parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint )
{
    setWindowTitle("Настройки соединения");
    setWindowIcon(QIcon(":resource/images/icon.png"));

    CConfPult *pOwner = qobject_cast<CConfPult*>(parent);

    m_sHost = new QLineEdit;
    m_sHost->setText(pOwner->m_sHost);
    QRegExp regexp_ip = QRegExp("((1{0,1}[0-9]{0,2}|2[0-4]{1,1}[0-9]{1,1}|25[0-5]{1,1})\\.){3,3}(1{0,1}[0-9]{0,2}|2[0-4]{1,1}[0-9]{1,1}|25[0-5]{1,1})");
    QRegExpValidator* pIpValid = new QRegExpValidator(regexp_ip, m_sHost);
    m_sHost->setValidator(pIpValid);
    m_sHost->setToolTip("IP-адрес подключения");

    m_sPort = new QLineEdit;
    m_sPort->setText(QString::number(pOwner->m_nPort));
    QIntValidator* pPortValid = new QIntValidator(0, 65535, m_sPort);
    m_sPort->setValidator(pPortValid);
    m_sPort->setToolTip("Порт подключения");

    m_sPassword = new QLineEdit;
    m_sPassword->setText(pOwner->m_sPassword);
    m_sPassword->setEchoMode(QLineEdit::Password);
    m_sPassword->setInputMask(QString("000000;_"));
    QRegExp regexp_password = QRegExp("[0-9][0-9][0-9][0-9][0-9][0-9]");
    QRegExpValidator* pPasswordValid = new QRegExpValidator(regexp_password, m_sPassword);
    m_sPassword->setValidator(pPasswordValid);
    m_sPassword->setToolTip("Пароль бинарного режима подключения");

    m_nSlot = new QSpinBox;
    m_nSlot->setRange(1, 128);
#ifdef ANDROID
    m_nSlot->setFixedHeight(55);
#endif
    m_nSlot->setValue(pOwner->m_nVirtLink - 768);
    m_nSlot->setToolTip("Номер виртуального слота (Air-а)");

    QCheckBox* pchkShowPassword = new QCheckBox("Показать пароль");
    connect(pchkShowPassword, SIGNAL(stateChanged(int)), SLOT(slotPasswordStateChanged(int)));

    QPushButton* pBtnConnect = new QPushButton("Подключиться");
    pBtnConnect->setIcon(QIcon(":resource/images/apply.png"));
    connect(pBtnConnect, SIGNAL(clicked()), SLOT(accept()));

    QFormLayout* pFormLayout = new QFormLayout;
    pFormLayout->addRow("IP-адрес:", m_sHost);
    pFormLayout->addRow("Порт:", m_sPort);
    pFormLayout->addRow("Пароль:", m_sPassword);
    pFormLayout->addRow("Cлот:", m_nSlot);

    QVBoxLayout* pTopLayout = new QVBoxLayout;
    pTopLayout->addLayout(pFormLayout);
    pTopLayout->addWidget(pchkShowPassword);
    pTopLayout->addWidget(pBtnConnect);
    setLayout(pTopLayout);

    /*
    QRect dt_rect = QApplication::desktop()->availableGeometry();
    QSize win_size = sizeHint();
    int _x = (dt_rect.width() - win_size.width()) >> 1;
    int _y = (dt_rect.height() - win_size.height()) >> 1;
    move(_x, _y);
    */
}

CInputDialog::~CInputDialog()
{
}

void CInputDialog::slotPasswordStateChanged( int state )
{
    m_sPassword->setEchoMode(state ? QLineEdit::Normal : QLineEdit::Password);
}
