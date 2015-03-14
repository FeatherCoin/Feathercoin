#include "multisigdialog.h"
#include "ui_multisigdialog.h"

#include "wallet.h"
#include "walletmodel.h"
#include "bitcoinunits.h"
#include "addressbookpage.h"
#include "optionsmodel.h"
#include "sendcoinsentry.h"
#include "guiutil.h"
#include "askpassphrasedialog.h"
#include "base58.h"
#include "init.h"
#include "coincontrol.h"
#include "createmultisigaddrdialog.h"
#include "rpcprotocol.h"

#include "json/json_spirit.h"
#include "json/json_spirit_reader_template.h"
#include "json/json_spirit_writer_template.h"
#include "json/json_spirit_utils.h"
#include "json/json_spirit_value.h"
using namespace json_spirit;

#include <QMessageBox>
#include <QTextDocument>
#include <QScrollBar>
#include <QFile>
#include <QTextStream>

CCoinControl* MultiSigDialog::coinControl = new CCoinControl();
CTransaction* MultiSigDialog::rawTx = new CTransaction();

MultiSigDialog::MultiSigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MultiSigDialog),
    model(0)
{
    ui->setupUi(this);

#ifdef Q_OS_MAC // Icons on push buttons are very uncommon on Mac
    ui->addButton->setIcon(QIcon());
    ui->clearButton->setIcon(QIcon());
    ui->sendButton->setIcon(QIcon());
    ui->btnExportDraft->setIcon(QIcon());
    ui->btnImportDraft->setIcon(QIcon());
    ui->btnCreateAddr->setIcon(QIcon());
#endif

    addEntry();

    connect(ui->addButton, SIGNAL(clicked()), this, SLOT(addEntry()));
    connect(ui->clearButton, SIGNAL(clicked()), this, SLOT(clear()));
    connect(ui->btnExportDraft, SIGNAL(clicked()), this, SLOT(exportDraft()));
    connect(ui->btnImportDraft, SIGNAL(clicked()), this, SLOT(importDraft()));
    
    connect(ui->btnSign0, SIGNAL(clicked()), this, SLOT(signAddress0()));
    connect(ui->btnSign1, SIGNAL(clicked()), this, SLOT(signAddress1()));
    connect(ui->btnSign2, SIGNAL(clicked()), this, SLOT(signAddress2()));

    fNewRecipientAllowed = true;

    currentIndex = -1;
    nCurrentIn = 0;
    isTxCreate = false;
    isComplete = false;
    connect(ui->comboBoxAddrList, SIGNAL(currentIndexChanged(int)), this, SLOT(handleAddrSelectionChanged(int)));
    ui->sendButton->setEnabled(false);
}

void MultiSigDialog::setModel(WalletModel *model)
{
    this->model = model;
    this->wallet = model->getWallet();

    for(int i = 0; i < ui->entries->count(); ++i)
    {
        SendCoinsEntry *entry = qobject_cast<SendCoinsEntry*>(ui->entries->itemAt(i)->widget());
        if(entry)
        {
            entry->setModel(model);
        }
    }
    if(model && model->getOptionsModel())
    {
        setSharedBalance(model->getSharedBalance(), model->getSharedUnconfirmedBalance(), model->getSharedImmatureBalance());
        connect(model, SIGNAL(sharedBalanceChanged(qint64, qint64, qint64)), this, SLOT(setSharedBalance(qint64, qint64, qint64)));
        connect(model->getOptionsModel(), SIGNAL(displayUnitChanged(int)), this, SLOT(updateDisplayUnit()));
        connect(model, SIGNAL(addressBookChanged()), this, SLOT(handleAddressBookChanged()));
    }
}

MultiSigDialog::~MultiSigDialog()
{
    delete ui;
}

static CCoinControl* coinControlTx = new CCoinControl();
void MultiSigDialog::createRawTransaction()
{
    QList<SendCoinsRecipient> recipients;
    bool valid = true;

    if(!model)
        return;

    for(int i = 0; i < ui->entries->count(); ++i)
    {
        SendCoinsEntry *entry = qobject_cast<SendCoinsEntry*>(ui->entries->itemAt(i)->widget());
        if(entry)
        {
            if(entry->validate())
            {
                recipients.append(entry->getValue());
            }
            else
            {
                valid = false;
            }
        }
    }

    if(!valid || recipients.isEmpty())
    {
        return;
    }

    if ( !coinControl->HasSelected() )
    {
        QMessageBox::warning(this, tr("Send Coins"),
            tr("The amount exceeds your balance."),
            QMessageBox::Ok, QMessageBox::Ok);

        return;
    }

    // Format confirmation message
    QStringList formatted;
    foreach(const SendCoinsRecipient &rcp, recipients)
    {
#if QT_VERSION < 0x050000
        formatted.append(tr("<b>%1</b> to %2 (%3)").arg(BitcoinUnits::formatWithUnit(BitcoinUnits::FTC, rcp.amount), Qt::escape(rcp.label), rcp.address));
#else
        formatted.append(tr("<b>%1</b> to %2 (%3)").arg(BitcoinUnits::formatWithUnit(BitcoinUnits::FTC, rcp.amount), rcp.label.toHtmlEscaped(), rcp.address));
#endif
    }

    fNewRecipientAllowed = false;

    QMessageBox::StandardButton retval = QMessageBox::question(this, tr("Confirm send coins"),
                          tr("Are you sure you want to send %1?").arg(formatted.join(tr(" and "))),
          QMessageBox::Yes|QMessageBox::Cancel,
          QMessageBox::Cancel);

    if(retval != QMessageBox::Yes)
    {
        fNewRecipientAllowed = true;
        return;
    }
/*2014-06-11
    model->isMultiSig = true;
    WalletModel::UnlockContext ctx(model->requestUnlock());
    if(!ctx.isValid())
    {
        // Unlock wallet was cancelled
        fNewRecipientAllowed = true;
        return;
    }
*/
    rawTx->SetNull();
    std::vector<COutPoint> vOutpoints;
    coinControl->ListSelected(vOutpoints);
    
    CCoinControl* coinControlCur = coinControl;
    // too many transactions
    if ( vOutpoints.size() > 10 )
    {
        qint64 total = 0;
        foreach(const SendCoinsRecipient &rcp, recipients)
            total += rcp.amount;
    
        coinControlTx->SetNull();
        int64 nValueRet = 0;
        {
            LOCK(pwalletMain->cs_wallet);
            for ( int i = 0; i < vOutpoints.size(); ++ i )
            {
                std::map<uint256, CWalletTx>::const_iterator mi = pwalletMain->mapWallet.find(vOutpoints[i].hash);
                if (mi != pwalletMain->mapWallet.end())
                {
                    const CWalletTx& prev = (*mi).second;
                    const CTxOut& txout = prev.vout[vOutpoints[i].n];
                    nValueRet += txout.nValue;
                    coinControlTx->Select(vOutpoints[i]);
                    if ( nValueRet > total )
                        break;
                }
            }
        }
        
        coinControlCur = coinControlTx;
    }
    
    WalletModel::SendCoinsReturn sendstatus = model->createRawTransaction(recipients, *rawTx, coinControlCur, true);
    switch(sendstatus.status)
    {
    case WalletModel::InvalidAddress:
        QMessageBox::warning(this, tr("Send Coins"),
            tr("The recipient address is not valid, please recheck."),
            QMessageBox::Ok, QMessageBox::Ok);
        break;
    case WalletModel::InvalidAmount:
        QMessageBox::warning(this, tr("Send Coins"),
            tr("The amount to pay must be larger than 0."),
            QMessageBox::Ok, QMessageBox::Ok);
        break;
    case WalletModel::AmountExceedsBalance:
        QMessageBox::warning(this, tr("Send Coins"),
            tr("The amount exceeds your balance."),
            QMessageBox::Ok, QMessageBox::Ok);
        break;
    case WalletModel::AmountWithFeeExceedsBalance:
        QMessageBox::warning(this, tr("Send Coins"),
            tr("The total exceeds your balance when the %1 transaction fee is included.").
            arg(BitcoinUnits::formatWithUnit(BitcoinUnits::FTC, sendstatus.fee)),
            QMessageBox::Ok, QMessageBox::Ok);
        break;
    case WalletModel::DuplicateAddress:
        QMessageBox::warning(this, tr("Send Coins"),
            tr("Duplicate address found, can only send to each address once per send operation."),
            QMessageBox::Ok, QMessageBox::Ok);
        break;
    case WalletModel::TransactionCreationFailed:
        QMessageBox::warning(this, tr("Send Coins"),
            tr("Error: Transaction creation failed!"),
            QMessageBox::Ok, QMessageBox::Ok);
        break;
    case WalletModel::TransactionCommitFailed:
        QMessageBox::warning(this, tr("Send Coins"),
            tr("Error: The transaction was rejected. This might happen if some of the coins in your wallet were already spent, such as if you used a copy of wallet.dat and coins were spent in the copy but not marked as spent here."),
            QMessageBox::Ok, QMessageBox::Ok);
        break;
    case WalletModel::Aborted: // User aborted, nothing to do
        break;
    case WalletModel::OK:
        isTxCreate = true;
        break;
    }
    fNewRecipientAllowed = true;
}

bool writeString(const QString &filename, const QString& hex)
{
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;
    
    QTextStream out(&file);
    out << hex;
    file.close();

    return file.error() == QFile::NoError;
}

void MultiSigDialog::importDraft()
{
    QString filename = GUIUtil::getLoadFileName(
            this,
            tr("Load Feathercoin Transaction"), QString(),
            tr("Feathercoin transaction file (*.ftc)"));

    if (filename.isNull()) return;
    
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QString hex;
    QTextStream hexin(&file);
    hexin >> hex;

    std::vector<unsigned char> txData(ParseHex(hex.toStdString()));
    CTransaction tx;
    CDataStream ssData(txData, SER_NETWORK, PROTOCOL_VERSION);
    try {
        ssData >> tx;
    }
    catch (std::exception &e) {
        return;
    }

    clear();
    for (unsigned int i = 0; i < tx.vin.size(); i++)
    {
        const CTxIn& txin = tx.vin[i];
        {
            LOCK(pwalletMain->cs_wallet);
            std::map<uint256, CWalletTx>::const_iterator mi = pwalletMain->mapWallet.find(txin.prevout.hash);
            if (mi != pwalletMain->mapWallet.end())
            {
                const CWalletTx& prev = (*mi).second;
                if (txin.prevout.n < prev.vout.size())
                {
                    const CTxOut& txout = prev.vout[txin.prevout.n];
                    std::vector<CTxDestination> addresses;
                    txnouttype whichType;
                    int nRequired;
                    if (!ExtractDestinations(txout.scriptPubKey, whichType, addresses, nRequired))
                        continue;

                    QString addrStr = QString::fromStdString(CBitcoinAddress(addresses[0]).ToString());
                    for ( int j = 0; j < ui->comboBoxAddrList->count(); j ++ )
                    {
                        if ( addrStr == ui->comboBoxAddrList->itemData(j).toString() )
                        {
                            nCurrentIn = i;
                            ui->comboBoxAddrList->setCurrentIndex(j);
                            if ( currentIndex != j )
                            {
                                currentIndex = j;
                                updateAddressBalance();
                                updateAddressDetail();
                            }
                            break;
                        }
                    }
                }
            }
        }
    }
    
    for (unsigned int i = 0; i < tx.vout.size(); i++)
    {
        const CTxOut& txout = tx.vout[i];
        std::vector<CTxDestination> addresses;
        txnouttype whichType;
        int nRequired;
        if (!ExtractDestinations(txout.scriptPubKey, whichType, addresses, nRequired))
            continue;

        QString addrStr = QString::fromStdString(CBitcoinAddress(addresses[0]).ToString());
        if ( addrStr == ui->comboBoxAddrList->itemData(ui->comboBoxAddrList->currentIndex()).toString() )
            continue;
            
        SendCoinsRecipient rv;
        rv.address = addrStr;
        rv.amount = txout.nValue;
        pasteEntry(rv);
    }
    
    *rawTx = tx;
    isTxCreate = true;
    editEnable(false);
    checkRawTransaction();
}

void MultiSigDialog::exportDraft()
{
    if ( !isTxCreate )
        createRawTransaction();
    
    if ( isTxCreate )
    {
        QString filename = GUIUtil::getSaveFileName(
                this,
                tr("Save Feathercoin Transaction"), QString(),
                tr("Feathercoin transaction file (*.ftc)"), NULL);

        if (filename.isNull()) return;

        CDataStream ss(SER_NETWORK, PROTOCOL_VERSION);
        ss << (*rawTx);
        QString hex = QString::fromStdString(HexStr(ss.begin(), ss.end()));

        if(!writeString(filename, hex))
        {
            QMessageBox::critical(this, tr("Error exporting"), tr("Could not write to file %1.").arg(filename),
                                  QMessageBox::Abort, QMessageBox::Abort);
        }
    }
}

/*void MultiSigDialog::sendRawTransaction()
{
    uint256 hashTx = rawTx->GetHash();

    bool fHave = false;
    CCoinsViewCache &view = *pcoinsTip;
    CCoins existingCoins;
    {
        fHave = view.GetCoins(hashTx, existingCoins);
        if (!fHave) {
            // push to local node
            CValidationState state;
            //if (!rawTx->AcceptToMemoryPool(state, true, false))
            if (AcceptToMemoryPool(mempool, state, rawTx, false, NULL, false))
                return;
                //throw JSONRPCError(RPC_DESERIALIZATION_ERROR, "TX rejected");
        }
    }
    if (fHave) {
        return;
        //if (existingCoins.nHeight < 1000000000)
            //throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "transaction already in block chain");
        // Not in block, but already in the memory pool; will drop
        // through to re-relay it.
    } else {
        SyncWithWallets(hashTx, *rawTx, NULL, true);
    }
    RelayTransaction(*rawTx, hashTx);
    clear();
}*/

void MultiSigDialog::sendRawTransaction()
{
    uint256 hashTx = rawTx->GetHash();

    CCoinsViewCache &view = *pcoinsTip;
    CCoins existingCoins;
    bool fHaveMempool = mempool.exists(hashTx);
    bool fHaveChain = view.GetCoins(hashTx, existingCoins) && existingCoins.nHeight < 1000000000;
    if (!fHaveMempool && !fHaveChain) {
        // push to local node and sync with wallets
        CValidationState state;
        if (AcceptToMemoryPool(mempool, state, *rawTx, false, NULL, false))
            SyncWithWallets(hashTx, *rawTx, NULL);
        else {
            if(state.IsInvalid())
                throw JSONRPCError(RPC_TRANSACTION_REJECTED, strprintf("%i: %s", state.GetRejectCode(), state.GetRejectReason()));
            else
                throw JSONRPCError(RPC_TRANSACTION_ERROR, state.GetRejectReason());
        }
    } else if (fHaveChain) {
        throw JSONRPCError(RPC_TRANSACTION_ALREADY_IN_CHAIN, "transaction already in block chain");
    }
    RelayTransaction(*rawTx, hashTx);
    
    clear();
}

void MultiSigDialog::signAddress0()
{
    QString addrStr = ui->labelRequireAddr0->text();
    signTransaction(&addrStr);
}

void MultiSigDialog::signAddress1()
{
    QString addrStr = ui->labelRequireAddr1->text();
    signTransaction(&addrStr);
}

void MultiSigDialog::signAddress2()
{
    QString addrStr = ui->labelRequireAddr2->text();
    signTransaction(&addrStr);
}

void MultiSigDialog::signTransaction(QString *addrStr)
{
//-----2014-06-11-------
    model->isMultiSig = true;
    WalletModel::UnlockContext ctx(model->requestUnlock());
    if(!ctx.isValid())
    {
        // Unlock wallet was cancelled
        fNewRecipientAllowed = true;
    }
//----------------------
    if ( !isTxCreate )
        createRawTransaction();

    // if created, sign
    if ( isTxCreate )
    {
        isComplete = true;
        // Fetch previous transactions (inputs):
        CCoinsView viewDummy;
        CCoinsViewCache view(viewDummy);
        {
            LOCK(mempool.cs);
            CCoinsViewCache &viewChain = *pcoinsTip;
            CCoinsViewMemPool viewMempool(viewChain, mempool);
            view.SetBackend(viewMempool); // temporarily switch cache backend to db+mempool view

            BOOST_FOREACH(const CTxIn& txin, rawTx->vin) {
                const uint256& prevHash = txin.prevout.hash;
                CCoins coins;
                view.GetCoins(prevHash, coins); // this is certainly allowed to fail
            }

            view.SetBackend(viewDummy); // switch back to avoid locking mempool for too long
        }

        bool fGivenKeys = false;
        CBasicKeyStore tempKeystore;
        if ( addrStr )
        {
            CBitcoinAddress address(addrStr->toStdString());
            CKeyID keyID;
            if (address.GetKeyID(keyID))
            {
                CKey vchSecret;
                if (pwalletMain->GetKey(keyID, vchSecret))
                {
                    fGivenKeys = true;
                    tempKeystore.AddKey(vchSecret);

                    QString s = ui->comboBoxAddrList->currentText();
                    CBitcoinAddress address(s.toStdString());
                    CScriptID scriptID;
                    address.GetScriptID(scriptID);
                    CScript redeemScript;
                    pwalletMain->GetCScript(scriptID, redeemScript);
                    tempKeystore.AddCScript(redeemScript);
                }
            }
        }
        
        const CKeyStore& keystore = ((fGivenKeys || !pwalletMain) ? tempKeystore : *pwalletMain);

        CTransaction txv(*rawTx);
        // Sign what we can:
        for (unsigned int i = 0; i < rawTx->vin.size(); i++)
        {
            CTxIn& txin = rawTx->vin[i];
            CCoins coins;
            if (!view.GetCoins(txin.prevout.hash, coins) || !coins.IsAvailable(txin.prevout.n))
            {
                isComplete = false;
                continue;
            }
            const CScript& prevPubKey = coins.vout[txin.prevout.n].scriptPubKey;

            txin.scriptSig.clear();
            SignSignature(keystore, prevPubKey, *rawTx, i, SIGHASH_ALL);
            txin.scriptSig = CombineSignatures(prevPubKey, *rawTx, i, txin.scriptSig, txv.vin[i].scriptSig);

            if (!VerifyScript(txin.scriptSig, prevPubKey, *rawTx, i, SCRIPT_VERIFY_P2SH | SCRIPT_VERIFY_STRICTENC, 0))
                isComplete = false;
        }
    
        editEnable(false);
        ui->sendButton->setEnabled(false);
    }

    if ( isComplete )
        ui->sendButton->setEnabled(true);

    checkRawTransaction();
}

void MultiSigDialog::on_sendButton_clicked()
{
    // if complete, send
    if ( isTxCreate && isComplete )
    {
        sendRawTransaction();
        return;
    }
}

void MultiSigDialog::editEnable(bool enable)
{
    for(int i = 0; i < ui->entries->count(); ++i)
    {
        SendCoinsEntry *entry = qobject_cast<SendCoinsEntry*>(ui->entries->itemAt(i)->widget());
        if(entry)
        {
            entry->setFieldEnable(enable);
        }
        ui->comboBoxAddrList->setEnabled(enable);
    }
}

void MultiSigDialog::clear()
{
    // Remove entries until only one left
    while(ui->entries->count())
    {
        ui->entries->takeAt(0)->widget()->deleteLater();
    }
    addEntry();

    updateRemoveEnabled();

    ui->sendButton->setDefault(true);
    isTxCreate = false;
    isComplete = false;
    ui->sendButton->setEnabled(false);
    ui->comboBoxAddrList->setEnabled(true);
    nCurrentIn = 0;
    checkRawTransaction();
}

void MultiSigDialog::reject()
{
    clear();
}

void MultiSigDialog::accept()
{
    clear();
}

SendCoinsEntry *MultiSigDialog::addEntry()
{
    SendCoinsEntry *entry = new SendCoinsEntry(this);
    entry->setModel(model);
    ui->entries->addWidget(entry);
    connect(entry, SIGNAL(removeEntry(SendCoinsEntry*)), this, SLOT(removeEntry(SendCoinsEntry*)));

    updateRemoveEnabled();

    // Focus the field, so that entry can start immediately
    entry->clear();
    entry->setFocus();
    ui->scrollAreaWidgetContents->resize(ui->scrollAreaWidgetContents->sizeHint());
    qApp->processEvents();
    QScrollBar* bar = ui->scrollArea->verticalScrollBar();
    if(bar)
        bar->setSliderPosition(bar->maximum());
    return entry;
}

void MultiSigDialog::updateRemoveEnabled()
{
    // Remove buttons are enabled as soon as there is more than one send-entry
    bool enabled = (ui->entries->count() > 1);
    for(int i = 0; i < ui->entries->count(); ++i)
    {
        SendCoinsEntry *entry = qobject_cast<SendCoinsEntry*>(ui->entries->itemAt(i)->widget());
        if(entry)
        {
            entry->setRemoveEnabled(enabled);
        }
    }
    setupTabChain(0);
}

void MultiSigDialog::removeEntry(SendCoinsEntry* entry)
{
    entry->deleteLater();
    updateRemoveEnabled();
}

QWidget *MultiSigDialog::setupTabChain(QWidget *prev)
{
    for(int i = 0; i < ui->entries->count(); ++i)
    {
        SendCoinsEntry *entry = qobject_cast<SendCoinsEntry*>(ui->entries->itemAt(i)->widget());
        if(entry)
        {
            prev = entry->setupTabChain(prev);
        }
    }
    QWidget::setTabOrder(prev, ui->addButton);
    QWidget::setTabOrder(ui->addButton, ui->sendButton);
    return ui->sendButton;
}

void MultiSigDialog::setAddress(const QString &address)
{
    SendCoinsEntry *entry = 0;
    // Replace the first entry if it is still unused
    if(ui->entries->count() == 1)
    {
        SendCoinsEntry *first = qobject_cast<SendCoinsEntry*>(ui->entries->itemAt(0)->widget());
        if(first->isClear())
        {
            entry = first;
        }
    }
    if(!entry)
    {
        entry = addEntry();
    }

    entry->setAddress(address);
}

void MultiSigDialog::pasteEntry(const SendCoinsRecipient &rv)
{
    if(!fNewRecipientAllowed)
        return;

    SendCoinsEntry *entry = 0;
    // Replace the first entry if it is still unused
    if(ui->entries->count() == 1)
    {
        SendCoinsEntry *first = qobject_cast<SendCoinsEntry*>(ui->entries->itemAt(0)->widget());
        if(first->isClear())
        {
            entry = first;
        }
    }
    if(!entry)
    {
        entry = addEntry();
    }

    entry->setValue(rv);
}

bool MultiSigDialog::handleURI(const QString &uri)
{
    SendCoinsRecipient rv;
    // URI has to be valid
    if (GUIUtil::parseBitcoinURI(uri, &rv))
    {
        CBitcoinAddress address(rv.address.toStdString());
        if (!address.IsValid())
            return false;
        pasteEntry(rv);
        return true;
    }

    return false;
}

void MultiSigDialog::setSharedBalance(qint64 balance, qint64 unconfirmedBalance, qint64 immatureBalance)
{
    //printf("setSharedBalance %s\n", BitcoinUnits::formatWithUnit(model->getOptionsModel()->getDisplayUnit(), balance).toStdString().c_str());
    Q_UNUSED(unconfirmedBalance);
    Q_UNUSED(immatureBalance);
    if(!model || !model->getOptionsModel())
        return;

    int unit = model->getOptionsModel()->getDisplayUnit();
    ui->labelBalance->setText(BitcoinUnits::formatWithUnit(unit, balance));
    updateAddressList();
    updateAddressBalance();
    checkRawTransaction();
}

void MultiSigDialog::updateDisplayUnit()
{
    if(model && model->getOptionsModel())
    {
        // Update labelBalance with the current balance and the current unit
        ui->labelBalance->setText(BitcoinUnits::formatWithUnit(model->getOptionsModel()->getDisplayUnit(), model->getSharedBalance()));
    }
}

void MultiSigDialog::updateAddressList()
{
    ui->comboBoxAddrList->clear();
    {
        LOCK(wallet->cs_wallet);
        //BOOST_FOREACH(const PAIRTYPE(CTxDestination, std::string)& item, wallet->mapAddressBook)
        BOOST_FOREACH(const PAIRTYPE(CTxDestination, CAddressBookData)& item, wallet->mapAddressBook)
        {
            const CBitcoinAddress& address = item.first;
            const std::string& strName = item.second.name;
            //bool fMine = IsMine(*wallet, address.Get());
            bool fMyShare = IsMyShare(*wallet, address.Get());
            if ( fMyShare )
            {
                ui->comboBoxAddrList->addItem(QString::fromStdString(address.ToString()), QVariant(QString::fromStdString(address.ToString())));
            }
        }
    }

    int n = ui->comboBoxAddrList->count();
    QString num_str = QString::number(n);
    ui->labelAddressesNum->setText(num_str);
}

void MultiSigDialog::updateAddressBalance()
{
    if ( currentIndex < 0 )
        return;

    QString s = ui->comboBoxAddrList->currentText();
    CBitcoinAddress address(s.toStdString());

    coinControl->SetNull();
    coinControl->destChange = address.Get();
    
    CScript scriptPubKey;
    scriptPubKey.SetDestination(address.Get());

    int64 nAmount = 0;
    {
        LOCK(pwalletMain->cs_wallet);
        for (std::map<uint256, CWalletTx>::iterator it = pwalletMain->mapWallet.begin(); it != pwalletMain->mapWallet.end(); ++it)
        {
            const CWalletTx& wtx = (*it).second;
            //if (wtx.IsCoinBase() || !wtx.IsFinal())
                //continue;

            //printf("updateAddressBalance wtx %s \n", wtx.GetHash().ToString().c_str());
            for (unsigned int i = 0; i < wtx.vout.size(); i++)
            {
                const CTxOut* txout = &wtx.vout[i];
                if ( scriptPubKey == txout->scriptPubKey && !wtx.IsSpent(i)
                    ){
                    //printf("updateAddressBalance GetDepthInMainChain %d\n", wtx.GetDepthInMainChain());
                    //printf("updateAddressBalance nValue %s\n", BitcoinUnits::formatWithUnit(model->getOptionsModel()->getDisplayUnit(), txout.nValue).toStdString().c_str());
                    //if (wtx.GetDepthInMainChain() > 0)
                    //if (wtx.IsConfirmed())
                    nAmount += txout->nValue;
                    COutPoint outpt(wtx.GetHash(), i);
                    coinControl->Select(outpt);
                }
            }
        }
    }

    //printf("updateAddressBalance nAmount %s\n", BitcoinUnits::formatWithUnit(model->getOptionsModel()->getDisplayUnit(), nAmount).toStdString().c_str());
    if(model && model->getOptionsModel())
    {
        ui->labelAvailableCoins->setText(BitcoinUnits::formatWithUnit(model->getOptionsModel()->getDisplayUnit(), nAmount));
    }
}

void MultiSigDialog::updateAddressDetail()
{
    QString s = ui->comboBoxAddrList->currentText();
    CBitcoinAddress address(s.toStdString());
    CScript scriptPubKey;
    scriptPubKey.SetDestination(address.Get());

    CScript subscript;
    CScriptID scriptID;
    address.GetScriptID(scriptID);
    pwalletMain->GetCScript(scriptID, subscript);
    std::vector<CTxDestination> addresses;
    txnouttype whichType;
    int nRequired;
    ExtractDestinations(subscript, whichType, addresses, nRequired);

    containAddresses.clear();
    int i;
    for ( i = 0; i < addresses.size(); ++ i )
        containAddresses.push_back(CBitcoinAddress(addresses[i]).ToString());

    ui->labelRequireAddr2->setVisible(false);
    for ( i = 0; i < addresses.size(); ++ i )
    {
        if ( i == 0 )
            ui->labelRequireAddr0->setText(QString::fromStdString(CBitcoinAddress(addresses[i]).ToString()));
        else if ( i == 1 )
            ui->labelRequireAddr1->setText(QString::fromStdString(CBitcoinAddress(addresses[i]).ToString()));
        else if ( i == 2 )
        {
            ui->labelRequireAddr2->setVisible(true);
            ui->labelRequireAddr2->setText(QString::fromStdString(CBitcoinAddress(addresses[i]).ToString()));
        }
    }
    
    ui->labelRequire->setText(QString(tr("Require ")) + QString::number(nRequired) + QString(tr(" of ")) + QString::number(i) + QString(tr(" signatures ")) );
    checkRawTransaction();
}

void MultiSigDialog::checkRawTransaction()
{
    bool IsSign[3] = {0};
    if ( isTxCreate )
    {
        // Fetch previous transactions (inputs):
        CCoinsView viewDummy;
        CCoinsViewCache view(viewDummy);
        {
            LOCK(mempool.cs);
            CCoinsViewCache &viewChain = *pcoinsTip;
            CCoinsViewMemPool viewMempool(viewChain, mempool);
            view.SetBackend(viewMempool); // temporarily switch cache backend to db+mempool view

            BOOST_FOREACH(const CTxIn& txin, rawTx->vin) {
                const uint256& prevHash = txin.prevout.hash;
                CCoins coins;
                view.GetCoins(prevHash, coins); // this is certainly allowed to fail
            }

            view.SetBackend(viewDummy); // switch back to avoid locking mempool for too long
        }

        CTransaction txv(*rawTx);
        // Sign what we can:
        CTxIn& txin = rawTx->vin[nCurrentIn];
        CCoins coins;
        if (view.GetCoins(txin.prevout.hash, coins))
        {
            const CScript& prevPubKey = coins.vout[txin.prevout.n].scriptPubKey;
            isComplete = VerifyMultiSigScript(txin.scriptSig, prevPubKey, *rawTx, nCurrentIn, 0, IsSign);
        }
        if ( isComplete )
            ui->sendButton->setEnabled(true);
    }

    QLineEdit *labelRequireAddr[3];
    QToolButton *btnSign[3];
    QLabel *labelIsSign[3];

    labelRequireAddr[0] = ui->labelRequireAddr0;labelRequireAddr[1] = ui->labelRequireAddr1;labelRequireAddr[2] = ui->labelRequireAddr2;
    btnSign[0] = ui->btnSign0;btnSign[1] = ui->btnSign1;btnSign[2] = ui->btnSign2;
    labelIsSign[0] = ui->labelIsSign0;labelIsSign[1] = ui->labelIsSign1;labelIsSign[2] = ui->labelIsSign2;

    for ( int i = 0; i < 3 ; i ++ )
    {
        btnSign[i]->setVisible(false);
        labelIsSign[i]->setVisible(false);
    }
    for ( int i = 0; i < containAddresses.size(); ++ i )
    {
        CBitcoinAddress addr(containAddresses[i]);
        labelRequireAddr[i]->setText(QString::fromStdString(CBitcoinAddress(addr).ToString()));
        
        if ( IsMine(*pwalletMain, addr.Get()) )
        {
            if ( !IsSign[i] )
            {
                btnSign[i]->setVisible(true);
                labelIsSign[i]->setVisible(false);
            }
            else
            {
                btnSign[i]->setVisible(false);
                labelIsSign[i]->setVisible(true);
                labelIsSign[i]->setStyleSheet("color:green");
                labelIsSign[i]->setText(tr("Signed"));
            }
        }
        else
        {
            btnSign[i]->setVisible(false);
            labelIsSign[i]->setVisible(true);
            if ( isTxCreate && IsSign[i])
            {
                labelIsSign[i]->setStyleSheet("color:green");
                labelIsSign[i]->setText(tr("Signed"));
            }
            else if ( isTxCreate && isComplete )
                labelIsSign[i]->setText("");
            else
            {
                labelIsSign[i]->setStyleSheet("color:red");
                labelIsSign[i]->setText(tr("Not in local wallet"));
            }
        }
    }
    if (model->was_locked)//Added by Jason 2014-06-11
    {
        model->setWalletLocked(true);
    }
}

void MultiSigDialog::handleAddrSelectionChanged(int idx)
{
    if ( currentIndex != idx )
    {
        currentIndex = idx;
        updateAddressBalance();
        updateAddressDetail();
    }
}

void MultiSigDialog::handleAddressBookChanged()
{
    clear();
    updateAddressList();
    updateAddressBalance();
    updateAddressDetail();
}

