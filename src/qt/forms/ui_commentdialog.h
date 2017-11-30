/********************************************************************************
** Form generated from reading UI file 'commentdialog.ui'
**
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_COMMENTDIALOG_H
#define UI_COMMENTDIALOG_H

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

class Ui_CommentDialog
{
public:
    QPushButton *insertButton;
    QDialogButtonBox *buttonBox;
    QLineEdit *addrEdit;
    QLabel *label;
    QLabel *label_2;
    QPushButton *pushButton;
    QLineEdit *txtComment;

    void setupUi(QDialog *CommentDialog)
    {
        if (CommentDialog->objectName().isEmpty())
            CommentDialog->setObjectName(QStringLiteral("CommentDialog"));
        CommentDialog->resize(556, 208);
        insertButton = new QPushButton(CommentDialog);
        insertButton->setObjectName(QStringLiteral("insertButton"));
        insertButton->setGeometry(QRect(30, 140, 171, 31));
        QIcon icon;
        icon.addFile(QStringLiteral(":/icons/add"), QSize(), QIcon::Normal, QIcon::Off);
        insertButton->setIcon(icon);
        buttonBox = new QDialogButtonBox(CommentDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(610, 415, 81, 26));
        buttonBox->setStandardButtons(QDialogButtonBox::NoButton);
        buttonBox->setCenterButtons(true);
        addrEdit = new QLineEdit(CommentDialog);
        addrEdit->setObjectName(QStringLiteral("addrEdit"));
        addrEdit->setGeometry(QRect(30, 40, 491, 20));
        label = new QLabel(CommentDialog);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(30, 20, 111, 16));
        label_2 = new QLabel(CommentDialog);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(30, 71, 171, 21));
        pushButton = new QPushButton(CommentDialog);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(210, 140, 81, 31));
        pushButton->setMaximumSize(QSize(16777215, 16177186));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/icons/quit"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton->setIcon(icon1);
        txtComment = new QLineEdit(CommentDialog);
        txtComment->setObjectName(QStringLiteral("txtComment"));
        txtComment->setGeometry(QRect(30, 100, 491, 21));

        retranslateUi(CommentDialog);

        QMetaObject::connectSlotsByName(CommentDialog);
    } // setupUi

    void retranslateUi(QDialog *CommentDialog)
    {
        CommentDialog->setWindowTitle(QApplication::translate("CommentDialog", "Insert your comments into blockchain", 0));
        insertButton->setText(QApplication::translate("CommentDialog", "Insert into BlockChain", 0));
#ifndef QT_NO_TOOLTIP
        addrEdit->setToolTip(QApplication::translate("CommentDialog", "Recipient address", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        addrEdit->setStatusTip(QApplication::translate("CommentDialog", "Recipient address", 0));
#endif // QT_NO_STATUSTIP
        label->setText(QApplication::translate("CommentDialog", "Pay to Address :", 0));
        label_2->setText(QApplication::translate("CommentDialog", "Comments (35 Characters) :", 0));
        pushButton->setText(QApplication::translate("CommentDialog", "Exit", 0));
    } // retranslateUi

};

namespace Ui {
    class CommentDialog: public Ui_CommentDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_COMMENTDIALOG_H
