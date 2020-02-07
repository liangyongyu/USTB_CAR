/********************************************************************************
** Form generated from reading UI file 'frmudpserver.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FRMUDPSERVER_H
#define UI_FRMUDPSERVER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_frmUdpServer
{
public:
    QGridLayout *gridLayout;
    QTextEdit *txtMain;
    QFrame *frame;
    QVBoxLayout *verticalLayout_2;
    QCheckBox *ckHexReceive;
    QCheckBox *ckHexSend;
    QCheckBox *ckAscii;
    QCheckBox *ckShow;
    QCheckBox *ckDebug;
    QCheckBox *ckAutoSend;
    QComboBox *cboxInterval;
    QLabel *labServerIP;
    QLineEdit *txtServerIP;
    QLabel *labServerPort;
    QLineEdit *txtServerPort;
    QLabel *labListenPort;
    QLineEdit *txtListenPort;
    QPushButton *btnListen;
    QPushButton *btnSave;
    QPushButton *btnClear;
    QSpacerItem *verticalSpacer;
    QWidget *widget;
    QHBoxLayout *layUdpServer;
    QComboBox *cboxData;
    QPushButton *btnSend;

    void setupUi(QWidget *frmUdpServer)
    {
        if (frmUdpServer->objectName().isEmpty())
            frmUdpServer->setObjectName(QStringLiteral("frmUdpServer"));
        frmUdpServer->resize(800, 600);
        gridLayout = new QGridLayout(frmUdpServer);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        txtMain = new QTextEdit(frmUdpServer);
        txtMain->setObjectName(QStringLiteral("txtMain"));
        txtMain->setReadOnly(true);

        gridLayout->addWidget(txtMain, 0, 0, 1, 1);

        frame = new QFrame(frmUdpServer);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setMinimumSize(QSize(170, 0));
        frame->setMaximumSize(QSize(170, 16777215));
        frame->setFrameShape(QFrame::Box);
        frame->setFrameShadow(QFrame::Sunken);
        verticalLayout_2 = new QVBoxLayout(frame);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        ckHexReceive = new QCheckBox(frame);
        ckHexReceive->setObjectName(QStringLiteral("ckHexReceive"));

        verticalLayout_2->addWidget(ckHexReceive);

        ckHexSend = new QCheckBox(frame);
        ckHexSend->setObjectName(QStringLiteral("ckHexSend"));

        verticalLayout_2->addWidget(ckHexSend);

        ckAscii = new QCheckBox(frame);
        ckAscii->setObjectName(QStringLiteral("ckAscii"));

        verticalLayout_2->addWidget(ckAscii);

        ckShow = new QCheckBox(frame);
        ckShow->setObjectName(QStringLiteral("ckShow"));

        verticalLayout_2->addWidget(ckShow);

        ckDebug = new QCheckBox(frame);
        ckDebug->setObjectName(QStringLiteral("ckDebug"));

        verticalLayout_2->addWidget(ckDebug);

        ckAutoSend = new QCheckBox(frame);
        ckAutoSend->setObjectName(QStringLiteral("ckAutoSend"));

        verticalLayout_2->addWidget(ckAutoSend);

        cboxInterval = new QComboBox(frame);
        cboxInterval->setObjectName(QStringLiteral("cboxInterval"));

        verticalLayout_2->addWidget(cboxInterval);

        labServerIP = new QLabel(frame);
        labServerIP->setObjectName(QStringLiteral("labServerIP"));

        verticalLayout_2->addWidget(labServerIP);

        txtServerIP = new QLineEdit(frame);
        txtServerIP->setObjectName(QStringLiteral("txtServerIP"));

        verticalLayout_2->addWidget(txtServerIP);

        labServerPort = new QLabel(frame);
        labServerPort->setObjectName(QStringLiteral("labServerPort"));

        verticalLayout_2->addWidget(labServerPort);

        txtServerPort = new QLineEdit(frame);
        txtServerPort->setObjectName(QStringLiteral("txtServerPort"));

        verticalLayout_2->addWidget(txtServerPort);

        labListenPort = new QLabel(frame);
        labListenPort->setObjectName(QStringLiteral("labListenPort"));

        verticalLayout_2->addWidget(labListenPort);

        txtListenPort = new QLineEdit(frame);
        txtListenPort->setObjectName(QStringLiteral("txtListenPort"));

        verticalLayout_2->addWidget(txtListenPort);

        btnListen = new QPushButton(frame);
        btnListen->setObjectName(QStringLiteral("btnListen"));

        verticalLayout_2->addWidget(btnListen);

        btnSave = new QPushButton(frame);
        btnSave->setObjectName(QStringLiteral("btnSave"));

        verticalLayout_2->addWidget(btnSave);

        btnClear = new QPushButton(frame);
        btnClear->setObjectName(QStringLiteral("btnClear"));

        verticalLayout_2->addWidget(btnClear);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer);


        gridLayout->addWidget(frame, 0, 1, 2, 1);

        widget = new QWidget(frmUdpServer);
        widget->setObjectName(QStringLiteral("widget"));
        layUdpServer = new QHBoxLayout(widget);
        layUdpServer->setObjectName(QStringLiteral("layUdpServer"));
        layUdpServer->setContentsMargins(0, 0, 0, 0);
        cboxData = new QComboBox(widget);
        cboxData->setObjectName(QStringLiteral("cboxData"));
        QSizePolicy sizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(cboxData->sizePolicy().hasHeightForWidth());
        cboxData->setSizePolicy(sizePolicy);
        cboxData->setEditable(true);

        layUdpServer->addWidget(cboxData);

        btnSend = new QPushButton(widget);
        btnSend->setObjectName(QStringLiteral("btnSend"));
        btnSend->setMinimumSize(QSize(80, 0));
        btnSend->setMaximumSize(QSize(80, 16777215));

        layUdpServer->addWidget(btnSend);


        gridLayout->addWidget(widget, 1, 0, 1, 1);


        retranslateUi(frmUdpServer);

        QMetaObject::connectSlotsByName(frmUdpServer);
    } // setupUi

    void retranslateUi(QWidget *frmUdpServer)
    {
        frmUdpServer->setWindowTitle(QApplication::translate("frmUdpServer", "Form", Q_NULLPTR));
        ckHexReceive->setText(QApplication::translate("frmUdpServer", "16\350\277\233\345\210\266\346\216\245\346\224\266", Q_NULLPTR));
        ckHexSend->setText(QApplication::translate("frmUdpServer", "16\350\277\233\345\210\266\345\217\221\351\200\201", Q_NULLPTR));
        ckAscii->setText(QApplication::translate("frmUdpServer", "Ascii\345\255\227\347\254\246", Q_NULLPTR));
        ckShow->setText(QApplication::translate("frmUdpServer", "\346\232\202\345\201\234\346\230\276\347\244\272", Q_NULLPTR));
        ckDebug->setText(QApplication::translate("frmUdpServer", "\346\250\241\346\213\237\350\256\276\345\244\207", Q_NULLPTR));
        ckAutoSend->setText(QApplication::translate("frmUdpServer", "\345\256\232\346\227\266\345\217\221\351\200\201", Q_NULLPTR));
        labServerIP->setText(QApplication::translate("frmUdpServer", "\350\277\234\347\250\213IP\345\234\260\345\235\200", Q_NULLPTR));
        labServerPort->setText(QApplication::translate("frmUdpServer", "\350\277\234\347\250\213\347\253\257\345\217\243", Q_NULLPTR));
        labListenPort->setText(QApplication::translate("frmUdpServer", "\347\233\221\345\220\254\347\253\257\345\217\243", Q_NULLPTR));
        btnListen->setText(QApplication::translate("frmUdpServer", "\347\233\221\345\220\254", Q_NULLPTR));
        btnSave->setText(QApplication::translate("frmUdpServer", "\344\277\235\345\255\230", Q_NULLPTR));
        btnClear->setText(QApplication::translate("frmUdpServer", "\346\270\205\347\251\272", Q_NULLPTR));
        btnSend->setText(QApplication::translate("frmUdpServer", "\345\217\221\351\200\201", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class frmUdpServer: public Ui_frmUdpServer {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FRMUDPSERVER_H
