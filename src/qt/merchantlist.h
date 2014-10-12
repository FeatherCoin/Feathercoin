// Copyright (c) 2013-2014 The Feathercoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef MERCHANTLISTVIEW_H
#define MERCHANTLISTVIEW_H

#include <QWidget>

class ClientModel;
class TransactionFilterProxy;
class MerViewDelegate;
class WalletModel;

namespace Ui {
	class MerchantListView;
}


QT_BEGIN_NAMESPACE
class QModelIndex;
QT_END_NAMESPACE


class MerchantListView : public QWidget
{
    Q_OBJECT

public:
    explicit MerchantListView(QWidget *parent = 0);
    ~MerchantListView();

    void setClientModel(ClientModel *clientModel);
    void setWalletModel(WalletModel *walletModel);
    void showOutOfSyncWarning(bool fShow);

protected:
     bool eventFilter(QObject *obj, QEvent *ev);
     
public slots:
    void setBalance(qint64 balance, qint64 unconfirmedBalance, qint64 immatureBalance);
    void setOverview();

signals:
    void transactionClicked(const QModelIndex &index);

private:
    Ui::MerchantListView *ui;
    ClientModel *clientModel;
    WalletModel *walletModel;
    qint64 currentBalance;
    qint64 currentUnconfirmedBalance;
    qint64 currentImmatureBalance;

    MerViewDelegate *txdelegate;
    TransactionFilterProxy *filter;
    
private slots:
    void updateDisplayUnit();
    void handleTransactionClicked(const QModelIndex &index);
    void updateAlerts(const QString &warnings);
};

#endif // MERCHANTLISTVIEW_H
