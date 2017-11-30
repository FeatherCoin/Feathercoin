/********************************************************************************
** Form generated from reading UI file 'opennamedialog.ui'
**
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OPENNAMEDIALOG_H
#define UI_OPENNAMEDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_OpennameDialog
{
public:
    QPushButton *insertButton;
    QDialogButtonBox *buttonBox;
    QLineEdit *txtName;
    QLabel *lblName;
    QLabel *lblLocation;
    QPushButton *pushButton;
    QLineEdit *txtLocation;
    QLabel *lblContact;
    QLineEdit *txtContact;
    QLabel *lblNotice;
    QLabel *lblOperations;
    QComboBox *cmbOpt;
    QLabel *lblPayTo;
    QLineEdit *txtPayAdress;
    QLabel *lblNamePrivkey;
    QLineEdit *txtNameAddress;

    void setupUi(QDialog *OpennameDialog)
    {
        if (OpennameDialog->objectName().isEmpty())
            OpennameDialog->setObjectName(QStringLiteral("OpennameDialog"));
        OpennameDialog->resize(552, 382);
        insertButton = new QPushButton(OpennameDialog);
        insertButton->setObjectName(QStringLiteral("insertButton"));
        insertButton->setGeometry(QRect(30, 330, 181, 31));
        QIcon icon;
        icon.addFile(QStringLiteral(":/icons/add"), QSize(), QIcon::Normal, QIcon::Off);
        insertButton->setIcon(icon);
        buttonBox = new QDialogButtonBox(OpennameDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(610, 415, 81, 26));
        buttonBox->setStandardButtons(QDialogButtonBox::NoButton);
        buttonBox->setCenterButtons(true);
        txtName = new QLineEdit(OpennameDialog);
        txtName->setObjectName(QStringLiteral("txtName"));
        txtName->setGeometry(QRect(30, 100, 451, 20));
        lblName = new QLabel(OpennameDialog);
        lblName->setObjectName(QStringLiteral("lblName"));
        lblName->setGeometry(QRect(30, 80, 54, 12));
        lblLocation = new QLabel(OpennameDialog);
        lblLocation->setObjectName(QStringLiteral("lblLocation"));
        lblLocation->setGeometry(QRect(30, 130, 54, 12));
        pushButton = new QPushButton(OpennameDialog);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(220, 330, 81, 31));
        pushButton->setMaximumSize(QSize(16777215, 16177186));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/icons/quit"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton->setIcon(icon1);
        txtLocation = new QLineEdit(OpennameDialog);
        txtLocation->setObjectName(QStringLiteral("txtLocation"));
        txtLocation->setGeometry(QRect(30, 150, 451, 20));
        lblContact = new QLabel(OpennameDialog);
        lblContact->setObjectName(QStringLiteral("lblContact"));
        lblContact->setGeometry(QRect(30, 180, 54, 12));
        txtContact = new QLineEdit(OpennameDialog);
        txtContact->setObjectName(QStringLiteral("txtContact"));
        txtContact->setGeometry(QRect(30, 200, 451, 20));
        lblNotice = new QLabel(OpennameDialog);
        lblNotice->setObjectName(QStringLiteral("lblNotice"));
        lblNotice->setGeometry(QRect(150, 10, 561, 21));
        lblOperations = new QLabel(OpennameDialog);
        lblOperations->setObjectName(QStringLiteral("lblOperations"));
        lblOperations->setGeometry(QRect(30, 230, 71, 16));
        cmbOpt = new QComboBox(OpennameDialog);
        cmbOpt->setObjectName(QStringLiteral("cmbOpt"));
        cmbOpt->setGeometry(QRect(120, 230, 231, 22));
        lblPayTo = new QLabel(OpennameDialog);
        lblPayTo->setObjectName(QStringLiteral("lblPayTo"));
        lblPayTo->setGeometry(QRect(30, 30, 54, 12));
        txtPayAdress = new QLineEdit(OpennameDialog);
        txtPayAdress->setObjectName(QStringLiteral("txtPayAdress"));
        txtPayAdress->setGeometry(QRect(30, 50, 451, 20));
        lblNamePrivkey = new QLabel(OpennameDialog);
        lblNamePrivkey->setObjectName(QStringLiteral("lblNamePrivkey"));
        lblNamePrivkey->setGeometry(QRect(30, 270, 141, 16));
        txtNameAddress = new QLineEdit(OpennameDialog);
        txtNameAddress->setObjectName(QStringLiteral("txtNameAddress"));
        txtNameAddress->setGeometry(QRect(30, 290, 401, 20));

        retranslateUi(OpennameDialog);

        QMetaObject::connectSlotsByName(OpennameDialog);
    } // setupUi

    void retranslateUi(QDialog *OpennameDialog)
    {
        OpennameDialog->setWindowTitle(QApplication::translate("OpennameDialog", "Insert your Opennames into blockchain", 0));
        insertButton->setText(QApplication::translate("OpennameDialog", "Commit into BlockChain", 0));
#ifndef QT_NO_TOOLTIP
        txtName->setToolTip(QApplication::translate("OpennameDialog", "Your Name", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        txtName->setStatusTip(QApplication::translate("OpennameDialog", "Your Name", 0));
#endif // QT_NO_STATUSTIP
        lblName->setText(QApplication::translate("OpennameDialog", "Name:", 0));
        lblLocation->setText(QApplication::translate("OpennameDialog", "Location:", 0));
        pushButton->setText(QApplication::translate("OpennameDialog", "Exit", 0));
        lblContact->setText(QApplication::translate("OpennameDialog", "Contact:", 0));
        lblNotice->setText(QApplication::translate("OpennameDialog", "All fields must be 40 characters or less.", 0));
        lblOperations->setText(QApplication::translate("OpennameDialog", "Operations:", 0));
        lblPayTo->setText(QApplication::translate("OpennameDialog", "Pay To:", 0));
#ifndef QT_NO_TOOLTIP
        txtPayAdress->setToolTip(QApplication::translate("OpennameDialog", "Your Name", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        txtPayAdress->setStatusTip(QApplication::translate("OpennameDialog", "Your Name", 0));
#endif // QT_NO_STATUSTIP
        lblNamePrivkey->setText(QApplication::translate("OpennameDialog", "PublicKey Hash160:", 0));
#ifndef QT_NO_TOOLTIP
        txtNameAddress->setToolTip(QApplication::translate("OpennameDialog", "Your Name", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        txtNameAddress->setStatusTip(QApplication::translate("OpennameDialog", "Your Name", 0));
#endif // QT_NO_STATUSTIP
    } // retranslateUi

};

namespace Ui {
    class OpennameDialog: public Ui_OpennameDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OPENNAMEDIALOG_H
