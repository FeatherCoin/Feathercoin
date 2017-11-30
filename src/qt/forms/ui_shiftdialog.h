/********************************************************************************
** Form generated from reading UI file 'shiftdialog.ui'
**
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SHIFTDIALOG_H
#define UI_SHIFTDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "qvalidatedlineedit.h"

QT_BEGIN_NAMESPACE

class Ui_ShiftDialog
{
public:
    QVBoxLayout *verticalLayout;
    QTabWidget *tabWidget;
    QWidget *tabMain;
    QVBoxLayout *verticalLayout_Main;
    QHBoxLayout *horizontalLayout_2_Main;
    QLabel *label_2;
    QLabel *databaseCacheLabel;
    QLabel *label;
    QLabel *threadsScriptVerifLabel;
    QSpacerItem *horizontalSpacer_2_Main;
    QHBoxLayout *horizontalLayout_3_Main;
    QLabel *databaseCacheUnitLabel;
    QLabel *overriddenByCommandLineInfoLabel;
    QLabel *label_5;
    QSpacerItem *horizontalSpacer;
    QLabel *label_9;
    QSpacerItem *horizontalSpacer_3_Main;
    QHBoxLayout *horizontalLayout_Bottom;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_7;
    QSpacerItem *horizontalSpacer_3;
    QLabel *label_8;
    QSpacerItem *horizontalSpacer_Bottom;
    QHBoxLayout *horizontalLayout;
    QLabel *label_36;
    QLabel *label_37;
    QLabel *label_38;
    QSpacerItem *horizontalSpacer_8;
    QLabel *label_39;
    QSpacerItem *horizontalSpacer_7;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_41;
    QLabel *label_40;
    QLabel *label_42;
    QSpacerItem *horizontalSpacer_10;
    QLabel *label_43;
    QSpacerItem *horizontalSpacer_9;
    QSpacerItem *horizontalSpacer_6;
    QHBoxLayout *horizontalLayout_1_Wallet;
    QPushButton *okButton;
    QSpacerItem *verticalSpacer_Main;
    QWidget *tabWallet;
    QVBoxLayout *verticalLayout_Wallet;
    QLabel *transactionFeeInfoLabel;
    QHBoxLayout *horizontalLayout_1_Wallet1;
    QLabel *transactionFeeLabel;
    QLineEdit *txAddress;
    QPushButton *txStatButton;
    QSpacerItem *horizontalSpacer_1_Wallet;
    QSpacerItem *verticalSpacer_Wallet;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_2;
    QLabel *label_10;
    QLabel *label_11;
    QLabel *label_12;
    QLabel *label_13;
    QLabel *label_14;
    QLabel *label_15;
    QLabel *label_17;
    QLabel *label_16;
    QWidget *tabNetwork;
    QVBoxLayout *verticalLayout_Network;
    QHBoxLayout *horizontalLayout_1_Network;
    QLabel *proxyIpLabel;
    QValidatedLineEdit *withdrawalAddr;
    QSpacerItem *horizontalSpacer_1_Network;
    QHBoxLayout *horizontalLayout123;
    QLabel *label_123;
    QLineEdit *ftcAddr;
    QSpacerItem *horizontalSpacer_4;
    QHBoxLayout *horizontalLayout124;
    QLabel *label_124;
    QComboBox *comboBox;
    QPushButton *postTransButton;
    QSpacerItem *horizontalSpacer_5;
    QGroupBox *groupBox_2;
    QLabel *label_18;
    QLabel *label_19;
    QLabel *label_20;
    QLabel *label_21;
    QLabel *label_23;
    QLabel *label_24;
    QLineEdit *lineEdit_3;
    QSpacerItem *verticalSpacer_Network;
    QWidget *tabWindow;
    QVBoxLayout *verticalLayout_Window;
    QGroupBox *groupBox_3;
    QLabel *label_22;
    QLineEdit *fixAmountEdit;
    QComboBox *fixComboBox;
    QLabel *label_25;
    QLineEdit *fixWithdrawalAddrEdit;
    QLabel *label_26;
    QLineEdit *returnAddr;
    QPushButton *postFixButton;
    QGroupBox *groupBox_4;
    QLabel *label_27;
    QLabel *label_28;
    QLabel *label_29;
    QLabel *label_30;
    QLabel *label_31;
    QLabel *label_32;
    QLabel *label_33;
    QLabel *label_34;
    QLabel *label_35;
    QLineEdit *lineEdit;
    QLineEdit *lineEdit_2;
    QWidget *tabDisplay;
    QVBoxLayout *verticalLayout_Display;
    QTextBrowser *textBrowser;
    QSpacerItem *verticalSpacer_Display;
    QFrame *frame;
    QVBoxLayout *verticalLayout_Bottom;
    QTextEdit *textReplyEdit;
    QHBoxLayout *horizontalLayout_Buttons;
    QSpacerItem *horizontalSpacer_1;
    QLabel *label_6;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *cancelButton;

    void setupUi(QDialog *ShiftDialog)
    {
        if (ShiftDialog->objectName().isEmpty())
            ShiftDialog->setObjectName(QStringLiteral("ShiftDialog"));
        ShiftDialog->resize(560, 470);
        ShiftDialog->setModal(true);
        verticalLayout = new QVBoxLayout(ShiftDialog);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        tabWidget = new QTabWidget(ShiftDialog);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tabMain = new QWidget();
        tabMain->setObjectName(QStringLiteral("tabMain"));
        verticalLayout_Main = new QVBoxLayout(tabMain);
        verticalLayout_Main->setObjectName(QStringLiteral("verticalLayout_Main"));
        horizontalLayout_2_Main = new QHBoxLayout();
        horizontalLayout_2_Main->setObjectName(QStringLiteral("horizontalLayout_2_Main"));
        label_2 = new QLabel(tabMain);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout_2_Main->addWidget(label_2);

        databaseCacheLabel = new QLabel(tabMain);
        databaseCacheLabel->setObjectName(QStringLiteral("databaseCacheLabel"));
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        databaseCacheLabel->setFont(font);
        databaseCacheLabel->setTextFormat(Qt::PlainText);

        horizontalLayout_2_Main->addWidget(databaseCacheLabel);

        label = new QLabel(tabMain);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout_2_Main->addWidget(label);

        threadsScriptVerifLabel = new QLabel(tabMain);
        threadsScriptVerifLabel->setObjectName(QStringLiteral("threadsScriptVerifLabel"));
        threadsScriptVerifLabel->setFont(font);
        threadsScriptVerifLabel->setTextFormat(Qt::PlainText);

        horizontalLayout_2_Main->addWidget(threadsScriptVerifLabel);

        horizontalSpacer_2_Main = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2_Main->addItem(horizontalSpacer_2_Main);


        verticalLayout_Main->addLayout(horizontalLayout_2_Main);

        horizontalLayout_3_Main = new QHBoxLayout();
        horizontalLayout_3_Main->setObjectName(QStringLiteral("horizontalLayout_3_Main"));
        databaseCacheUnitLabel = new QLabel(tabMain);
        databaseCacheUnitLabel->setObjectName(QStringLiteral("databaseCacheUnitLabel"));
        databaseCacheUnitLabel->setFont(font);
        databaseCacheUnitLabel->setTextFormat(Qt::PlainText);

        horizontalLayout_3_Main->addWidget(databaseCacheUnitLabel);

        overriddenByCommandLineInfoLabel = new QLabel(tabMain);
        overriddenByCommandLineInfoLabel->setObjectName(QStringLiteral("overriddenByCommandLineInfoLabel"));
        overriddenByCommandLineInfoLabel->setTextFormat(Qt::PlainText);

        horizontalLayout_3_Main->addWidget(overriddenByCommandLineInfoLabel);

        label_5 = new QLabel(tabMain);
        label_5->setObjectName(QStringLiteral("label_5"));
        QFont font1;
        font1.setUnderline(true);
        label_5->setFont(font1);
        label_5->setCursor(QCursor(Qt::PointingHandCursor));

        horizontalLayout_3_Main->addWidget(label_5);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Preferred, QSizePolicy::Minimum);

        horizontalLayout_3_Main->addItem(horizontalSpacer);

        label_9 = new QLabel(tabMain);
        label_9->setObjectName(QStringLiteral("label_9"));
        label_9->setFont(font1);
        label_9->setCursor(QCursor(Qt::PointingHandCursor));

        horizontalLayout_3_Main->addWidget(label_9);

        horizontalSpacer_3_Main = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3_Main->addItem(horizontalSpacer_3_Main);


        verticalLayout_Main->addLayout(horizontalLayout_3_Main);

        horizontalLayout_Bottom = new QHBoxLayout();
        horizontalLayout_Bottom->setObjectName(QStringLiteral("horizontalLayout_Bottom"));
        label_3 = new QLabel(tabMain);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setFont(font);

        horizontalLayout_Bottom->addWidget(label_3);

        label_4 = new QLabel(tabMain);
        label_4->setObjectName(QStringLiteral("label_4"));

        horizontalLayout_Bottom->addWidget(label_4);

        label_7 = new QLabel(tabMain);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setFont(font1);
        label_7->setCursor(QCursor(Qt::PointingHandCursor));

        horizontalLayout_Bottom->addWidget(label_7);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Minimum, QSizePolicy::Minimum);

        horizontalLayout_Bottom->addItem(horizontalSpacer_3);

        label_8 = new QLabel(tabMain);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setFont(font1);
        label_8->setCursor(QCursor(Qt::PointingHandCursor));

        horizontalLayout_Bottom->addWidget(label_8);

        horizontalSpacer_Bottom = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_Bottom->addItem(horizontalSpacer_Bottom);


        verticalLayout_Main->addLayout(horizontalLayout_Bottom);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label_36 = new QLabel(tabMain);
        label_36->setObjectName(QStringLiteral("label_36"));
        label_36->setFont(font);

        horizontalLayout->addWidget(label_36);

        label_37 = new QLabel(tabMain);
        label_37->setObjectName(QStringLiteral("label_37"));

        horizontalLayout->addWidget(label_37);

        label_38 = new QLabel(tabMain);
        label_38->setObjectName(QStringLiteral("label_38"));
        label_38->setFont(font1);
        label_38->setCursor(QCursor(Qt::PointingHandCursor));

        horizontalLayout->addWidget(label_38);

        horizontalSpacer_8 = new QSpacerItem(40, 20, QSizePolicy::Minimum, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_8);

        label_39 = new QLabel(tabMain);
        label_39->setObjectName(QStringLiteral("label_39"));
        label_39->setFont(font1);
        label_39->setCursor(QCursor(Qt::PointingHandCursor));

        horizontalLayout->addWidget(label_39);

        horizontalSpacer_7 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_7);


        verticalLayout_Main->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label_41 = new QLabel(tabMain);
        label_41->setObjectName(QStringLiteral("label_41"));
        label_41->setFont(font);

        horizontalLayout_2->addWidget(label_41);

        label_40 = new QLabel(tabMain);
        label_40->setObjectName(QStringLiteral("label_40"));

        horizontalLayout_2->addWidget(label_40);

        label_42 = new QLabel(tabMain);
        label_42->setObjectName(QStringLiteral("label_42"));
        label_42->setFont(font1);
        label_42->setCursor(QCursor(Qt::PointingHandCursor));

        horizontalLayout_2->addWidget(label_42);

        horizontalSpacer_10 = new QSpacerItem(40, 20, QSizePolicy::Minimum, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_10);

        label_43 = new QLabel(tabMain);
        label_43->setObjectName(QStringLiteral("label_43"));
        label_43->setFont(font1);
        label_43->setCursor(QCursor(Qt::PointingHandCursor));

        horizontalLayout_2->addWidget(label_43);

        horizontalSpacer_9 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_9);


        verticalLayout_Main->addLayout(horizontalLayout_2);

        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        verticalLayout_Main->addItem(horizontalSpacer_6);

        horizontalLayout_1_Wallet = new QHBoxLayout();
        horizontalLayout_1_Wallet->setObjectName(QStringLiteral("horizontalLayout_1_Wallet"));
        okButton = new QPushButton(tabMain);
        okButton->setObjectName(QStringLiteral("okButton"));
        okButton->setMinimumSize(QSize(40, 0));
        okButton->setMaximumSize(QSize(120, 16777215));
        okButton->setFont(font);
        okButton->setLayoutDirection(Qt::RightToLeft);

        horizontalLayout_1_Wallet->addWidget(okButton);


        verticalLayout_Main->addLayout(horizontalLayout_1_Wallet);

        verticalSpacer_Main = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_Main->addItem(verticalSpacer_Main);

        tabWidget->addTab(tabMain, QString());
        tabWallet = new QWidget();
        tabWallet->setObjectName(QStringLiteral("tabWallet"));
        verticalLayout_Wallet = new QVBoxLayout(tabWallet);
        verticalLayout_Wallet->setObjectName(QStringLiteral("verticalLayout_Wallet"));
        transactionFeeInfoLabel = new QLabel(tabWallet);
        transactionFeeInfoLabel->setObjectName(QStringLiteral("transactionFeeInfoLabel"));
        transactionFeeInfoLabel->setTextFormat(Qt::PlainText);
        transactionFeeInfoLabel->setWordWrap(true);

        verticalLayout_Wallet->addWidget(transactionFeeInfoLabel);

        horizontalLayout_1_Wallet1 = new QHBoxLayout();
        horizontalLayout_1_Wallet1->setObjectName(QStringLiteral("horizontalLayout_1_Wallet1"));
        transactionFeeLabel = new QLabel(tabWallet);
        transactionFeeLabel->setObjectName(QStringLiteral("transactionFeeLabel"));
        transactionFeeLabel->setTextFormat(Qt::PlainText);

        horizontalLayout_1_Wallet1->addWidget(transactionFeeLabel);

        txAddress = new QLineEdit(tabWallet);
        txAddress->setObjectName(QStringLiteral("txAddress"));

        horizontalLayout_1_Wallet1->addWidget(txAddress);

        txStatButton = new QPushButton(tabWallet);
        txStatButton->setObjectName(QStringLiteral("txStatButton"));

        horizontalLayout_1_Wallet1->addWidget(txStatButton);

        horizontalSpacer_1_Wallet = new QSpacerItem(40, 20, QSizePolicy::Minimum, QSizePolicy::Minimum);

        horizontalLayout_1_Wallet1->addItem(horizontalSpacer_1_Wallet);


        verticalLayout_Wallet->addLayout(horizontalLayout_1_Wallet1);

        verticalSpacer_Wallet = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_Wallet->addItem(verticalSpacer_Wallet);

        groupBox = new QGroupBox(tabWallet);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        verticalLayout_2 = new QVBoxLayout(groupBox);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        label_10 = new QLabel(groupBox);
        label_10->setObjectName(QStringLiteral("label_10"));

        verticalLayout_2->addWidget(label_10);

        label_11 = new QLabel(groupBox);
        label_11->setObjectName(QStringLiteral("label_11"));

        verticalLayout_2->addWidget(label_11);

        label_12 = new QLabel(groupBox);
        label_12->setObjectName(QStringLiteral("label_12"));

        verticalLayout_2->addWidget(label_12);

        label_13 = new QLabel(groupBox);
        label_13->setObjectName(QStringLiteral("label_13"));

        verticalLayout_2->addWidget(label_13);

        label_14 = new QLabel(groupBox);
        label_14->setObjectName(QStringLiteral("label_14"));

        verticalLayout_2->addWidget(label_14);

        label_15 = new QLabel(groupBox);
        label_15->setObjectName(QStringLiteral("label_15"));

        verticalLayout_2->addWidget(label_15);

        label_17 = new QLabel(groupBox);
        label_17->setObjectName(QStringLiteral("label_17"));

        verticalLayout_2->addWidget(label_17);

        label_16 = new QLabel(groupBox);
        label_16->setObjectName(QStringLiteral("label_16"));

        verticalLayout_2->addWidget(label_16);


        verticalLayout_Wallet->addWidget(groupBox);

        tabWidget->addTab(tabWallet, QString());
        tabNetwork = new QWidget();
        tabNetwork->setObjectName(QStringLiteral("tabNetwork"));
        verticalLayout_Network = new QVBoxLayout(tabNetwork);
        verticalLayout_Network->setObjectName(QStringLiteral("verticalLayout_Network"));
        horizontalLayout_1_Network = new QHBoxLayout();
        horizontalLayout_1_Network->setObjectName(QStringLiteral("horizontalLayout_1_Network"));
        proxyIpLabel = new QLabel(tabNetwork);
        proxyIpLabel->setObjectName(QStringLiteral("proxyIpLabel"));
        proxyIpLabel->setMinimumSize(QSize(116, 0));
        proxyIpLabel->setTextFormat(Qt::PlainText);

        horizontalLayout_1_Network->addWidget(proxyIpLabel);

        withdrawalAddr = new QValidatedLineEdit(tabNetwork);
        withdrawalAddr->setObjectName(QStringLiteral("withdrawalAddr"));
        withdrawalAddr->setMinimumSize(QSize(251, 0));
        withdrawalAddr->setMaximumSize(QSize(140, 16777215));

        horizontalLayout_1_Network->addWidget(withdrawalAddr);

        horizontalSpacer_1_Network = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_1_Network->addItem(horizontalSpacer_1_Network);


        verticalLayout_Network->addLayout(horizontalLayout_1_Network);

        horizontalLayout123 = new QHBoxLayout();
        horizontalLayout123->setObjectName(QStringLiteral("horizontalLayout123"));
        horizontalLayout123->setSizeConstraint(QLayout::SetMaximumSize);
        label_123 = new QLabel(tabNetwork);
        label_123->setObjectName(QStringLiteral("label_123"));

        horizontalLayout123->addWidget(label_123);

        ftcAddr = new QLineEdit(tabNetwork);
        ftcAddr->setObjectName(QStringLiteral("ftcAddr"));
        ftcAddr->setMinimumSize(QSize(251, 0));

        horizontalLayout123->addWidget(ftcAddr);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout123->addItem(horizontalSpacer_4);


        verticalLayout_Network->addLayout(horizontalLayout123);

        horizontalLayout124 = new QHBoxLayout();
        horizontalLayout124->setObjectName(QStringLiteral("horizontalLayout124"));
        horizontalLayout124->setSizeConstraint(QLayout::SetMaximumSize);
        label_124 = new QLabel(tabNetwork);
        label_124->setObjectName(QStringLiteral("label_124"));

        horizontalLayout124->addWidget(label_124);

        comboBox = new QComboBox(tabNetwork);
        comboBox->setObjectName(QStringLiteral("comboBox"));

        horizontalLayout124->addWidget(comboBox);

        postTransButton = new QPushButton(tabNetwork);
        postTransButton->setObjectName(QStringLiteral("postTransButton"));
        postTransButton->setMinimumSize(QSize(0, 0));
        postTransButton->setMaximumSize(QSize(80, 16777215));
        postTransButton->setBaseSize(QSize(0, 0));
        postTransButton->setAutoDefault(true);

        horizontalLayout124->addWidget(postTransButton);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Minimum, QSizePolicy::Minimum);

        horizontalLayout124->addItem(horizontalSpacer_5);


        verticalLayout_Network->addLayout(horizontalLayout124);

        groupBox_2 = new QGroupBox(tabNetwork);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        groupBox_2->setMinimumSize(QSize(0, 150));
        label_18 = new QLabel(groupBox_2);
        label_18->setObjectName(QStringLiteral("label_18"));
        label_18->setGeometry(QRect(10, 20, 101, 21));
        label_19 = new QLabel(groupBox_2);
        label_19->setObjectName(QStringLiteral("label_19"));
        label_19->setGeometry(QRect(10, 40, 501, 16));
        label_20 = new QLabel(groupBox_2);
        label_20->setObjectName(QStringLiteral("label_20"));
        label_20->setGeometry(QRect(10, 60, 501, 16));
        label_21 = new QLabel(groupBox_2);
        label_21->setObjectName(QStringLiteral("label_21"));
        label_21->setGeometry(QRect(10, 80, 501, 16));
        label_23 = new QLabel(groupBox_2);
        label_23->setObjectName(QStringLiteral("label_23"));
        label_23->setGeometry(QRect(10, 100, 491, 16));
        label_24 = new QLabel(groupBox_2);
        label_24->setObjectName(QStringLiteral("label_24"));
        label_24->setGeometry(QRect(10, 120, 501, 16));
        lineEdit_3 = new QLineEdit(groupBox_2);
        lineEdit_3->setObjectName(QStringLiteral("lineEdit_3"));
        lineEdit_3->setGeometry(QRect(112, 20, 381, 20));

        verticalLayout_Network->addWidget(groupBox_2);

        verticalSpacer_Network = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_Network->addItem(verticalSpacer_Network);

        tabWidget->addTab(tabNetwork, QString());
        tabWindow = new QWidget();
        tabWindow->setObjectName(QStringLiteral("tabWindow"));
        verticalLayout_Window = new QVBoxLayout(tabWindow);
        verticalLayout_Window->setObjectName(QStringLiteral("verticalLayout_Window"));
        groupBox_3 = new QGroupBox(tabWindow);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        groupBox_3->setMinimumSize(QSize(0, 100));
        label_22 = new QLabel(groupBox_3);
        label_22->setObjectName(QStringLiteral("label_22"));
        label_22->setGeometry(QRect(10, 20, 121, 16));
        fixAmountEdit = new QLineEdit(groupBox_3);
        fixAmountEdit->setObjectName(QStringLiteral("fixAmountEdit"));
        fixAmountEdit->setGeometry(QRect(160, 20, 151, 20));
        fixComboBox = new QComboBox(groupBox_3);
        fixComboBox->setObjectName(QStringLiteral("fixComboBox"));
        fixComboBox->setGeometry(QRect(350, 20, 91, 22));
        label_25 = new QLabel(groupBox_3);
        label_25->setObjectName(QStringLiteral("label_25"));
        label_25->setGeometry(QRect(10, 60, 121, 16));
        fixWithdrawalAddrEdit = new QLineEdit(groupBox_3);
        fixWithdrawalAddrEdit->setObjectName(QStringLiteral("fixWithdrawalAddrEdit"));
        fixWithdrawalAddrEdit->setGeometry(QRect(150, 60, 291, 20));
        label_26 = new QLabel(groupBox_3);
        label_26->setObjectName(QStringLiteral("label_26"));
        label_26->setGeometry(QRect(30, 100, 111, 16));
        returnAddr = new QLineEdit(groupBox_3);
        returnAddr->setObjectName(QStringLiteral("returnAddr"));
        returnAddr->setGeometry(QRect(150, 100, 261, 20));
        postFixButton = new QPushButton(groupBox_3);
        postFixButton->setObjectName(QStringLiteral("postFixButton"));
        postFixButton->setGeometry(QRect(440, 100, 75, 23));

        verticalLayout_Window->addWidget(groupBox_3);

        groupBox_4 = new QGroupBox(tabWindow);
        groupBox_4->setObjectName(QStringLiteral("groupBox_4"));
        groupBox_4->setMinimumSize(QSize(0, 140));
        label_27 = new QLabel(groupBox_4);
        label_27->setObjectName(QStringLiteral("label_27"));
        label_27->setGeometry(QRect(9, 20, 261, 16));
        label_28 = new QLabel(groupBox_4);
        label_28->setObjectName(QStringLiteral("label_28"));
        label_28->setGeometry(QRect(9, 58, 481, 16));
        label_29 = new QLabel(groupBox_4);
        label_29->setObjectName(QStringLiteral("label_29"));
        label_29->setGeometry(QRect(10, 77, 191, 20));
        label_30 = new QLabel(groupBox_4);
        label_30->setObjectName(QStringLiteral("label_30"));
        label_30->setGeometry(QRect(9, 35, 101, 21));
        label_31 = new QLabel(groupBox_4);
        label_31->setObjectName(QStringLiteral("label_31"));
        label_31->setGeometry(QRect(330, 40, 101, 16));
        label_32 = new QLabel(groupBox_4);
        label_32->setObjectName(QStringLiteral("label_32"));
        label_32->setGeometry(QRect(323, 80, 171, 16));
        label_33 = new QLabel(groupBox_4);
        label_33->setObjectName(QStringLiteral("label_33"));
        label_33->setGeometry(QRect(10, 115, 311, 16));
        label_34 = new QLabel(groupBox_4);
        label_34->setObjectName(QStringLiteral("label_34"));
        label_34->setGeometry(QRect(10, 95, 331, 21));
        label_35 = new QLabel(groupBox_4);
        label_35->setObjectName(QStringLiteral("label_35"));
        label_35->setGeometry(QRect(324, 21, 181, 16));
        lineEdit = new QLineEdit(groupBox_4);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));
        lineEdit->setGeometry(QRect(120, 40, 211, 20));
        lineEdit->setMinimumSize(QSize(100, 0));
        lineEdit->setMaximumSize(QSize(300, 16777215));
        lineEdit_2 = new QLineEdit(groupBox_4);
        lineEdit_2->setObjectName(QStringLiteral("lineEdit_2"));
        lineEdit_2->setGeometry(QRect(440, 40, 81, 20));
        label_27->raise();
        label_28->raise();
        label_29->raise();
        label_30->raise();
        label_31->raise();
        label_32->raise();
        label_33->raise();
        label_34->raise();
        label_35->raise();
        lineEdit->raise();
        lineEdit_2->raise();
        groupBox_3->raise();

        verticalLayout_Window->addWidget(groupBox_4);

        tabWidget->addTab(tabWindow, QString());
        tabDisplay = new QWidget();
        tabDisplay->setObjectName(QStringLiteral("tabDisplay"));
        verticalLayout_Display = new QVBoxLayout(tabDisplay);
        verticalLayout_Display->setObjectName(QStringLiteral("verticalLayout_Display"));
        textBrowser = new QTextBrowser(tabDisplay);
        textBrowser->setObjectName(QStringLiteral("textBrowser"));
        textBrowser->setMinimumSize(QSize(0, 120));
        textBrowser->setMaximumSize(QSize(16777215, 260));

        verticalLayout_Display->addWidget(textBrowser);

        verticalSpacer_Display = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_Display->addItem(verticalSpacer_Display);

        tabWidget->addTab(tabDisplay, QString());

        verticalLayout->addWidget(tabWidget);

        frame = new QFrame(ShiftDialog);
        frame->setObjectName(QStringLiteral("frame"));
        verticalLayout_Bottom = new QVBoxLayout(frame);
        verticalLayout_Bottom->setObjectName(QStringLiteral("verticalLayout_Bottom"));
        textReplyEdit = new QTextEdit(frame);
        textReplyEdit->setObjectName(QStringLiteral("textReplyEdit"));
        textReplyEdit->setMinimumSize(QSize(0, 40));

        verticalLayout_Bottom->addWidget(textReplyEdit);


        verticalLayout->addWidget(frame);

        horizontalLayout_Buttons = new QHBoxLayout();
        horizontalLayout_Buttons->setObjectName(QStringLiteral("horizontalLayout_Buttons"));
        horizontalSpacer_1 = new QSpacerItem(40, 48, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_Buttons->addItem(horizontalSpacer_1);

        label_6 = new QLabel(ShiftDialog);
        label_6->setObjectName(QStringLiteral("label_6"));

        horizontalLayout_Buttons->addWidget(label_6);

        horizontalSpacer_2 = new QSpacerItem(40, 48, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_Buttons->addItem(horizontalSpacer_2);

        cancelButton = new QPushButton(ShiftDialog);
        cancelButton->setObjectName(QStringLiteral("cancelButton"));
        cancelButton->setAutoDefault(false);

        horizontalLayout_Buttons->addWidget(cancelButton);


        verticalLayout->addLayout(horizontalLayout_Buttons);

#ifndef QT_NO_SHORTCUT
        proxyIpLabel->setBuddy(withdrawalAddr);
#endif // QT_NO_SHORTCUT

        retranslateUi(ShiftDialog);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(ShiftDialog);
    } // setupUi

    void retranslateUi(QDialog *ShiftDialog)
    {
        ShiftDialog->setWindowTitle(QApplication::translate("ShiftDialog", "ShapeShift", 0));
        label_2->setText(QApplication::translate("ShiftDialog", "..............", 0));
        databaseCacheLabel->setText(QApplication::translate("ShiftDialog", "Rate", 0));
        label->setText(QApplication::translate("ShiftDialog", "........", 0));
        threadsScriptVerifLabel->setText(QApplication::translate("ShiftDialog", "Deposit Limit", 0));
        databaseCacheUnitLabel->setText(QApplication::translate("ShiftDialog", "FTC/BTC", 0));
        overriddenByCommandLineInfoLabel->setText(QApplication::translate("ShiftDialog", "...", 0));
        label_5->setText(QApplication::translate("ShiftDialog", "0.0001234", 0));
        label_9->setText(QApplication::translate("ShiftDialog", "1000", 0));
        label_3->setText(QApplication::translate("ShiftDialog", "FTC/DOGE", 0));
        label_4->setText(QApplication::translate("ShiftDialog", "..", 0));
        label_7->setText(QApplication::translate("ShiftDialog", "0.0001234", 0));
        label_8->setText(QApplication::translate("ShiftDialog", "1000", 0));
        label_36->setText(QApplication::translate("ShiftDialog", "FTC/LTC", 0));
        label_37->setText(QApplication::translate("ShiftDialog", "...", 0));
        label_38->setText(QApplication::translate("ShiftDialog", "0.0001234", 0));
        label_39->setText(QApplication::translate("ShiftDialog", "1000", 0));
        label_41->setText(QApplication::translate("ShiftDialog", "FTC/BC", 0));
        label_40->setText(QApplication::translate("ShiftDialog", "....", 0));
        label_42->setText(QApplication::translate("ShiftDialog", "0.0001234", 0));
        label_43->setText(QApplication::translate("ShiftDialog", "1000", 0));
        okButton->setText(QApplication::translate("ShiftDialog", "&Refresh Data", 0));
        tabWidget->setTabText(tabWidget->indexOf(tabMain), QApplication::translate("ShiftDialog", "&Status", 0));
        transactionFeeInfoLabel->setText(QApplication::translate("ShiftDialog", "Status of deposit to address", 0));
        transactionFeeLabel->setText(QApplication::translate("ShiftDialog", "Your address", 0));
        txStatButton->setText(QApplication::translate("ShiftDialog", "Seek", 0));
        groupBox->setTitle(QApplication::translate("ShiftDialog", "txStat", 0));
#ifndef QT_NO_TOOLTIP
        label_10->setToolTip(QApplication::translate("ShiftDialog", "deposit to address", 0));
#endif // QT_NO_TOOLTIP
        label_10->setText(QApplication::translate("ShiftDialog", "Address:", 0));
        label_11->setText(QApplication::translate("ShiftDialog", "Status:", 0));
#ifndef QT_NO_TOOLTIP
        label_12->setToolTip(QApplication::translate("ShiftDialog", "withdrawal address", 0));
#endif // QT_NO_TOOLTIP
        label_12->setText(QApplication::translate("ShiftDialog", "Withdrawal address:", 0));
#ifndef QT_NO_TOOLTIP
        label_13->setToolTip(QApplication::translate("ShiftDialog", "amount deposited", 0));
#endif // QT_NO_TOOLTIP
        label_13->setText(QApplication::translate("ShiftDialog", "Amount deposited:", 0));
#ifndef QT_NO_TOOLTIP
        label_14->setToolTip(QApplication::translate("ShiftDialog", "coin type of deposit", 0));
#endif // QT_NO_TOOLTIP
        label_14->setText(QApplication::translate("ShiftDialog", "Coin type of deposit:", 0));
        label_15->setText(QApplication::translate("ShiftDialog", "Amount sent to withdrawal address:", 0));
#ifndef QT_NO_TOOLTIP
        label_17->setToolTip(QApplication::translate("ShiftDialog", "coin type of withdrawal", 0));
#endif // QT_NO_TOOLTIP
        label_17->setText(QApplication::translate("ShiftDialog", "Coin type of withdrawal:", 0));
#ifndef QT_NO_TOOLTIP
        label_16->setToolTip(QApplication::translate("ShiftDialog", "transaction id of coin sent to withdrawal address", 0));
#endif // QT_NO_TOOLTIP
        label_16->setText(QApplication::translate("ShiftDialog", "Transaction id of coin sent to withdrawal address:", 0));
        tabWidget->setTabText(tabWidget->indexOf(tabWallet), QApplication::translate("ShiftDialog", "Address", 0));
        proxyIpLabel->setText(QApplication::translate("ShiftDialog", "Withdrawal Address", 0));
#ifndef QT_NO_TOOLTIP
        withdrawalAddr->setToolTip(QApplication::translate("ShiftDialog", "the address for resulting coin to be sent to", 0));
#endif // QT_NO_TOOLTIP
        label_123->setText(QApplication::translate("ShiftDialog", "Feathercoin Address", 0));
#ifndef QT_NO_TOOLTIP
        ftcAddr->setToolTip(QApplication::translate("ShiftDialog", "address to return deposit to if anything goes wrong with exchange", 0));
#endif // QT_NO_TOOLTIP
        label_124->setText(QApplication::translate("ShiftDialog", "Your Feathercoin shift to", 0));
#ifndef QT_NO_TOOLTIP
        comboBox->setToolTip(QApplication::translate("ShiftDialog", "what coins are being exchanged in the form", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        postTransButton->setToolTip(QApplication::translate("ShiftDialog", "Normal Transaction", 0));
#endif // QT_NO_TOOLTIP
        postTransButton->setText(QApplication::translate("ShiftDialog", "POST", 0));
        groupBox_2->setTitle(QApplication::translate("ShiftDialog", "Output", 0));
#ifndef QT_NO_TOOLTIP
        label_18->setToolTip(QApplication::translate("ShiftDialog", "Send your feathercoin to this address", 0));
#endif // QT_NO_TOOLTIP
        label_18->setText(QApplication::translate("ShiftDialog", "Deposit Address:", 0));
#ifndef QT_NO_TOOLTIP
        label_19->setToolTip(QApplication::translate("ShiftDialog", "coin type of deposit", 0));
#endif // QT_NO_TOOLTIP
        label_19->setText(QApplication::translate("ShiftDialog", "Deposit Type:", 0));
#ifndef QT_NO_TOOLTIP
        label_20->setToolTip(QApplication::translate("ShiftDialog", "the address for resulting coin to be sent to", 0));
#endif // QT_NO_TOOLTIP
        label_20->setText(QApplication::translate("ShiftDialog", "Withdrawal Address:", 0));
#ifndef QT_NO_TOOLTIP
        label_21->setToolTip(QApplication::translate("ShiftDialog", "coin type of withdrawal", 0));
#endif // QT_NO_TOOLTIP
        label_21->setText(QApplication::translate("ShiftDialog", "Withdrawal Type:", 0));
#ifndef QT_NO_TOOLTIP
        label_23->setToolTip(QApplication::translate("ShiftDialog", "address to return deposit to if anything goes wrong with exchange", 0));
#endif // QT_NO_TOOLTIP
        label_23->setText(QApplication::translate("ShiftDialog", "Return Address:", 0));
#ifndef QT_NO_TOOLTIP
        label_24->setToolTip(QApplication::translate("ShiftDialog", "return Address Type", 0));
#endif // QT_NO_TOOLTIP
        label_24->setText(QApplication::translate("ShiftDialog", "Return Address Type:", 0));
        tabWidget->setTabText(tabWidget->indexOf(tabNetwork), QApplication::translate("ShiftDialog", "Normal Transaction", 0));
        groupBox_3->setTitle(QString());
        label_22->setText(QApplication::translate("ShiftDialog", "Withdrawal Amount", 0));
        label_25->setText(QApplication::translate("ShiftDialog", "Withdrawal Address", 0));
        label_26->setText(QApplication::translate("ShiftDialog", "Return Address", 0));
        postFixButton->setText(QApplication::translate("ShiftDialog", "POST", 0));
        groupBox_4->setTitle(QApplication::translate("ShiftDialog", "Output", 0));
        label_27->setText(QApplication::translate("ShiftDialog", "Post status:", 0));
#ifndef QT_NO_TOOLTIP
        label_28->setToolTip(QApplication::translate("ShiftDialog", "the address for resulting coin to be sent to", 0));
#endif // QT_NO_TOOLTIP
        label_28->setText(QApplication::translate("ShiftDialog", "Withdrawal Address:", 0));
#ifndef QT_NO_TOOLTIP
        label_29->setToolTip(QApplication::translate("ShiftDialog", "the address for resulting coin to be sent to", 0));
#endif // QT_NO_TOOLTIP
        label_29->setText(QApplication::translate("ShiftDialog", "Withdrawal Amount:", 0));
#ifndef QT_NO_TOOLTIP
        label_30->setToolTip(QApplication::translate("ShiftDialog", "Send your feathercoin to this address", 0));
#endif // QT_NO_TOOLTIP
        label_30->setText(QApplication::translate("ShiftDialog", "Deposit Address:", 0));
#ifndef QT_NO_TOOLTIP
        label_31->setToolTip(QApplication::translate("ShiftDialog", "the address for resulting coin to be sent to", 0));
#endif // QT_NO_TOOLTIP
        label_31->setText(QApplication::translate("ShiftDialog", "Deposit Amount:", 0));
        label_32->setText(QApplication::translate("ShiftDialog", "Rate:", 0));
        label_33->setText(QApplication::translate("ShiftDialog", "Expiration:", 0));
#ifndef QT_NO_TOOLTIP
        label_34->setToolTip(QApplication::translate("ShiftDialog", "address to return deposit to if anything goes wrong with exchange", 0));
#endif // QT_NO_TOOLTIP
        label_34->setText(QApplication::translate("ShiftDialog", "Return Address:", 0));
        label_35->setText(QApplication::translate("ShiftDialog", "Pair:", 0));
        tabWidget->setTabText(tabWidget->indexOf(tabWindow), QApplication::translate("ShiftDialog", "Fixed Amount Transaction", 0));
        textBrowser->setHtml(QApplication::translate("ShiftDialog", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Noto Sans'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'SimSun'; font-size:9pt; font-weight:600;\">Step1:</span><span style=\" font-family:'SimSun'; font-size:9pt;\"> Select Feathercoin as the input and Bitcoin as the output.</span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:'SimSun'; font-size:9pt;\"><br /></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'SimSun'"
                        "; font-size:9pt; font-weight:600;\">Step2:</span><span style=\" font-family:'SimSun'; font-size:9pt;\"> Provide your Bitcoin address in the box, then click the POST button.</span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:'SimSun'; font-size:9pt;\"><br /></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'SimSun'; font-size:9pt; font-weight:600;\">Step3</span><span style=\" font-family:'SimSun'; font-size:9pt;\">: ShapeShift will generate a FTC deposit address for you. Please send your FTCs to this generated address. (Don't send more than the Deposit Limit).</span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:'SimSun'; font-size:9pt;\"><br /></p>\n"
"<p style=\"-qt-para"
                        "graph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:'SimSun'; font-size:9pt;\"><br /></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'Helvetica Neue,Helvetica,Arial,sans-serif'; font-size:9pt; color:#31708f; background-color:#d9edf7;\">FTC Miner Fee: 0.1 FTC</span></p></body></html>", 0));
        tabWidget->setTabText(tabWidget->indexOf(tabDisplay), QApplication::translate("ShiftDialog", "More", 0));
        label_6->setText(QApplication::translate("ShiftDialog", "Connect  shapeshift's API (experts only!)", 0));
        cancelButton->setText(QApplication::translate("ShiftDialog", "&Exit", 0));
    } // retranslateUi

};

namespace Ui {
    class ShiftDialog: public Ui_ShiftDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SHIFTDIALOG_H
