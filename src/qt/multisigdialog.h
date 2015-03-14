#ifndef MULTISIGDIALOG_H
#define MULTISIGDIALOG_H

#include <QDialog>
#include <QString>

namespace Ui {
    class MultiSigDialog;
}

class CWallet;
class WalletModel;
class SendCoinsEntry;
class SendCoinsRecipient;
class CCoinControl;
class CTransaction;

QT_BEGIN_NAMESPACE
class QUrl;
QT_END_NAMESPACE

/** Dialog for sending bitcoins */
class MultiSigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MultiSigDialog(QWidget *parent = 0);
    ~MultiSigDialog();

    void setModel(WalletModel *model);

    /** Set up the tab chain manually, as Qt messes up the tab chain by default in some cases (issue https://bugreports.qt-project.org/browse/QTBUG-10907).
     */
    QWidget *setupTabChain(QWidget *prev);

    void setAddress(const QString &address);
    void pasteEntry(const SendCoinsRecipient &rv);
    bool handleURI(const QString &uri);

    static CCoinControl *coinControl;
    static CTransaction *rawTx;

public slots:
    void clear();
    void reject();
    void accept();
    void exportDraft();
    void importDraft();
    void editEnable(bool enable);

    void signAddress0();
    void signAddress1();
    void signAddress2();
    void signTransaction(QString *addrStr = NULL);
    
    SendCoinsEntry *addEntry();
    void updateRemoveEnabled();
    void setSharedBalance(qint64 balance, qint64 unconfirmedBalance, qint64 immatureBalance);

private:
    Ui::MultiSigDialog *ui;
    WalletModel *model;
    CWallet *wallet;
    
    bool fNewRecipientAllowed;
    void updateAddressList();
    void updateAddressDetail();
    void updateAddressBalance();

    bool isTxCreate;
    bool isComplete;
    void createRawTransaction();
    void sendRawTransaction();

    int currentIndex;
    int nCurrentIn;

    std::vector<std::string> containAddresses;
    void checkRawTransaction();

private slots:
    void on_sendButton_clicked();
    void removeEntry(SendCoinsEntry* entry);
    void updateDisplayUnit();
private slots:
    void handleAddrSelectionChanged(int idx);
    void handleAddressBookChanged();
};

#endif // MULTISIGDIALOG_H
