#ifndef CINPUTDIALOG_H
#define CINPUTDIALOG_H

#include <QtGui>
//#ifdef ANDROID
#include <QtWidgets>
//#endif

class CInputDialog : public QDialog
{
    Q_OBJECT

public:

    QLineEdit* m_sHost;
    QLineEdit* m_sPort;
    QLineEdit* m_sPassword;
    QSpinBox* m_nSlot;

    explicit CInputDialog( QWidget* parent = 0 );
    ~CInputDialog();

private slots:

    void slotPasswordStateChanged( int state );

};

#endif // CINPUTDIALOG_H
