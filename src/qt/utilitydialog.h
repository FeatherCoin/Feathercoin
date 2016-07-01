// Copyright (c) 2011-2014 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_QT_UTILITYDIALOG_H
#define BITCOIN_QT_UTILITYDIALOG_H

#include <QDialog>
#include <QObject>
#include "walletmodel.h"

class BitcoinGUI;
class ClientModel;
class SendCoinsRecipient;

namespace Ui {
    class HelpMessageDialog;
    class CommentDialog;
}

/** "Help message" dialog box */
class HelpMessageDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HelpMessageDialog(QWidget *parent, bool about);
    ~HelpMessageDialog();

    void printToConsole();
    void showOrPrint();

private:
    Ui::HelpMessageDialog *ui;
    QString text;

private Q_SLOTS:
    void on_okButton_accepted();
};


/** "Shutdown" window */
class ShutdownWindow : public QWidget
{
    Q_OBJECT

public:
    ShutdownWindow(QWidget *parent=0, Qt::WindowFlags f=0);
    static void showShutdownWindow(BitcoinGUI *window);

protected:
    void closeEvent(QCloseEvent *event);
};


/** "Comment" dialog box */
class CommentDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CommentDialog(QWidget *parent);
    ~CommentDialog();

    void setModel(WalletModel *model);

private:
    Ui::CommentDialog *ui;
    WalletModel *model;
    void processSendCoinsReturn(const WalletModel::SendCoinsReturn &sendCoinsReturn, const QString &msgArg = QString());

private Q_SLOTS:
    void on_insertButton_clicked();
    void on_pushButton_clicked();
    
Q_SIGNALS:	
    void message(const QString &title, const QString &message, unsigned int style);
};

#endif // BITCOIN_QT_UTILITYDIALOG_H
