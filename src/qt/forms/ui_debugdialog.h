/********************************************************************************
** Form generated from reading UI file 'debugdialog.ui'
**
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DEBUGDIALOG_H
#define UI_DEBUGDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_DebugDialog
{
public:
    QPushButton *sxButton;
    QDialogButtonBox *buttonBox;
    QLineEdit *addrEdit;
    QLabel *label;
    QPushButton *pushButton;
    QLabel *label_2;
    QLineEdit *addrEdit2;
    QLabel *label_3;
    QLabel *label_4;

    void setupUi(QDialog *DebugDialog)
    {
        if (DebugDialog->objectName().isEmpty())
            DebugDialog->setObjectName(QStringLiteral("DebugDialog"));
        DebugDialog->resize(556, 208);
        sxButton = new QPushButton(DebugDialog);
        sxButton->setObjectName(QStringLiteral("sxButton"));
        sxButton->setGeometry(QRect(30, 130, 191, 31));
        QIcon icon;
        icon.addFile(QStringLiteral(":/icons/sx"), QSize(), QIcon::Normal, QIcon::Off);
        sxButton->setIcon(icon);
        buttonBox = new QDialogButtonBox(DebugDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(610, 415, 81, 26));
        buttonBox->setStandardButtons(QDialogButtonBox::NoButton);
        buttonBox->setCenterButtons(true);
        addrEdit = new QLineEdit(DebugDialog);
        addrEdit->setObjectName(QStringLiteral("addrEdit"));
        addrEdit->setGeometry(QRect(30, 80, 131, 20));
        label = new QLabel(DebugDialog);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(30, 20, 191, 21));
        pushButton = new QPushButton(DebugDialog);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(240, 130, 81, 31));
        pushButton->setMaximumSize(QSize(16777215, 16177186));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/icons/quit"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton->setIcon(icon1);
        label_2 = new QLabel(DebugDialog);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(160, 60, 21, 16));
        addrEdit2 = new QLineEdit(DebugDialog);
        addrEdit2->setObjectName(QStringLiteral("addrEdit2"));
        addrEdit2->setGeometry(QRect(190, 80, 131, 20));
        label_3 = new QLabel(DebugDialog);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(30, 60, 121, 16));
        label_4 = new QLabel(DebugDialog);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(190, 60, 91, 16));

        retranslateUi(DebugDialog);

        QMetaObject::connectSlotsByName(DebugDialog);
    } // setupUi

    void retranslateUi(QDialog *DebugDialog)
    {
        DebugDialog->setWindowTitle(QApplication::translate("DebugDialog", "Stealth Transaction Tool (experts only!)", 0));
        sxButton->setText(QApplication::translate("DebugDialog", "Scan Stealth Transactions", 0));
#ifndef QT_NO_TOOLTIP
        addrEdit->setToolTip(QApplication::translate("DebugDialog", "Block Number", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        addrEdit->setStatusTip(QApplication::translate("DebugDialog", "Block Number", 0));
#endif // QT_NO_STATUSTIP
        addrEdit->setText(QApplication::translate("DebugDialog", "556535", 0));
        label->setText(QApplication::translate("DebugDialog", "Find My Stealth Transactions : ", 0));
        pushButton->setText(QApplication::translate("DebugDialog", "Exit", 0));
        label_2->setText(QApplication::translate("DebugDialog", "To", 0));
#ifndef QT_NO_TOOLTIP
        addrEdit2->setToolTip(QApplication::translate("DebugDialog", "Block Number", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        addrEdit2->setStatusTip(QApplication::translate("DebugDialog", "Block Number", 0));
#endif // QT_NO_STATUSTIP
        label_3->setText(QApplication::translate("DebugDialog", "From block Height:", 0));
        label_4->setText(QApplication::translate("DebugDialog", "End Height:", 0));
    } // retranslateUi

};

namespace Ui {
    class DebugDialog: public Ui_DebugDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DEBUGDIALOG_H
