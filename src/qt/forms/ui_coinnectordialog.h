/********************************************************************************
** Form generated from reading UI file 'coinnectordialog.ui'
**
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_COINNECTORDIALOG_H
#define UI_COINNECTORDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_CoinnectorDialog
{
public:
    QVBoxLayout *verticalLayout;
    QFrame *frame;
    QVBoxLayout *verticalLayout_Bottom;
    QTableView *tableView;
    QHBoxLayout *horizontalLayout_Buttons;
    QSpacerItem *horizontalSpacer_1;
    QLineEdit *textReplyEdit;
    QLabel *label_6;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButton;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *cancelButton;

    void setupUi(QDialog *CoinnectorDialog)
    {
        if (CoinnectorDialog->objectName().isEmpty())
            CoinnectorDialog->setObjectName(QStringLiteral("CoinnectorDialog"));
        CoinnectorDialog->resize(560, 428);
        CoinnectorDialog->setModal(true);
        verticalLayout = new QVBoxLayout(CoinnectorDialog);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        frame = new QFrame(CoinnectorDialog);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setFrameShadow(QFrame::Raised);
        verticalLayout_Bottom = new QVBoxLayout(frame);
        verticalLayout_Bottom->setObjectName(QStringLiteral("verticalLayout_Bottom"));
        tableView = new QTableView(frame);
        tableView->setObjectName(QStringLiteral("tableView"));

        verticalLayout_Bottom->addWidget(tableView);


        verticalLayout->addWidget(frame);

        horizontalLayout_Buttons = new QHBoxLayout();
        horizontalLayout_Buttons->setObjectName(QStringLiteral("horizontalLayout_Buttons"));
        horizontalSpacer_1 = new QSpacerItem(40, 48, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_Buttons->addItem(horizontalSpacer_1);

        textReplyEdit = new QLineEdit(CoinnectorDialog);
        textReplyEdit->setObjectName(QStringLiteral("textReplyEdit"));

        horizontalLayout_Buttons->addWidget(textReplyEdit);

        label_6 = new QLabel(CoinnectorDialog);
        label_6->setObjectName(QStringLiteral("label_6"));
        QFont font;
        font.setUnderline(true);
        label_6->setFont(font);
        label_6->setCursor(QCursor(Qt::PointingHandCursor));

        horizontalLayout_Buttons->addWidget(label_6);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));

        horizontalLayout_Buttons->addLayout(horizontalLayout);

        pushButton = new QPushButton(CoinnectorDialog);
        pushButton->setObjectName(QStringLiteral("pushButton"));

        horizontalLayout_Buttons->addWidget(pushButton);

        horizontalSpacer_2 = new QSpacerItem(40, 48, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_Buttons->addItem(horizontalSpacer_2);

        cancelButton = new QPushButton(CoinnectorDialog);
        cancelButton->setObjectName(QStringLiteral("cancelButton"));
        cancelButton->setAutoDefault(false);

        horizontalLayout_Buttons->addWidget(cancelButton);


        verticalLayout->addLayout(horizontalLayout_Buttons);


        retranslateUi(CoinnectorDialog);

        QMetaObject::connectSlotsByName(CoinnectorDialog);
    } // setupUi

    void retranslateUi(QDialog *CoinnectorDialog)
    {
        CoinnectorDialog->setWindowTitle(QApplication::translate("CoinnectorDialog", "Coinnector", 0));
        textReplyEdit->setText(QString());
        label_6->setText(QApplication::translate("CoinnectorDialog", "Connect  Coinnector API (experts only!)", 0));
        pushButton->setText(QApplication::translate("CoinnectorDialog", "Refresh", 0));
        cancelButton->setText(QApplication::translate("CoinnectorDialog", "&Exit", 0));
    } // retranslateUi

};

namespace Ui {
    class CoinnectorDialog: public Ui_CoinnectorDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_COINNECTORDIALOG_H
