/********************************************************************************
** Form generated from reading UI file 'multisigdialog.ui'
**
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MULTISIGDIALOG_H
#define UI_MULTISIGDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MultiSigDialog
{
public:
    QVBoxLayout *verticalLayout;
    QFrame *frameCoinControl;
    QVBoxLayout *verticalLayoutCoinControl2;
    QVBoxLayout *verticalLayoutCoinControl;
    QHBoxLayout *horizontalLayoutCoinControl1;
    QLabel *labelMyMultiSig;
    QLabel *label;
    QLabel *labelBalance;
    QLabel *labelAddresses;
    QLabel *labelAddressesNum;
    QSpacerItem *horizontalSpacer_2;
    QHBoxLayout *horizontalLayoutCoinControl2;
    QLabel *labelMultiSigAddress;
    QComboBox *comboBoxAddrList;
    QSpacerItem *horizontalSpacerCoinControl;
    QWidget *widgetCoinControl;
    QHBoxLayout *horizontalLayoutCoinControl5;
    QHBoxLayout *horizontalLayoutCoinControl3;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_3;
    QLabel *labelAvailable;
    QLabel *labelAvailableCoins;
    QPushButton *btnImportDraft;
    QPushButton *btnExportDraft;
    QVBoxLayout *verticalLayout_4;
    QLabel *labelRequire;
    QHBoxLayout *horizontalLayout_4;
    QLineEdit *labelRequireAddr0;
    QToolButton *btnSign0;
    QLabel *labelIsSign0;
    QSpacerItem *horizontalSpacer_4;
    QHBoxLayout *horizontalLayout_5;
    QLineEdit *labelRequireAddr1;
    QToolButton *btnSign1;
    QLabel *labelIsSign1;
    QSpacerItem *horizontalSpacer_5;
    QHBoxLayout *horizontalLayout_6;
    QLineEdit *labelRequireAddr2;
    QToolButton *btnSign2;
    QLabel *labelIsSign2;
    QSpacerItem *horizontalSpacer_6;
    QSpacerItem *horizontalSpacer_3;
    QSpacerItem *verticalSpacerCoinControl;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *entries;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout;
    QPushButton *addButton;
    QPushButton *clearButton;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer;
    QPushButton *sendButton;
    QFrame *lineStatusBarDivide;

    void setupUi(QDialog *MultiSigDialog)
    {
        if (MultiSigDialog->objectName().isEmpty())
            MultiSigDialog->setObjectName(QStringLiteral("MultiSigDialog"));
        MultiSigDialog->resize(850, 400);
        verticalLayout = new QVBoxLayout(MultiSigDialog);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(-1, -1, -1, 8);
        frameCoinControl = new QFrame(MultiSigDialog);
        frameCoinControl->setObjectName(QStringLiteral("frameCoinControl"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(frameCoinControl->sizePolicy().hasHeightForWidth());
        frameCoinControl->setSizePolicy(sizePolicy);
        frameCoinControl->setMaximumSize(QSize(16777215, 16777215));
        frameCoinControl->setFrameShape(QFrame::StyledPanel);
        frameCoinControl->setFrameShadow(QFrame::Sunken);
        verticalLayoutCoinControl2 = new QVBoxLayout(frameCoinControl);
        verticalLayoutCoinControl2->setSpacing(6);
        verticalLayoutCoinControl2->setObjectName(QStringLiteral("verticalLayoutCoinControl2"));
        verticalLayoutCoinControl2->setContentsMargins(0, 0, 0, 0);
        verticalLayoutCoinControl = new QVBoxLayout();
        verticalLayoutCoinControl->setSpacing(0);
        verticalLayoutCoinControl->setObjectName(QStringLiteral("verticalLayoutCoinControl"));
        verticalLayoutCoinControl->setContentsMargins(10, 10, -1, -1);
        horizontalLayoutCoinControl1 = new QHBoxLayout();
        horizontalLayoutCoinControl1->setObjectName(QStringLiteral("horizontalLayoutCoinControl1"));
        horizontalLayoutCoinControl1->setContentsMargins(-1, -1, -1, 15);
        labelMyMultiSig = new QLabel(frameCoinControl);
        labelMyMultiSig->setObjectName(QStringLiteral("labelMyMultiSig"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Maximum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(labelMyMultiSig->sizePolicy().hasHeightForWidth());
        labelMyMultiSig->setSizePolicy(sizePolicy1);
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        labelMyMultiSig->setFont(font);
        labelMyMultiSig->setStyleSheet(QStringLiteral("font-weight:bold;"));

        horizontalLayoutCoinControl1->addWidget(labelMyMultiSig);

        label = new QLabel(frameCoinControl);
        label->setObjectName(QStringLiteral("label"));
        label->setFont(font);

        horizontalLayoutCoinControl1->addWidget(label);

        labelBalance = new QLabel(frameCoinControl);
        labelBalance->setObjectName(QStringLiteral("labelBalance"));
        labelBalance->setCursor(QCursor(Qt::IBeamCursor));
        labelBalance->setMargin(0);
        labelBalance->setIndent(-1);
        labelBalance->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByKeyboard|Qt::TextSelectableByMouse);

        horizontalLayoutCoinControl1->addWidget(labelBalance, 0, Qt::AlignHCenter|Qt::AlignVCenter);

        labelAddresses = new QLabel(frameCoinControl);
        labelAddresses->setObjectName(QStringLiteral("labelAddresses"));
        labelAddresses->setFont(font);
        labelAddresses->setIndent(-1);

        horizontalLayoutCoinControl1->addWidget(labelAddresses, 0, Qt::AlignVCenter);

        labelAddressesNum = new QLabel(frameCoinControl);
        labelAddressesNum->setObjectName(QStringLiteral("labelAddressesNum"));
        labelAddressesNum->setMinimumSize(QSize(28, 0));
        labelAddressesNum->setIndent(-1);

        horizontalLayoutCoinControl1->addWidget(labelAddressesNum, 0, Qt::AlignVCenter);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayoutCoinControl1->addItem(horizontalSpacer_2);


        verticalLayoutCoinControl->addLayout(horizontalLayoutCoinControl1);

        horizontalLayoutCoinControl2 = new QHBoxLayout();
        horizontalLayoutCoinControl2->setSpacing(8);
        horizontalLayoutCoinControl2->setObjectName(QStringLiteral("horizontalLayoutCoinControl2"));
        horizontalLayoutCoinControl2->setContentsMargins(-1, -1, -1, 10);
        labelMultiSigAddress = new QLabel(frameCoinControl);
        labelMultiSigAddress->setObjectName(QStringLiteral("labelMultiSigAddress"));
        labelMultiSigAddress->setFont(font);
        labelMultiSigAddress->setMargin(5);

        horizontalLayoutCoinControl2->addWidget(labelMultiSigAddress);

        comboBoxAddrList = new QComboBox(frameCoinControl);
        comboBoxAddrList->setObjectName(QStringLiteral("comboBoxAddrList"));
        QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(comboBoxAddrList->sizePolicy().hasHeightForWidth());
        comboBoxAddrList->setSizePolicy(sizePolicy2);
        comboBoxAddrList->setMinimumSize(QSize(240, 0));
        comboBoxAddrList->setSizeIncrement(QSize(0, 0));
        comboBoxAddrList->setBaseSize(QSize(0, 0));
        comboBoxAddrList->setEditable(false);
        comboBoxAddrList->setMaxVisibleItems(20);

        horizontalLayoutCoinControl2->addWidget(comboBoxAddrList);

        horizontalSpacerCoinControl = new QSpacerItem(40, 1, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayoutCoinControl2->addItem(horizontalSpacerCoinControl);


        verticalLayoutCoinControl->addLayout(horizontalLayoutCoinControl2);

        widgetCoinControl = new QWidget(frameCoinControl);
        widgetCoinControl->setObjectName(QStringLiteral("widgetCoinControl"));
        QSizePolicy sizePolicy3(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(widgetCoinControl->sizePolicy().hasHeightForWidth());
        widgetCoinControl->setSizePolicy(sizePolicy3);
        widgetCoinControl->setMinimumSize(QSize(0, 0));
        widgetCoinControl->setStyleSheet(QStringLiteral(""));
        horizontalLayoutCoinControl5 = new QHBoxLayout(widgetCoinControl);
        horizontalLayoutCoinControl5->setObjectName(QStringLiteral("horizontalLayoutCoinControl5"));
        horizontalLayoutCoinControl5->setContentsMargins(0, 0, 0, 0);
        horizontalLayoutCoinControl3 = new QHBoxLayout();
        horizontalLayoutCoinControl3->setSpacing(20);
        horizontalLayoutCoinControl3->setObjectName(QStringLiteral("horizontalLayoutCoinControl3"));
        horizontalLayoutCoinControl3->setContentsMargins(-1, 0, -1, 4);
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setSpacing(4);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(10, 4, -1, -1);
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(1);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        labelAvailable = new QLabel(widgetCoinControl);
        labelAvailable->setObjectName(QStringLiteral("labelAvailable"));
        labelAvailable->setFont(font);

        horizontalLayout_3->addWidget(labelAvailable);

        labelAvailableCoins = new QLabel(widgetCoinControl);
        labelAvailableCoins->setObjectName(QStringLiteral("labelAvailableCoins"));

        horizontalLayout_3->addWidget(labelAvailableCoins);


        verticalLayout_3->addLayout(horizontalLayout_3);

        btnImportDraft = new QPushButton(widgetCoinControl);
        btnImportDraft->setObjectName(QStringLiteral("btnImportDraft"));
        btnImportDraft->setMinimumSize(QSize(120, 0));
        QIcon icon;
        icon.addFile(QStringLiteral(":/icons/editpaste"), QSize(), QIcon::Normal, QIcon::Off);
        btnImportDraft->setIcon(icon);

        verticalLayout_3->addWidget(btnImportDraft);

        btnExportDraft = new QPushButton(widgetCoinControl);
        btnExportDraft->setObjectName(QStringLiteral("btnExportDraft"));
        btnExportDraft->setMinimumSize(QSize(120, 0));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/icons/editcopy"), QSize(), QIcon::Normal, QIcon::Off);
        btnExportDraft->setIcon(icon1);

        verticalLayout_3->addWidget(btnExportDraft);


        horizontalLayoutCoinControl3->addLayout(verticalLayout_3);

        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        verticalLayout_4->setContentsMargins(30, 2, 1, 0);
        labelRequire = new QLabel(widgetCoinControl);
        labelRequire->setObjectName(QStringLiteral("labelRequire"));

        verticalLayout_4->addWidget(labelRequire);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(0);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        labelRequireAddr0 = new QLineEdit(widgetCoinControl);
        labelRequireAddr0->setObjectName(QStringLiteral("labelRequireAddr0"));
        QSizePolicy sizePolicy4(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(labelRequireAddr0->sizePolicy().hasHeightForWidth());
        labelRequireAddr0->setSizePolicy(sizePolicy4);
        labelRequireAddr0->setMinimumSize(QSize(280, 0));
        labelRequireAddr0->setFrame(false);
        labelRequireAddr0->setReadOnly(true);

        horizontalLayout_4->addWidget(labelRequireAddr0);

        btnSign0 = new QToolButton(widgetCoinControl);
        btnSign0->setObjectName(QStringLiteral("btnSign0"));
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/icons/key"), QSize(), QIcon::Normal, QIcon::Off);
        btnSign0->setIcon(icon2);
        btnSign0->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        horizontalLayout_4->addWidget(btnSign0);

        labelIsSign0 = new QLabel(widgetCoinControl);
        labelIsSign0->setObjectName(QStringLiteral("labelIsSign0"));

        horizontalLayout_4->addWidget(labelIsSign0);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_4);


        verticalLayout_4->addLayout(horizontalLayout_4);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(0);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        labelRequireAddr1 = new QLineEdit(widgetCoinControl);
        labelRequireAddr1->setObjectName(QStringLiteral("labelRequireAddr1"));
        sizePolicy4.setHeightForWidth(labelRequireAddr1->sizePolicy().hasHeightForWidth());
        labelRequireAddr1->setSizePolicy(sizePolicy4);
        labelRequireAddr1->setMinimumSize(QSize(280, 0));
        labelRequireAddr1->setFrame(false);
        labelRequireAddr1->setReadOnly(true);

        horizontalLayout_5->addWidget(labelRequireAddr1);

        btnSign1 = new QToolButton(widgetCoinControl);
        btnSign1->setObjectName(QStringLiteral("btnSign1"));
        btnSign1->setIcon(icon2);
        btnSign1->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        horizontalLayout_5->addWidget(btnSign1);

        labelIsSign1 = new QLabel(widgetCoinControl);
        labelIsSign1->setObjectName(QStringLiteral("labelIsSign1"));

        horizontalLayout_5->addWidget(labelIsSign1);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_5);


        verticalLayout_4->addLayout(horizontalLayout_5);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setSpacing(0);
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        labelRequireAddr2 = new QLineEdit(widgetCoinControl);
        labelRequireAddr2->setObjectName(QStringLiteral("labelRequireAddr2"));
        sizePolicy4.setHeightForWidth(labelRequireAddr2->sizePolicy().hasHeightForWidth());
        labelRequireAddr2->setSizePolicy(sizePolicy4);
        labelRequireAddr2->setMinimumSize(QSize(280, 0));
        labelRequireAddr2->setFrame(false);
        labelRequireAddr2->setReadOnly(true);

        horizontalLayout_6->addWidget(labelRequireAddr2);

        btnSign2 = new QToolButton(widgetCoinControl);
        btnSign2->setObjectName(QStringLiteral("btnSign2"));
        btnSign2->setIcon(icon2);
        btnSign2->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        horizontalLayout_6->addWidget(btnSign2);

        labelIsSign2 = new QLabel(widgetCoinControl);
        labelIsSign2->setObjectName(QStringLiteral("labelIsSign2"));

        horizontalLayout_6->addWidget(labelIsSign2);

        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_6);


        verticalLayout_4->addLayout(horizontalLayout_6);


        horizontalLayoutCoinControl3->addLayout(verticalLayout_4);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayoutCoinControl3->addItem(horizontalSpacer_3);


        horizontalLayoutCoinControl5->addLayout(horizontalLayoutCoinControl3);


        verticalLayoutCoinControl->addWidget(widgetCoinControl);

        verticalSpacerCoinControl = new QSpacerItem(800, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayoutCoinControl->addItem(verticalSpacerCoinControl);

        verticalLayoutCoinControl->setStretch(3, 1);

        verticalLayoutCoinControl2->addLayout(verticalLayoutCoinControl);


        verticalLayout->addWidget(frameCoinControl);

        scrollArea = new QScrollArea(MultiSigDialog);
        scrollArea->setObjectName(QStringLiteral("scrollArea"));
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QStringLiteral("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 824, 99));
        verticalLayout_2 = new QVBoxLayout(scrollAreaWidgetContents);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        entries = new QVBoxLayout();
        entries->setSpacing(6);
        entries->setObjectName(QStringLiteral("entries"));

        verticalLayout_2->addLayout(entries);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer);

        scrollArea->setWidget(scrollAreaWidgetContents);

        verticalLayout->addWidget(scrollArea);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        addButton = new QPushButton(MultiSigDialog);
        addButton->setObjectName(QStringLiteral("addButton"));
        QIcon icon3;
        icon3.addFile(QStringLiteral(":/icons/add"), QSize(), QIcon::Normal, QIcon::Off);
        addButton->setIcon(icon3);
        addButton->setAutoDefault(false);

        horizontalLayout->addWidget(addButton);

        clearButton = new QPushButton(MultiSigDialog);
        clearButton->setObjectName(QStringLiteral("clearButton"));
        QSizePolicy sizePolicy5(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(clearButton->sizePolicy().hasHeightForWidth());
        clearButton->setSizePolicy(sizePolicy5);
        QIcon icon4;
        icon4.addFile(QStringLiteral(":/icons/remove"), QSize(), QIcon::Normal, QIcon::Off);
        clearButton->setIcon(icon4);
        clearButton->setAutoRepeatDelay(300);
        clearButton->setAutoDefault(false);

        horizontalLayout->addWidget(clearButton);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(3);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));

        horizontalLayout->addLayout(horizontalLayout_2);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        sendButton = new QPushButton(MultiSigDialog);
        sendButton->setObjectName(QStringLiteral("sendButton"));
        sendButton->setMinimumSize(QSize(150, 0));
        QIcon icon5;
        icon5.addFile(QStringLiteral(":/icons/send"), QSize(), QIcon::Normal, QIcon::Off);
        sendButton->setIcon(icon5);

        horizontalLayout->addWidget(sendButton);


        verticalLayout->addLayout(horizontalLayout);

        lineStatusBarDivide = new QFrame(MultiSigDialog);
        lineStatusBarDivide->setObjectName(QStringLiteral("lineStatusBarDivide"));
        lineStatusBarDivide->setFrameShadow(QFrame::Raised);
        lineStatusBarDivide->setFrameShape(QFrame::HLine);

        verticalLayout->addWidget(lineStatusBarDivide, 0, Qt::AlignBottom);

        verticalLayout->setStretch(1, 1);

        retranslateUi(MultiSigDialog);

        sendButton->setDefault(true);


        QMetaObject::connectSlotsByName(MultiSigDialog);
    } // setupUi

    void retranslateUi(QDialog *MultiSigDialog)
    {
        MultiSigDialog->setWindowTitle(QApplication::translate("MultiSigDialog", "Send Coins", 0));
        labelMyMultiSig->setText(QString());
        label->setText(QApplication::translate("MultiSigDialog", "Total Balance:  ", 0));
        labelBalance->setText(QApplication::translate("MultiSigDialog", "0.00000 FTC", 0));
        labelAddresses->setText(QApplication::translate("MultiSigDialog", "    Addresses:  ", 0));
        labelAddressesNum->setText(QApplication::translate("MultiSigDialog", "0", 0));
        labelMultiSigAddress->setText(QApplication::translate("MultiSigDialog", "Address", 0));
        labelAvailable->setText(QApplication::translate("MultiSigDialog", "Available: ", 0));
        labelAvailableCoins->setText(QApplication::translate("MultiSigDialog", "0 FTC", 0));
#ifndef QT_NO_TOOLTIP
        btnImportDraft->setToolTip(QApplication::translate("MultiSigDialog", "Import draft transaction with other's signature", 0));
#endif // QT_NO_TOOLTIP
        btnImportDraft->setText(QApplication::translate("MultiSigDialog", "Import Raw Tx", 0));
#ifndef QT_NO_TOOLTIP
        btnExportDraft->setToolTip(QApplication::translate("MultiSigDialog", "Export draft transaction with my signature", 0));
#endif // QT_NO_TOOLTIP
        btnExportDraft->setText(QApplication::translate("MultiSigDialog", "Export Raw Tx", 0));
        labelRequire->setText(QApplication::translate("MultiSigDialog", "Require 0 of 0 signatures", 0));
        btnSign0->setText(QApplication::translate("MultiSigDialog", "Sign", 0));
        labelIsSign0->setText(QApplication::translate("MultiSigDialog", "Unsigned", 0));
        btnSign1->setText(QApplication::translate("MultiSigDialog", "Sign", 0));
        labelIsSign1->setText(QApplication::translate("MultiSigDialog", "Signed", 0));
        btnSign2->setText(QApplication::translate("MultiSigDialog", "Sign", 0));
        labelIsSign2->setText(QString());
#ifndef QT_NO_TOOLTIP
        addButton->setToolTip(QApplication::translate("MultiSigDialog", "Send to multiple recipients at once", 0));
#endif // QT_NO_TOOLTIP
        addButton->setText(QApplication::translate("MultiSigDialog", "Add &Recipient", 0));
#ifndef QT_NO_TOOLTIP
        clearButton->setToolTip(QApplication::translate("MultiSigDialog", "Remove all transaction fields", 0));
#endif // QT_NO_TOOLTIP
        clearButton->setText(QApplication::translate("MultiSigDialog", "Clear &All", 0));
#ifndef QT_NO_TOOLTIP
        sendButton->setToolTip(QApplication::translate("MultiSigDialog", "Confirm the send action", 0));
#endif // QT_NO_TOOLTIP
        sendButton->setText(QApplication::translate("MultiSigDialog", "S&end", 0));
    } // retranslateUi

};

namespace Ui {
    class MultiSigDialog: public Ui_MultiSigDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MULTISIGDIALOG_H
