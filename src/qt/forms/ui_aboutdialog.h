/********************************************************************************
** Form generated from reading UI file 'aboutdialog.ui'
**
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ABOUTDIALOG_H
#define UI_ABOUTDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_AboutDialog
{
public:
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_4;
    QVBoxLayout *verticalLayout_2;
    QSpacerItem *verticalSpacer_2;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLabel *versionLabel;
    QSpacerItem *horizontalSpacer;
    QLabel *copyrightLabel;
    QLabel *label_11;
    QLabel *label_2;
    QLabel *label_12;
    QLabel *label_3;
    QLabel *label_6;
    QLabel *label_13;
    QLabel *label_5;
    QLabel *label_7;
    QLabel *label_9;
    QLabel *label_10;
    QSpacerItem *verticalSpacer;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *AboutDialog)
    {
        if (AboutDialog->objectName().isEmpty())
            AboutDialog->setObjectName(QStringLiteral("AboutDialog"));
        AboutDialog->resize(555, 352);
        horizontalLayout_2 = new QHBoxLayout(AboutDialog);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label_4 = new QLabel(AboutDialog);
        label_4->setObjectName(QStringLiteral("label_4"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Ignored);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label_4->sizePolicy().hasHeightForWidth());
        label_4->setSizePolicy(sizePolicy);
        label_4->setPixmap(QPixmap(QString::fromUtf8(":/images/about")));
        label_4->setAlignment(Qt::AlignBottom|Qt::AlignLeading|Qt::AlignLeft);

        horizontalLayout_2->addWidget(label_4);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, -1, -1, -1);
        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer_2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label = new QLabel(AboutDialog);
        label->setObjectName(QStringLiteral("label"));
        label->setCursor(QCursor(Qt::IBeamCursor));
        label->setIndent(6);
        label->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByKeyboard|Qt::TextSelectableByMouse);

        horizontalLayout->addWidget(label);

        versionLabel = new QLabel(AboutDialog);
        versionLabel->setObjectName(QStringLiteral("versionLabel"));
        versionLabel->setCursor(QCursor(Qt::IBeamCursor));
        versionLabel->setText(QStringLiteral("0.9.6.1"));
        versionLabel->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByKeyboard|Qt::TextSelectableByMouse);

        horizontalLayout->addWidget(versionLabel);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout_2->addLayout(horizontalLayout);

        copyrightLabel = new QLabel(AboutDialog);
        copyrightLabel->setObjectName(QStringLiteral("copyrightLabel"));
        copyrightLabel->setCursor(QCursor(Qt::IBeamCursor));
        copyrightLabel->setText(QStringLiteral("Copyright &copy; 2013-2017  The Feathercoin Core developers"));
        copyrightLabel->setTextFormat(Qt::RichText);
        copyrightLabel->setMargin(0);
        copyrightLabel->setIndent(6);
        copyrightLabel->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByKeyboard|Qt::TextSelectableByMouse);

        verticalLayout_2->addWidget(copyrightLabel);

        label_11 = new QLabel(AboutDialog);
        label_11->setObjectName(QStringLiteral("label_11"));

        verticalLayout_2->addWidget(label_11);

        label_2 = new QLabel(AboutDialog);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setCursor(QCursor(Qt::IBeamCursor));
        label_2->setWordWrap(false);
        label_2->setIndent(6);
        label_2->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByKeyboard|Qt::TextSelectableByMouse);

        verticalLayout_2->addWidget(label_2);

        label_12 = new QLabel(AboutDialog);
        label_12->setObjectName(QStringLiteral("label_12"));

        verticalLayout_2->addWidget(label_12);

        label_3 = new QLabel(AboutDialog);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setIndent(6);

        verticalLayout_2->addWidget(label_3);

        label_6 = new QLabel(AboutDialog);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setIndent(6);

        verticalLayout_2->addWidget(label_6);

        label_13 = new QLabel(AboutDialog);
        label_13->setObjectName(QStringLiteral("label_13"));

        verticalLayout_2->addWidget(label_13);

        label_5 = new QLabel(AboutDialog);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setIndent(6);

        verticalLayout_2->addWidget(label_5);

        label_7 = new QLabel(AboutDialog);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setIndent(6);

        verticalLayout_2->addWidget(label_7);

        label_9 = new QLabel(AboutDialog);
        label_9->setObjectName(QStringLiteral("label_9"));
        label_9->setIndent(6);

        verticalLayout_2->addWidget(label_9);

        label_10 = new QLabel(AboutDialog);
        label_10->setObjectName(QStringLiteral("label_10"));
        label_10->setIndent(6);

        verticalLayout_2->addWidget(label_10);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer);

        buttonBox = new QDialogButtonBox(AboutDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Ok);

        verticalLayout_2->addWidget(buttonBox);


        horizontalLayout_2->addLayout(verticalLayout_2);


        retranslateUi(AboutDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), AboutDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), AboutDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(AboutDialog);
    } // setupUi

    void retranslateUi(QDialog *AboutDialog)
    {
        AboutDialog->setWindowTitle(QApplication::translate("AboutDialog", "About Feathercoin Core", 0));
        label->setText(QApplication::translate("AboutDialog", "<b>Feathercoin Core</b> version", 0));
        label_11->setText(QString());
        label_2->setText(QApplication::translate("AboutDialog", "This is experimental software.", 0));
        label_12->setText(QString());
        label_3->setText(QApplication::translate("AboutDialog", "Distributed under the MIT/X11 software license, see the accompanying file", 0));
        label_6->setText(QApplication::translate("AboutDialog", "COPYING or http://www.opensource.org/licenses/mit-license.php.", 0));
        label_13->setText(QString());
        label_5->setText(QApplication::translate("AboutDialog", "This product includes software developed by the OpenSSL Project for use", 0));
        label_7->setText(QApplication::translate("AboutDialog", "in the OpenSSL Toolkit (http://www.openssl.org/) and cryptographic", 0));
        label_9->setText(QApplication::translate("AboutDialog", "software written by Eric Young (eay@cryptsoft.com) and UPnP software", 0));
        label_10->setText(QApplication::translate("AboutDialog", "written by Thomas Bernard.", 0));
    } // retranslateUi

};

namespace Ui {
    class AboutDialog: public Ui_AboutDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ABOUTDIALOG_H
