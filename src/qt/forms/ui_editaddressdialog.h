/********************************************************************************
** Form generated from reading UI file 'editaddressdialog.ui'
**
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EDITADDRESSDIALOG_H
#define UI_EDITADDRESSDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QVBoxLayout>
#include "qvalidatedlineedit.h"

QT_BEGIN_NAMESPACE

class Ui_EditAddressDialog
{
public:
    QVBoxLayout *verticalLayout;
    QFormLayout *formLayout;
    QLabel *label;
    QLineEdit *labelEdit;
    QLabel *label_2;
    QValidatedLineEdit *addressEdit;
    QCheckBox *stealthCB;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_5;
    QLabel *lblPubkey;
    QLabel *lblPrikey;
    QLabel *label_8;
    QLabel *label_9;
    QLabel *lblSpendPubkeyT;
    QLabel *lblSpendSecretT;
    QLabel *lblScanPubkey;
    QLabel *lblScanSecret;
    QLabel *lblSpendPubkey;
    QLabel *lblSpendSecret;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *EditAddressDialog)
    {
        if (EditAddressDialog->objectName().isEmpty())
            EditAddressDialog->setObjectName(QStringLiteral("EditAddressDialog"));
        EditAddressDialog->resize(602, 236);
        verticalLayout = new QVBoxLayout(EditAddressDialog);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        formLayout = new QFormLayout();
        formLayout->setObjectName(QStringLiteral("formLayout"));
        formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        label = new QLabel(EditAddressDialog);
        label->setObjectName(QStringLiteral("label"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label);

        labelEdit = new QLineEdit(EditAddressDialog);
        labelEdit->setObjectName(QStringLiteral("labelEdit"));

        formLayout->setWidget(0, QFormLayout::FieldRole, labelEdit);

        label_2 = new QLabel(EditAddressDialog);
        label_2->setObjectName(QStringLiteral("label_2"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_2);

        addressEdit = new QValidatedLineEdit(EditAddressDialog);
        addressEdit->setObjectName(QStringLiteral("addressEdit"));

        formLayout->setWidget(1, QFormLayout::FieldRole, addressEdit);

        stealthCB = new QCheckBox(EditAddressDialog);
        stealthCB->setObjectName(QStringLiteral("stealthCB"));

        formLayout->setWidget(2, QFormLayout::LabelRole, stealthCB);

        label_3 = new QLabel(EditAddressDialog);
        label_3->setObjectName(QStringLiteral("label_3"));

        formLayout->setWidget(3, QFormLayout::LabelRole, label_3);

        label_4 = new QLabel(EditAddressDialog);
        label_4->setObjectName(QStringLiteral("label_4"));

        formLayout->setWidget(4, QFormLayout::LabelRole, label_4);

        label_5 = new QLabel(EditAddressDialog);
        label_5->setObjectName(QStringLiteral("label_5"));

        formLayout->setWidget(2, QFormLayout::FieldRole, label_5);

        lblPubkey = new QLabel(EditAddressDialog);
        lblPubkey->setObjectName(QStringLiteral("lblPubkey"));

        formLayout->setWidget(3, QFormLayout::FieldRole, lblPubkey);

        lblPrikey = new QLabel(EditAddressDialog);
        lblPrikey->setObjectName(QStringLiteral("lblPrikey"));

        formLayout->setWidget(4, QFormLayout::FieldRole, lblPrikey);

        label_8 = new QLabel(EditAddressDialog);
        label_8->setObjectName(QStringLiteral("label_8"));

        formLayout->setWidget(5, QFormLayout::LabelRole, label_8);

        label_9 = new QLabel(EditAddressDialog);
        label_9->setObjectName(QStringLiteral("label_9"));

        formLayout->setWidget(6, QFormLayout::LabelRole, label_9);

        lblSpendPubkeyT = new QLabel(EditAddressDialog);
        lblSpendPubkeyT->setObjectName(QStringLiteral("lblSpendPubkeyT"));

        formLayout->setWidget(7, QFormLayout::LabelRole, lblSpendPubkeyT);

        lblSpendSecretT = new QLabel(EditAddressDialog);
        lblSpendSecretT->setObjectName(QStringLiteral("lblSpendSecretT"));

        formLayout->setWidget(8, QFormLayout::LabelRole, lblSpendSecretT);

        lblScanPubkey = new QLabel(EditAddressDialog);
        lblScanPubkey->setObjectName(QStringLiteral("lblScanPubkey"));

        formLayout->setWidget(5, QFormLayout::FieldRole, lblScanPubkey);

        lblScanSecret = new QLabel(EditAddressDialog);
        lblScanSecret->setObjectName(QStringLiteral("lblScanSecret"));

        formLayout->setWidget(6, QFormLayout::FieldRole, lblScanSecret);

        lblSpendPubkey = new QLabel(EditAddressDialog);
        lblSpendPubkey->setObjectName(QStringLiteral("lblSpendPubkey"));

        formLayout->setWidget(7, QFormLayout::FieldRole, lblSpendPubkey);

        lblSpendSecret = new QLabel(EditAddressDialog);
        lblSpendSecret->setObjectName(QStringLiteral("lblSpendSecret"));

        formLayout->setWidget(8, QFormLayout::FieldRole, lblSpendSecret);


        verticalLayout->addLayout(formLayout);

        buttonBox = new QDialogButtonBox(EditAddressDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);

#ifndef QT_NO_SHORTCUT
        label->setBuddy(labelEdit);
        label_2->setBuddy(addressEdit);
#endif // QT_NO_SHORTCUT

        retranslateUi(EditAddressDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), EditAddressDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), EditAddressDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(EditAddressDialog);
    } // setupUi

    void retranslateUi(QDialog *EditAddressDialog)
    {
        EditAddressDialog->setWindowTitle(QApplication::translate("EditAddressDialog", "Edit Address", 0));
        label->setText(QApplication::translate("EditAddressDialog", "&Label", 0));
#ifndef QT_NO_TOOLTIP
        labelEdit->setToolTip(QApplication::translate("EditAddressDialog", "The label associated with this address list entry", 0));
#endif // QT_NO_TOOLTIP
        label_2->setText(QApplication::translate("EditAddressDialog", "&Address", 0));
#ifndef QT_NO_TOOLTIP
        addressEdit->setToolTip(QApplication::translate("EditAddressDialog", "The address associated with this address list entry. This can only be modified for sending addresses.", 0));
#endif // QT_NO_TOOLTIP
        stealthCB->setText(QApplication::translate("EditAddressDialog", "Stealth Address", 0));
        label_3->setText(QApplication::translate("EditAddressDialog", "Public Key", 0));
        label_4->setText(QApplication::translate("EditAddressDialog", "Private Key", 0));
        label_5->setText(QString());
        lblPubkey->setText(QApplication::translate("EditAddressDialog", "......", 0));
        lblPrikey->setText(QApplication::translate("EditAddressDialog", "......", 0));
        label_8->setText(QApplication::translate("EditAddressDialog", "Scan Pubkey", 0));
        label_9->setText(QApplication::translate("EditAddressDialog", "Scan Secret", 0));
        lblSpendPubkeyT->setText(QApplication::translate("EditAddressDialog", "Spend Pubkey", 0));
        lblSpendSecretT->setText(QApplication::translate("EditAddressDialog", "Spend Secret", 0));
        lblScanPubkey->setText(QApplication::translate("EditAddressDialog", "......", 0));
        lblScanSecret->setText(QApplication::translate("EditAddressDialog", "......", 0));
        lblSpendPubkey->setText(QApplication::translate("EditAddressDialog", "......", 0));
        lblSpendSecret->setText(QApplication::translate("EditAddressDialog", "......", 0));
    } // retranslateUi

};

namespace Ui {
    class EditAddressDialog: public Ui_EditAddressDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EDITADDRESSDIALOG_H
