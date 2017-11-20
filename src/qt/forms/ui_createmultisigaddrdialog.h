/********************************************************************************
** Form generated from reading UI file 'createmultisigaddrdialog.ui'
**
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CREATEMULTISIGADDRDIALOG_H
#define UI_CREATEMULTISIGADDRDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_CreateMultiSigAddrDialog
{
public:
    QVBoxLayout *verticalLayout;
    QVBoxLayout *verticalLayout_3;
    QLabel *label0;
    QLineEdit *pubkeyEdit0;
    QLabel *label1;
    QLineEdit *pubkeyEdit1;
    QLabel *label2;
    QLineEdit *pubkeyEdit2;
    QLabel *labelPromptText;
    QHBoxLayout *horizontalLayout;
    QLabel *label_3;
    QComboBox *comboBoxRequire;
    QLabel *label_4;
    QComboBox *comboBoxTotal;
    QSpacerItem *horizontalSpacer;
    QPushButton *btnImport;
    QPushButton *btnCreate;
    QPushButton *btnCancel;

    void setupUi(QDialog *CreateMultiSigAddrDialog)
    {
        if (CreateMultiSigAddrDialog->objectName().isEmpty())
            CreateMultiSigAddrDialog->setObjectName(QStringLiteral("CreateMultiSigAddrDialog"));
        CreateMultiSigAddrDialog->resize(506, 209);
        verticalLayout = new QVBoxLayout(CreateMultiSigAddrDialog);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        label0 = new QLabel(CreateMultiSigAddrDialog);
        label0->setObjectName(QStringLiteral("label0"));

        verticalLayout_3->addWidget(label0);

        pubkeyEdit0 = new QLineEdit(CreateMultiSigAddrDialog);
        pubkeyEdit0->setObjectName(QStringLiteral("pubkeyEdit0"));

        verticalLayout_3->addWidget(pubkeyEdit0);

        label1 = new QLabel(CreateMultiSigAddrDialog);
        label1->setObjectName(QStringLiteral("label1"));

        verticalLayout_3->addWidget(label1);

        pubkeyEdit1 = new QLineEdit(CreateMultiSigAddrDialog);
        pubkeyEdit1->setObjectName(QStringLiteral("pubkeyEdit1"));

        verticalLayout_3->addWidget(pubkeyEdit1);

        label2 = new QLabel(CreateMultiSigAddrDialog);
        label2->setObjectName(QStringLiteral("label2"));

        verticalLayout_3->addWidget(label2);

        pubkeyEdit2 = new QLineEdit(CreateMultiSigAddrDialog);
        pubkeyEdit2->setObjectName(QStringLiteral("pubkeyEdit2"));

        verticalLayout_3->addWidget(pubkeyEdit2);


        verticalLayout->addLayout(verticalLayout_3);

        labelPromptText = new QLabel(CreateMultiSigAddrDialog);
        labelPromptText->setObjectName(QStringLiteral("labelPromptText"));

        verticalLayout->addWidget(labelPromptText);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label_3 = new QLabel(CreateMultiSigAddrDialog);
        label_3->setObjectName(QStringLiteral("label_3"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label_3->sizePolicy().hasHeightForWidth());
        label_3->setSizePolicy(sizePolicy);
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        label_3->setFont(font);

        horizontalLayout->addWidget(label_3);

        comboBoxRequire = new QComboBox(CreateMultiSigAddrDialog);
        comboBoxRequire->setObjectName(QStringLiteral("comboBoxRequire"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(comboBoxRequire->sizePolicy().hasHeightForWidth());
        comboBoxRequire->setSizePolicy(sizePolicy1);
        comboBoxRequire->setMaximumSize(QSize(16777215, 16777215));

        horizontalLayout->addWidget(comboBoxRequire);

        label_4 = new QLabel(CreateMultiSigAddrDialog);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setFont(font);

        horizontalLayout->addWidget(label_4);

        comboBoxTotal = new QComboBox(CreateMultiSigAddrDialog);
        comboBoxTotal->setObjectName(QStringLiteral("comboBoxTotal"));

        horizontalLayout->addWidget(comboBoxTotal);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        btnImport = new QPushButton(CreateMultiSigAddrDialog);
        btnImport->setObjectName(QStringLiteral("btnImport"));

        horizontalLayout->addWidget(btnImport);

        btnCreate = new QPushButton(CreateMultiSigAddrDialog);
        btnCreate->setObjectName(QStringLiteral("btnCreate"));

        horizontalLayout->addWidget(btnCreate);

        btnCancel = new QPushButton(CreateMultiSigAddrDialog);
        btnCancel->setObjectName(QStringLiteral("btnCancel"));

        horizontalLayout->addWidget(btnCancel);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(CreateMultiSigAddrDialog);

        QMetaObject::connectSlotsByName(CreateMultiSigAddrDialog);
    } // setupUi

    void retranslateUi(QDialog *CreateMultiSigAddrDialog)
    {
        CreateMultiSigAddrDialog->setWindowTitle(QApplication::translate("CreateMultiSigAddrDialog", "Create MultiSig Address", 0));
        label0->setText(QApplication::translate("CreateMultiSigAddrDialog", "PublicKey", 0));
#ifndef QT_NO_TOOLTIP
        pubkeyEdit0->setToolTip(QString());
#endif // QT_NO_TOOLTIP
        label1->setText(QApplication::translate("CreateMultiSigAddrDialog", "PublicKey", 0));
#ifndef QT_NO_TOOLTIP
        pubkeyEdit1->setToolTip(QString());
#endif // QT_NO_TOOLTIP
        label2->setText(QApplication::translate("CreateMultiSigAddrDialog", "PublicKey", 0));
        labelPromptText->setText(QApplication::translate("CreateMultiSigAddrDialog", "You need y keys to create a x-of-y Multisig address", 0));
        label_3->setText(QApplication::translate("CreateMultiSigAddrDialog", "Require", 0));
        label_4->setText(QApplication::translate("CreateMultiSigAddrDialog", "of", 0));
        btnImport->setText(QApplication::translate("CreateMultiSigAddrDialog", "Import", 0));
        btnCreate->setText(QApplication::translate("CreateMultiSigAddrDialog", "Create", 0));
        btnCancel->setText(QApplication::translate("CreateMultiSigAddrDialog", "Cancel", 0));
    } // retranslateUi

};

namespace Ui {
    class CreateMultiSigAddrDialog: public Ui_CreateMultiSigAddrDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CREATEMULTISIGADDRDIALOG_H
