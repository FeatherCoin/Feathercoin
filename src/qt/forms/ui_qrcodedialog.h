/********************************************************************************
** Form generated from reading UI file 'qrcodedialog.ui'
**
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QRCODEDIALOG_H
#define UI_QRCODEDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "bitcoinamountfield.h"

QT_BEGIN_NAMESPACE

class Ui_QRCodeDialog
{
public:
    QVBoxLayout *verticalLayout_3;
    QLabel *lblQRCode;
    QPlainTextEdit *outUri;
    QWidget *widget;
    QVBoxLayout *verticalLayout_2;
    QCheckBox *chkReqPayment;
    QFormLayout *formLayout;
    QLabel *lblLabel;
    QLineEdit *lnLabel;
    QLabel *lblMessage;
    QLineEdit *lnMessage;
    QLabel *lblAmount;
    BitcoinAmountField *lnReqAmount;
    QHBoxLayout *horizontalLayout;
    QPushButton *btnClose;
    QSpacerItem *horizontalSpacer;
    QPushButton *btnSaveAs;

    void setupUi(QDialog *QRCodeDialog)
    {
        if (QRCodeDialog->objectName().isEmpty())
            QRCodeDialog->setObjectName(QStringLiteral("QRCodeDialog"));
        QRCodeDialog->resize(340, 543);
        verticalLayout_3 = new QVBoxLayout(QRCodeDialog);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        lblQRCode = new QLabel(QRCodeDialog);
        lblQRCode->setObjectName(QStringLiteral("lblQRCode"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(lblQRCode->sizePolicy().hasHeightForWidth());
        lblQRCode->setSizePolicy(sizePolicy);
        lblQRCode->setMinimumSize(QSize(300, 300));
        lblQRCode->setTextFormat(Qt::PlainText);
        lblQRCode->setAlignment(Qt::AlignCenter);
        lblQRCode->setWordWrap(true);

        verticalLayout_3->addWidget(lblQRCode);

        outUri = new QPlainTextEdit(QRCodeDialog);
        outUri->setObjectName(QStringLiteral("outUri"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Minimum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(outUri->sizePolicy().hasHeightForWidth());
        outUri->setSizePolicy(sizePolicy1);
        outUri->setMinimumSize(QSize(0, 50));
        outUri->setTabChangesFocus(true);
        outUri->setTextInteractionFlags(Qt::TextSelectableByKeyboard|Qt::TextSelectableByMouse);

        verticalLayout_3->addWidget(outUri);

        widget = new QWidget(QRCodeDialog);
        widget->setObjectName(QStringLiteral("widget"));
        verticalLayout_2 = new QVBoxLayout(widget);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        chkReqPayment = new QCheckBox(widget);
        chkReqPayment->setObjectName(QStringLiteral("chkReqPayment"));
        chkReqPayment->setEnabled(true);

        verticalLayout_2->addWidget(chkReqPayment);

        formLayout = new QFormLayout();
        formLayout->setObjectName(QStringLiteral("formLayout"));
        formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        lblLabel = new QLabel(widget);
        lblLabel->setObjectName(QStringLiteral("lblLabel"));
        lblLabel->setTextFormat(Qt::PlainText);
        lblLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout->setWidget(1, QFormLayout::LabelRole, lblLabel);

        lnLabel = new QLineEdit(widget);
        lnLabel->setObjectName(QStringLiteral("lnLabel"));

        formLayout->setWidget(1, QFormLayout::FieldRole, lnLabel);

        lblMessage = new QLabel(widget);
        lblMessage->setObjectName(QStringLiteral("lblMessage"));
        lblMessage->setTextFormat(Qt::PlainText);
        lblMessage->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout->setWidget(2, QFormLayout::LabelRole, lblMessage);

        lnMessage = new QLineEdit(widget);
        lnMessage->setObjectName(QStringLiteral("lnMessage"));

        formLayout->setWidget(2, QFormLayout::FieldRole, lnMessage);

        lblAmount = new QLabel(widget);
        lblAmount->setObjectName(QStringLiteral("lblAmount"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Maximum);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(lblAmount->sizePolicy().hasHeightForWidth());
        lblAmount->setSizePolicy(sizePolicy2);
        lblAmount->setTextFormat(Qt::PlainText);
        lblAmount->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout->setWidget(0, QFormLayout::LabelRole, lblAmount);

        lnReqAmount = new BitcoinAmountField(widget);
        lnReqAmount->setObjectName(QStringLiteral("lnReqAmount"));
        lnReqAmount->setEnabled(false);
        lnReqAmount->setMinimumSize(QSize(80, 0));

        formLayout->setWidget(0, QFormLayout::FieldRole, lnReqAmount);


        verticalLayout_2->addLayout(formLayout);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        btnClose = new QPushButton(widget);
        btnClose->setObjectName(QStringLiteral("btnClose"));

        horizontalLayout->addWidget(btnClose);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        btnSaveAs = new QPushButton(widget);
        btnSaveAs->setObjectName(QStringLiteral("btnSaveAs"));

        horizontalLayout->addWidget(btnSaveAs);


        verticalLayout_2->addLayout(horizontalLayout);


        verticalLayout_3->addWidget(widget);

#ifndef QT_NO_SHORTCUT
        lblLabel->setBuddy(lnLabel);
        lblMessage->setBuddy(lnMessage);
        lblAmount->setBuddy(lnReqAmount);
#endif // QT_NO_SHORTCUT

        retranslateUi(QRCodeDialog);
        QObject::connect(chkReqPayment, SIGNAL(clicked(bool)), lnReqAmount, SLOT(setEnabled(bool)));
        QObject::connect(btnClose, SIGNAL(clicked(bool)), QRCodeDialog, SLOT(close()));

        QMetaObject::connectSlotsByName(QRCodeDialog);
    } // setupUi

    void retranslateUi(QDialog *QRCodeDialog)
    {
        QRCodeDialog->setWindowTitle(QApplication::translate("QRCodeDialog", "QR Code Dialog", 0));
        chkReqPayment->setText(QApplication::translate("QRCodeDialog", "Request Payment", 0));
        lblLabel->setText(QApplication::translate("QRCodeDialog", "Label:", 0));
        lblMessage->setText(QApplication::translate("QRCodeDialog", "Message:", 0));
        lblAmount->setText(QApplication::translate("QRCodeDialog", "Amount:", 0));
        btnClose->setText(QApplication::translate("QRCodeDialog", "Close", 0));
        btnSaveAs->setText(QApplication::translate("QRCodeDialog", "&Save As...", 0));
    } // retranslateUi

};

namespace Ui {
    class QRCodeDialog: public Ui_QRCodeDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QRCODEDIALOG_H
