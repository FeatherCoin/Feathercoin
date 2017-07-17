/********************************************************************************
** Form generated from reading UI file 'addressbookpage.ui'
**
** Created by: Qt User Interface Compiler version 5.6.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ADDRESSBOOKPAGE_H
#define UI_ADDRESSBOOKPAGE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AddressBookPage
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *labelExplanation;
    QTableView *tableView;
    QHBoxLayout *horizontalLayout;
    QPushButton *newAddress;
    QPushButton *newMultiSigAddress;
    QPushButton *copyAddress;
    QPushButton *deleteAddress;
    QSpacerItem *horizontalSpacer;
    QPushButton *signMessage;
    QPushButton *verifyMessage;
    QPushButton *exportButton;
    QPushButton *closeButton;

    void setupUi(QWidget *AddressBookPage)
    {
        if (AddressBookPage->objectName().isEmpty())
            AddressBookPage->setObjectName(QStringLiteral("AddressBookPage"));
        AddressBookPage->resize(840, 380);
        verticalLayout = new QVBoxLayout(AddressBookPage);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        labelExplanation = new QLabel(AddressBookPage);
        labelExplanation->setObjectName(QStringLiteral("labelExplanation"));
        labelExplanation->setTextFormat(Qt::PlainText);
        labelExplanation->setWordWrap(true);

        verticalLayout->addWidget(labelExplanation);

        tableView = new QTableView(AddressBookPage);
        tableView->setObjectName(QStringLiteral("tableView"));
        tableView->setContextMenuPolicy(Qt::CustomContextMenu);
        tableView->setTabKeyNavigation(false);
        tableView->setAlternatingRowColors(true);
        tableView->setSelectionMode(QAbstractItemView::SingleSelection);
        tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
        tableView->setSortingEnabled(true);
        tableView->verticalHeader()->setVisible(false);

        verticalLayout->addWidget(tableView);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        newAddress = new QPushButton(AddressBookPage);
        newAddress->setObjectName(QStringLiteral("newAddress"));
        QIcon icon;
        icon.addFile(QStringLiteral(":/icons/add"), QSize(), QIcon::Normal, QIcon::Off);
        newAddress->setIcon(icon);
        newAddress->setAutoDefault(false);

        horizontalLayout->addWidget(newAddress);

        newMultiSigAddress = new QPushButton(AddressBookPage);
        newMultiSigAddress->setObjectName(QStringLiteral("newMultiSigAddress"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/icons/verify"), QSize(), QIcon::Normal, QIcon::Off);
        newMultiSigAddress->setIcon(icon1);

        horizontalLayout->addWidget(newMultiSigAddress);

        copyAddress = new QPushButton(AddressBookPage);
        copyAddress->setObjectName(QStringLiteral("copyAddress"));
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/icons/editcopy"), QSize(), QIcon::Normal, QIcon::Off);
        copyAddress->setIcon(icon2);
        copyAddress->setAutoDefault(false);

        horizontalLayout->addWidget(copyAddress);

        deleteAddress = new QPushButton(AddressBookPage);
        deleteAddress->setObjectName(QStringLiteral("deleteAddress"));
        QIcon icon3;
        icon3.addFile(QStringLiteral(":/icons/remove"), QSize(), QIcon::Normal, QIcon::Off);
        deleteAddress->setIcon(icon3);
        deleteAddress->setAutoDefault(false);

        horizontalLayout->addWidget(deleteAddress);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        signMessage = new QPushButton(AddressBookPage);
        signMessage->setObjectName(QStringLiteral("signMessage"));
        QIcon icon4;
        icon4.addFile(QStringLiteral(":/icons/edit"), QSize(), QIcon::Normal, QIcon::Off);
        signMessage->setIcon(icon4);

        horizontalLayout->addWidget(signMessage);

        verifyMessage = new QPushButton(AddressBookPage);
        verifyMessage->setObjectName(QStringLiteral("verifyMessage"));
        QIcon icon5;
        icon5.addFile(QStringLiteral(":/icons/transaction_confirmed"), QSize(), QIcon::Normal, QIcon::Off);
        verifyMessage->setIcon(icon5);

        horizontalLayout->addWidget(verifyMessage);

        exportButton = new QPushButton(AddressBookPage);
        exportButton->setObjectName(QStringLiteral("exportButton"));
        QIcon icon6;
        icon6.addFile(QStringLiteral(":/icons/export"), QSize(), QIcon::Normal, QIcon::Off);
        exportButton->setIcon(icon6);
        exportButton->setAutoDefault(false);

        horizontalLayout->addWidget(exportButton);

        closeButton = new QPushButton(AddressBookPage);
        closeButton->setObjectName(QStringLiteral("closeButton"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(closeButton->sizePolicy().hasHeightForWidth());
        closeButton->setSizePolicy(sizePolicy);
        closeButton->setAutoDefault(false);

        horizontalLayout->addWidget(closeButton);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(AddressBookPage);

        QMetaObject::connectSlotsByName(AddressBookPage);
    } // setupUi

    void retranslateUi(QWidget *AddressBookPage)
    {
#ifndef QT_NO_TOOLTIP
        tableView->setToolTip(QApplication::translate("AddressBookPage", "Right-click to edit address or label", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        newAddress->setToolTip(QApplication::translate("AddressBookPage", "Create a new address", 0));
#endif // QT_NO_TOOLTIP
        newAddress->setText(QApplication::translate("AddressBookPage", "&New Address", 0));
        newMultiSigAddress->setText(QApplication::translate("AddressBookPage", "New &MultiSig", 0));
#ifndef QT_NO_TOOLTIP
        copyAddress->setToolTip(QApplication::translate("AddressBookPage", "Copy the currently selected address to the system clipboard", 0));
#endif // QT_NO_TOOLTIP
        copyAddress->setText(QApplication::translate("AddressBookPage", "&Copy", 0));
#ifndef QT_NO_TOOLTIP
        deleteAddress->setToolTip(QApplication::translate("AddressBookPage", "Delete the currently selected address from the list", 0));
#endif // QT_NO_TOOLTIP
        deleteAddress->setText(QApplication::translate("AddressBookPage", "&Delete", 0));
        signMessage->setText(QApplication::translate("AddressBookPage", "Sign Message", 0));
        verifyMessage->setText(QApplication::translate("AddressBookPage", "Verify Message", 0));
#ifndef QT_NO_TOOLTIP
        exportButton->setToolTip(QApplication::translate("AddressBookPage", "Export the data in the current tab to a file", 0));
#endif // QT_NO_TOOLTIP
        exportButton->setText(QApplication::translate("AddressBookPage", "&Export", 0));
        closeButton->setText(QApplication::translate("AddressBookPage", "C&lose", 0));
        Q_UNUSED(AddressBookPage);
    } // retranslateUi

};

namespace Ui {
    class AddressBookPage: public Ui_AddressBookPage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ADDRESSBOOKPAGE_H
