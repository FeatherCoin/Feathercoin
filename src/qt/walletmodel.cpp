// Copyright (c) 2011-2014 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "walletmodel.h"

#include "addresstablemodel.h"
#include "guiconstants.h"
#include "guiutil.h"
#include "paymentserver.h"
#include "recentrequeststablemodel.h"
#include "transactiontablemodel.h"

#include "base58.h"
#include "keystore.h"
#include "main.h"
#include "sync.h"
#include "ui_interface.h"
#include "wallet/wallet.h"
#include "wallet/walletdb.h" // for BackupWallet
#include "core_io.h"

#include <stdint.h>

#include <QDebug>
#include <QSet>
#include <QTimer>

#if defined(_MSC_VER) || defined(__MSVCRT__)
  #define PRIszx    "Ix"
  #define PRIszu    "Iu"
  #define PRIszd    "Id"
  #define PRIpdx    "Ix"
  #define PRIpdu    "Iu"
  #define PRIpdd    "Id"
#else /* C99 standard */
  #define PRIszx    "zx"
  #define PRIszu    "zu"
  #define PRIszd    "zd"
  #define PRIpdx    "tx"
  #define PRIpdu    "tu"
  #define PRIpdd    "td"
#endif

using namespace std;

WalletModel::WalletModel(CWallet *wallet, OptionsModel *optionsModel, QObject *parent) :
    QObject(parent), wallet(wallet), optionsModel(optionsModel), addressTableModel(0),
    transactionTableModel(0),
    recentRequestsTableModel(0),
    cachedBalance(0), cachedUnconfirmedBalance(0), cachedImmatureBalance(0),
    cachedEncryptionStatus(Unencrypted),
    cachedNumBlocks(0)
{
    fHaveWatchOnly = wallet->HaveWatchOnly();
    fForceCheckBalanceChanged = false;

    addressTableModel = new AddressTableModel(wallet, this);
    transactionTableModel = new TransactionTableModel(wallet, this);
    recentRequestsTableModel = new RecentRequestsTableModel(wallet, this);

    // This timer will be fired repeatedly to update the balance
    pollTimer = new QTimer(this);
    connect(pollTimer, SIGNAL(timeout()), this, SLOT(pollBalanceChanged()));
    pollTimer->start(MODEL_UPDATE_DELAY);

    subscribeToCoreSignals();
}

WalletModel::~WalletModel()
{
    unsubscribeFromCoreSignals();
}

CAmount WalletModel::getBalance(const CCoinControl *coinControl) const
{
    if (coinControl)
    {
        CAmount nBalance = 0;
        std::vector<COutput> vCoins;
        wallet->AvailableCoins(vCoins, true, coinControl);
        BOOST_FOREACH(const COutput& out, vCoins)
            if(out.fSpendable)
                nBalance += out.tx->vout[out.i].nValue;

        return nBalance;
    }

    return wallet->GetBalance();
}

CAmount WalletModel::getUnconfirmedBalance() const
{
    return wallet->GetUnconfirmedBalance();
}

CAmount WalletModel::getImmatureBalance() const
{
    return wallet->GetImmatureBalance();
}

bool WalletModel::haveWatchOnly() const
{
    return fHaveWatchOnly;
}

CAmount WalletModel::getWatchBalance() const
{
    return wallet->GetWatchOnlyBalance();
}

CAmount WalletModel::getWatchUnconfirmedBalance() const
{
    return wallet->GetUnconfirmedWatchOnlyBalance();
}

CAmount WalletModel::getWatchImmatureBalance() const
{
    return wallet->GetImmatureWatchOnlyBalance();
}

void WalletModel::updateStatus()
{
    EncryptionStatus newEncryptionStatus = getEncryptionStatus();

    if(cachedEncryptionStatus != newEncryptionStatus)
        Q_EMIT encryptionStatusChanged(newEncryptionStatus);
}

void WalletModel::pollBalanceChanged()
{
    // Get required locks upfront. This avoids the GUI from getting stuck on
    // periodical polls if the core is holding the locks for a longer time -
    // for example, during a wallet rescan.
    TRY_LOCK(cs_main, lockMain);
    if(!lockMain)
        return;
    TRY_LOCK(wallet->cs_wallet, lockWallet);
    if(!lockWallet)
        return;

    if(fForceCheckBalanceChanged || chainActive.Height() != cachedNumBlocks)
    {
        fForceCheckBalanceChanged = false;

        // Balance and number of transactions might have changed
        cachedNumBlocks = chainActive.Height();

        checkBalanceChanged();
        if(transactionTableModel)
            transactionTableModel->updateConfirmations();
    }
}

void WalletModel::checkBalanceChanged()
{
    CAmount newBalance = getBalance();
    CAmount newUnconfirmedBalance = getUnconfirmedBalance();
    CAmount newImmatureBalance = getImmatureBalance();
    CAmount newWatchOnlyBalance = 0;
    CAmount newWatchUnconfBalance = 0;
    CAmount newWatchImmatureBalance = 0;
    if (haveWatchOnly())
    {
        newWatchOnlyBalance = getWatchBalance();
        newWatchUnconfBalance = getWatchUnconfirmedBalance();
        newWatchImmatureBalance = getWatchImmatureBalance();
    }

    if(cachedBalance != newBalance || cachedUnconfirmedBalance != newUnconfirmedBalance || cachedImmatureBalance != newImmatureBalance ||
        cachedWatchOnlyBalance != newWatchOnlyBalance || cachedWatchUnconfBalance != newWatchUnconfBalance || cachedWatchImmatureBalance != newWatchImmatureBalance)
    {
        cachedBalance = newBalance;
        cachedUnconfirmedBalance = newUnconfirmedBalance;
        cachedImmatureBalance = newImmatureBalance;
        cachedWatchOnlyBalance = newWatchOnlyBalance;
        cachedWatchUnconfBalance = newWatchUnconfBalance;
        cachedWatchImmatureBalance = newWatchImmatureBalance;
        Q_EMIT balanceChanged(newBalance, newUnconfirmedBalance, newImmatureBalance,
                            newWatchOnlyBalance, newWatchUnconfBalance, newWatchImmatureBalance);
    }
}

void WalletModel::updateTransaction()
{
    // Balance and number of transactions might have changed
    fForceCheckBalanceChanged = true;
}

void WalletModel::updateAddressBook(const QString &address, const QString &label,
        bool isMine, const QString &purpose, int status)
{
    if(addressTableModel)
        addressTableModel->updateEntry(address, label, isMine, purpose, status);
}

void WalletModel::updateWatchOnlyFlag(bool fHaveWatchonly)
{
    fHaveWatchOnly = fHaveWatchonly;
    Q_EMIT notifyWatchonlyChanged(fHaveWatchonly);
}

bool WalletModel::validateAddress(const QString &address)
{
    std::string sAddr = address.toStdString();    
    if (sAddr.length() > 75)
    {
        if (IsStealthAddress(sAddr))
            return true;
    }
    
    CBitcoinAddress addressParsed(address.toStdString());
    return addressParsed.IsValid();
}

WalletModel::SendCoinsReturn WalletModel::prepareTransaction(WalletModelTransaction &transaction, const CCoinControl *coinControl, int nHashType)
{
    CAmount total = 0;
    bool fSubtractFeeFromAmount = false;
    QList<SendCoinsRecipient> recipients = transaction.getRecipients();
    std::vector<CRecipient> vecSend;

    if(recipients.empty())
    {
        return OK;
    }

    QSet<QString> setAddress; // Used to detect duplicates
    int nAddresses = 0;

    // Pre-check input data for validity
    Q_FOREACH(const SendCoinsRecipient &rcp, recipients)
    {
        if (rcp.fSubtractFeeFromAmount)
            fSubtractFeeFromAmount = true;

        if (rcp.paymentRequest.IsInitialized())
        {   // PaymentRequest...
            CAmount subtotal = 0;
            const payments::PaymentDetails& details = rcp.paymentRequest.getDetails();
            for (int i = 0; i < details.outputs_size(); i++)
            {
                const payments::Output& out = details.outputs(i);
                if (out.amount() <= 0) continue;
                subtotal += out.amount();
                const unsigned char* scriptStr = (const unsigned char*)out.script().data();
                CScript scriptPubKey(scriptStr, scriptStr+out.script().size());
                CAmount nAmount = out.amount();
                CRecipient recipient = {scriptPubKey, nAmount, rcp.fSubtractFeeFromAmount};
                vecSend.push_back(recipient);
            }
            if (subtotal <= 0)
            {
                return InvalidAmount;
            }
            total += subtotal;
        }
        else
        {   // User-entered feathercoin address / amount:
            if(!validateAddress(rcp.address))
            {
                return InvalidAddress;
            }
            if(rcp.amount <= 0)
            {
                return InvalidAmount;
            }
            setAddress.insert(rcp.address);
            ++nAddresses;

						std::string sAddr = rcp.address.toStdString();            
            if (rcp.typeInd == AddressTableModel::AT_Stealth)
            {
                CStealthAddress sxAddr;
                if (sxAddr.SetEncoded(sAddr))
                {
                    ec_secret ephem_secret;
                    ec_secret secretShared;
                    ec_point pkSendTo;
                    ec_point ephem_pubkey;
                    
                    
                    if (GenerateRandomSecret(ephem_secret) != 0)
                    {
                        printf("GenerateRandomSecret failed.\n");
                        return Aborted;
                    };
                    
                    LogPrintf("StealthSecret send start....\n");
                    if (true)
                    {
			                LogPrintf("ephem_secret.e=%s \n",HexStr(&ephem_secret.e[0],&ephem_secret.e[32]).c_str());   //secret
			                LogPrintf("sxAddr.scan_secret= %s\n", HexStr(sxAddr.scan_secret).c_str()); //
			                LogPrintf("sxAddr.scan_pubkey= %s\n", HexStr(sxAddr.scan_pubkey).c_str()); //pubkey[0]
			                LogPrintf("sxAddr.spend_secret= %s\n", HexStr(sxAddr.spend_secret).c_str()); //
			                LogPrintf("sxAddr.spend_pubkey= %s\n",HexStr(sxAddr.spend_pubkey).c_str()); //pkSpend[0]
			                LogPrintf("secretShared.e=%s \n",HexStr(&secretShared.e[0],&secretShared.e[32]).c_str());  //sharedSOut
			                LogPrintf("pkSendTo= %"PRIszu": %s\n", pkSendTo.size(), HexStr(pkSendTo).c_str());//pkOut
			                LogPrintf("send:  secret = ephem_secret, pubkey = scan_pubkey....\n");
                    };
                    
                    if (StealthSecret(ephem_secret, sxAddr.scan_pubkey, sxAddr.spend_pubkey, secretShared, pkSendTo) != 0)
                    {
                        printf("Could not generate receiving public key.\n");
                        return Aborted;
                    };
                    if (true)
                    {
			                LogPrintf("ephem_secret.e=%s \n",HexStr(&ephem_secret.e[0],&ephem_secret.e[32]).c_str());   //secret
			                LogPrintf("sxAddr.scan_secret= %s\n", HexStr(sxAddr.scan_secret).c_str()); //
			                LogPrintf("sxAddr.scan_pubkey= %s\n", HexStr(sxAddr.scan_pubkey).c_str()); //pubkey[0]
			                LogPrintf("sxAddr.spend_secret= %s\n", HexStr(sxAddr.spend_secret).c_str()); //
			                LogPrintf("sxAddr.spend_pubkey= %s\n",HexStr(sxAddr.spend_pubkey).c_str()); //pkSpend[0]
			                LogPrintf("secretShared.e=%s \n",HexStr(&secretShared.e[0],&secretShared.e[32]).c_str());  //sharedSOut
			                LogPrintf("pkSendTo= %"PRIszu": %s\n", pkSendTo.size(), HexStr(pkSendTo).c_str());//pkOut
                    };
                    
                    CPubKey cpkTo(pkSendTo);
                    if (!cpkTo.IsValid())
                    {
                        printf("Invalid public key generated.\n");
                        return Aborted;
                    };
                    LogPrintf("CPubKey(pkSendTo)=%s \n",cpkTo.GetHash().ToString().c_str());
                    
                    CKeyID ckidTo = cpkTo.GetID();                    
                    CBitcoinAddress addrTo(ckidTo);
                    
                    if (SecretToPublicKey(ephem_secret, ephem_pubkey) != 0)
                    {
                        printf("Could not generate ephem public key.\n");
                        return Aborted;
                    };
                    
                    if (true)
                    {
                        LogPrintf("Stealth send to generated pubkey,pkSendTo= %"PRIszu": %s\n", pkSendTo.size(), HexStr(pkSendTo).c_str());
                        LogPrintf("hash, Address= %s\n", addrTo.ToString().c_str());
                        LogPrintf("enerate ephem public key,ephem_pubkey= %"PRIszu": %s\n", ephem_pubkey.size(), HexStr(ephem_pubkey).c_str());
                    };
                    
                    CScript scriptPubKey = GetScriptForDestination(addrTo.Get());
                    LogPrintf("scriptPubKey= %s \n", scriptPubKey.ToString());
                    CRecipient recipient = {scriptPubKey, rcp.amount, rcp.fSubtractFeeFromAmount};
                    vecSend.push_back(recipient);
                    
                    CScript scriptP = CScript() << OP_RETURN << ephem_pubkey;
                    recipient = {scriptP, CENT, true };
                    vecSend.push_back(recipient);
                    
                    continue;
                }; // else drop through to normal
            }
            
            LogPrintf("rcp.isSmart=%i\n", rcp.isSmart);
            LogPrintf("rcp.heightSmart=%i\n", rcp.heightSmart);
            
            CScript scriptPubKey = GetScriptForDestination(CBitcoinAddress(sAddr).Get());
						CRecipient recipient ;
						if (rcp.heightSmart ==0) {
								recipient = {scriptPubKey, rcp.amount, rcp.fSubtractFeeFromAmount};
						} else {
								recipient = {scriptPubKey, rcp.amount, rcp.fSubtractFeeFromAmount, rcp.heightSmart};
						}
            vecSend.push_back(recipient);
            
            //insert message into blockchain
						if ((rcp.message.length()>=1)&&(rcp.label=="comment"))
						{				
								std::string strMess = rcp.message.toStdString();
								const char* pszMess =strMess.c_str();
                CScript scriptP = CScript() << OP_RETURN << vector<unsigned char>((const unsigned char*)pszMess, (const unsigned char*)pszMess + strlen(pszMess));                
                LogPrintf("blockchain scriptP=%s\n",scriptP.ToString().c_str());
                recipient = {scriptP, CENT, true };
                vecSend.push_back(recipient);
             }
            
            //commit openname
						if ((rcp.message.length()>=1)&&(rcp.label=="openname"))
						{		
								std::string strMess = rcp.message.toStdString();//\D2Ѿ\AD\CA\C716\BD\F8\D6\C6\C2\EB\C1\F7
                CScript scriptP = CScript() << OP_RETURN << ParseHex(strMess);
                LogPrintf("openname scriptP=%s\n",scriptP.ToString().c_str());//scriptP=OP_RETURN 580861e00414720684e88cb7943fc6751527a94b2e0cdd2a9148d8b13939723d2aca16c75c6d68
               	recipient = {scriptP, CENT, true };
               	vecSend.push_back(recipient);
             }
            
            total += rcp.amount;
        }
    }
    if(setAddress.size() != nAddresses)
    {
        return DuplicateAddress;
    }

    CAmount nBalance = getBalance(coinControl);

		//\BA\CFԼ\BD\BB\D2ײ\BB\D1\E9֤\CA\E4\B3\F6\A3\ACnHashType = SIGHASH_ALL|SIGHASH_ANYONECANPAY
    if((total > nBalance) && (nHashType != int(SIGHASH_ALL|SIGHASH_ANYONECANPAY)))
    {
    		LogPrintf("prepareTransaction,total=%d,nBalance=%d,nHashType=%i\n",total, nBalance, nHashType);
        return AmountExceedsBalance;
    }

    {
        LOCK2(cs_main, wallet->cs_wallet);

        transaction.newPossibleKeyChange(wallet);

        CAmount nFeeRequired = 0;
        int nChangePosRet = -1;
        std::string strFailReason;

        CWalletTx *newTx = transaction.getTransaction();
        CReserveKey *keyChange = transaction.getPossibleKeyChange();
        bool fCreated = wallet->CreateTransaction(vecSend, *newTx, *keyChange, nFeeRequired, nChangePosRet, strFailReason, coinControl, nHashType);
        //\B4\CBʱ\BD\BB\D2\D7\D2Ѿ\AD\CC\EEд\CA\E4\C8롢\CA\E4\B3\F6\BA\CDǩ\C3\FB
        LogPrintf("prepareTransaction,fCreated=%i\n",fCreated);
        
        transaction.setTransactionFee(nFeeRequired);
        if (fSubtractFeeFromAmount && fCreated)
            transaction.reassignAmounts(nChangePosRet);

        if(!fCreated)
        {
            if(!fSubtractFeeFromAmount && (total + nFeeRequired) > nBalance)
            {
                return SendCoinsReturn(AmountWithFeeExceedsBalance);
            }
            Q_EMIT message(tr("Send Coins"), QString::fromStdString(strFailReason),
                         CClientUIInterface::MSG_ERROR);
            return TransactionCreationFailed;
        }

        // reject absurdly high fee > 0.1 feathercoin
        if (nFeeRequired > 10000000)
            return AbsurdFee;
    }

    return SendCoinsReturn(OK);
}

WalletModel::SendCoinsReturn WalletModel::sendCoins(WalletModelTransaction &transaction,bool fSend)
{
    QByteArray transaction_array; /* store serialized transaction */

    {
        LOCK2(cs_main, wallet->cs_wallet);
        CWalletTx *newTx = transaction.getTransaction();

        Q_FOREACH(const SendCoinsRecipient &rcp, transaction.getRecipients())
        {
            if (rcp.paymentRequest.IsInitialized())
            {
                // Make sure any payment requests involved are still valid.
                if (PaymentServer::verifyExpired(rcp.paymentRequest.getDetails())) {
                    return PaymentRequestExpired;
                }

                // Store PaymentRequests in wtx.vOrderForm in wallet.
                std::string key("PaymentRequest");
                std::string value;
                rcp.paymentRequest.SerializeToString(&value);
                newTx->vOrderForm.push_back(make_pair(key, value));
            }
            else if (!rcp.message.isEmpty()) // Message from normal feathercoin:URI (feathercoin:123...?message=example)
                newTx->vOrderForm.push_back(make_pair("Message", rcp.message.toStdString()));
        }

        CReserveKey *keyChange = transaction.getPossibleKeyChange();
        //\B9㲥\BD\BB\D2\D7
        if(!wallet->CommitTransaction(*newTx, *keyChange))
            return TransactionCommitFailed;

        CTransaction* t = (CTransaction*)newTx;
        CDataStream ssTx(SER_NETWORK, PROTOCOL_VERSION);
        ssTx << *t;
        transaction_array.append(&(ssTx[0]), ssTx.size());
    }

    // Add addresses / update labels that we've sent to to the address book,
    // and emit coinsSent signal for each recipient
    Q_FOREACH(const SendCoinsRecipient &rcp, transaction.getRecipients())
    {
        // Don't touch the address book when we have a payment request
        if (!rcp.paymentRequest.IsInitialized())
        {
            std::string strAddress = rcp.address.toStdString();
            CTxDestination dest = CBitcoinAddress(strAddress).Get();
            std::string strLabel = rcp.label.toStdString();
            {
                LOCK(wallet->cs_wallet);

                if (rcp.typeInd == AddressTableModel::AT_Stealth)
		            {
		                wallet->UpdateStealthAddress(strAddress, strLabel, true);
		            } else 
		            {
		                std::map<CTxDestination, CAddressBookData>::iterator mi = wallet->mapAddressBook.find(dest);
		                // Check if we have a new address or an updated label
		                if (mi == wallet->mapAddressBook.end())
		                {
		                    wallet->SetAddressBook(dest, strLabel, "send");
		                }
		                else if (mi->second.name != strLabel)
		                {
		                    wallet->SetAddressBook(dest, strLabel, ""); // "" means don't change purpose
		                }
		            }
            }
        }
        Q_EMIT coinsSent(wallet, rcp, transaction_array);
    }
   
    checkBalanceChanged(); // update balance immediately, otherwise there could be a short noticeable delay until pollBalanceChanged hits

    return SendCoinsReturn(OK);
}

QString WalletModel::hashCoins(WalletModelTransaction &transaction,bool fSend)
{
    //\BB\F1ȡ\BD\BB\D2\D7hash
    QString strHash;
    
    {
        LOCK2(cs_main, wallet->cs_wallet);
        CWalletTx *newTx = transaction.getTransaction();
        uint256 hashTx = newTx->GetHash();
        strHash = QString::fromStdString(hashTx.ToString());
    }
    
    return strHash;
}

QString WalletModel::codeCoins(WalletModelTransaction &transaction,bool fSend)
{
    //\BB\F1ȡ\BD\BB\D2׵Ķ\FE\BD\F8\D6\C6\C2\EB
    QString strCode;
    
    {
        LOCK2(cs_main, wallet->cs_wallet);
        CWalletTx *newTx = transaction.getTransaction();
        uint256 hashTx = newTx->GetHash();
        
        //1.\B4\B4\BD\A8\BD\BB\D2\D7
        CMutableTransaction rawTx;        
        BOOST_FOREACH(const CTxIn& txin, newTx->vin)
        {
        	rawTx.vin.push_back(txin);
        }
        BOOST_FOREACH(const CTxOut& txout, newTx->vout)
        {
        	rawTx.vout.push_back(txout);
        }
        rawTx.nVersion = newTx->nVersion;
        rawTx.nLockTime = newTx->nLockTime;
        std::string strHex = EncodeHexTx(rawTx);
        	
        //2.\BD\BB\D2\D7ǩ\C3\FB\A3\AC\D2Ѿ\ADǩ\D4\DAvin\A3\AC\CA\C7\D5\FB\B5\A5ǩ\C3\FB
				vector<unsigned char> txData(ParseHex(strHex));
        
        
        //3.\D7\EE\D6յĶ\FE\BD\F8\D6\C6\C2\EB
        strCode = QString::fromStdString(strHex);
    }
    
    return strCode;
}

OptionsModel *WalletModel::getOptionsModel()
{
    return optionsModel;
}

AddressTableModel *WalletModel::getAddressTableModel()
{
    return addressTableModel;
}

TransactionTableModel *WalletModel::getTransactionTableModel()
{
    return transactionTableModel;
}

RecentRequestsTableModel *WalletModel::getRecentRequestsTableModel()
{
    return recentRequestsTableModel;
}

WalletModel::EncryptionStatus WalletModel::getEncryptionStatus() const
{
    if(!wallet->IsCrypted())
    {
        return Unencrypted;
    }
    else if(wallet->IsLocked())
    {
        return Locked;
    }
    else
    {
        return Unlocked;
    }
}

bool WalletModel::setWalletEncrypted(bool encrypted, const SecureString &passphrase)
{
    if(encrypted)
    {
        // Encrypt
        return wallet->EncryptWallet(passphrase);
    }
    else
    {
        // Decrypt -- TODO; not supported yet
        return false;
    }
}

bool WalletModel::setWalletLocked(bool locked, const SecureString &passPhrase)
{
    if(locked)
    {
        // Lock
        return wallet->Lock();
    }
    else
    {
        // Unlock
        return wallet->Unlock(passPhrase);
    }
}

bool WalletModel::changePassphrase(const SecureString &oldPass, const SecureString &newPass)
{
    bool retval;
    {
        LOCK(wallet->cs_wallet);
        wallet->Lock(); // Make sure wallet is locked before attempting pass change
        retval = wallet->ChangeWalletPassphrase(oldPass, newPass);
    }
    return retval;
}

bool WalletModel::backupWallet(const QString &filename)
{
    return BackupWallet(*wallet, filename.toLocal8Bit().data());
}

// Handlers for core signals
static void NotifyKeyStoreStatusChanged(WalletModel *walletmodel, CCryptoKeyStore *wallet)
{
    qDebug() << "NotifyKeyStoreStatusChanged";
    QMetaObject::invokeMethod(walletmodel, "updateStatus", Qt::QueuedConnection);
}

static void NotifyAddressBookChanged(WalletModel *walletmodel, CWallet *wallet,
        const CTxDestination &address, const std::string &label, bool isMine,
        const std::string &purpose, ChangeType status)
{
    QString strAddress = QString::fromStdString(CBitcoinAddress(address).ToString());
    QString strLabel = QString::fromStdString(label);
    QString strPurpose = QString::fromStdString(purpose);

    /*qDebug() << "NotifyAddressBookChanged: " + strAddress + " " + strLabel + " isMine=" + QString::number(isMine) + " purpose=" + strPurpose + " status=" + QString::number(status);
    QMetaObject::invokeMethod(walletmodel, "updateAddressBook", Qt::QueuedConnection,
                              Q_ARG(QString, strAddress),
                              Q_ARG(QString, strLabel),
                              Q_ARG(bool, isMine),
                              Q_ARG(QString, strPurpose),
                              Q_ARG(int, status));*/
                              
    if (address.type() == typeid(CStealthAddress))
    {
        CStealthAddress sxAddr = boost::get<CStealthAddress>(address);
        std::string enc = sxAddr.Encoded();
        qDebug() << "NotifyAddressBookChanged : " + QString::fromStdString(enc) + " " + QString::fromStdString(label) + " isMine=" + QString::number(isMine) + " purpose=" + strPurpose + " status=" + QString::number(status);
        QMetaObject::invokeMethod(walletmodel, "updateAddressBook", Qt::QueuedConnection,
                                  Q_ARG(QString, QString::fromStdString(enc)),
                                  Q_ARG(QString, QString::fromStdString(label)),
                                  Q_ARG(bool, isMine),
                                  Q_ARG(QString, strPurpose),
                                  Q_ARG(int, status));
    } else {
        qDebug() << "NotifyAddressBookChanged : " + strAddress + " " + strLabel + " isMine=" + QString::number(isMine) + " purpose=" + strPurpose + " status=" + QString::number(status);
        QMetaObject::invokeMethod(walletmodel, "updateAddressBook", Qt::QueuedConnection,
                                  Q_ARG(QString, QString::fromStdString(CBitcoinAddress(address).ToString())),
                                  Q_ARG(QString, QString::fromStdString(label)),
                                  Q_ARG(bool, isMine),
                                  Q_ARG(QString, strPurpose),
                                  Q_ARG(int, status));
    }
}

static void NotifyTransactionChanged(WalletModel *walletmodel, CWallet *wallet, const uint256 &hash, ChangeType status)
{
    Q_UNUSED(wallet);
    Q_UNUSED(hash);
    Q_UNUSED(status);
    QMetaObject::invokeMethod(walletmodel, "updateTransaction", Qt::QueuedConnection);
}

static void ShowProgress(WalletModel *walletmodel, const std::string &title, int nProgress)
{
    // emits signal "showProgress"
    QMetaObject::invokeMethod(walletmodel, "showProgress", Qt::QueuedConnection,
                              Q_ARG(QString, QString::fromStdString(title)),
                              Q_ARG(int, nProgress));
}

static void NotifyWatchonlyChanged(WalletModel *walletmodel, bool fHaveWatchonly)
{
    QMetaObject::invokeMethod(walletmodel, "updateWatchOnlyFlag", Qt::QueuedConnection,
                              Q_ARG(bool, fHaveWatchonly));
}

void WalletModel::subscribeToCoreSignals()
{
    // Connect signals to wallet
    wallet->NotifyStatusChanged.connect(boost::bind(&NotifyKeyStoreStatusChanged, this, _1));
    wallet->NotifyAddressBookChanged.connect(boost::bind(NotifyAddressBookChanged, this, _1, _2, _3, _4, _5, _6));
    wallet->NotifyTransactionChanged.connect(boost::bind(NotifyTransactionChanged, this, _1, _2, _3));
    wallet->ShowProgress.connect(boost::bind(ShowProgress, this, _1, _2));
    wallet->NotifyWatchonlyChanged.connect(boost::bind(NotifyWatchonlyChanged, this, _1));
}

void WalletModel::unsubscribeFromCoreSignals()
{
    // Disconnect signals from wallet
    wallet->NotifyStatusChanged.disconnect(boost::bind(&NotifyKeyStoreStatusChanged, this, _1));
    wallet->NotifyAddressBookChanged.disconnect(boost::bind(NotifyAddressBookChanged, this, _1, _2, _3, _4, _5, _6));
    wallet->NotifyTransactionChanged.disconnect(boost::bind(NotifyTransactionChanged, this, _1, _2, _3));
    wallet->ShowProgress.disconnect(boost::bind(ShowProgress, this, _1, _2));
    wallet->NotifyWatchonlyChanged.disconnect(boost::bind(NotifyWatchonlyChanged, this, _1));
}

// WalletModel::UnlockContext implementation
WalletModel::UnlockContext WalletModel::requestUnlock()
{
    bool was_locked = getEncryptionStatus() == Locked;
    if(was_locked)
    {
        // Request UI to unlock wallet
        Q_EMIT requireUnlock();
    }
    // If wallet is still locked, unlock was failed or cancelled, mark context as invalid
    bool valid = getEncryptionStatus() != Locked;

    return UnlockContext(this, valid, was_locked);
}

WalletModel::UnlockContext::UnlockContext(WalletModel *wallet, bool valid, bool relock):
        wallet(wallet),
        valid(valid),
        relock(relock)
{
}

WalletModel::UnlockContext::~UnlockContext()
{
    if(valid && relock)
    {
        wallet->setWalletLocked(true);
    }
}

void WalletModel::UnlockContext::CopyFrom(const UnlockContext& rhs)
{
    // Transfer context; old object no longer relocks wallet
    *this = rhs;
    rhs.relock = false;
}

bool WalletModel::getPubKey(const CKeyID &address, CPubKey& vchPubKeyOut) const
{
    return wallet->GetPubKey(address, vchPubKeyOut);
}

// returns a list of COutputs from COutPoints
void WalletModel::getOutputs(const std::vector<COutPoint>& vOutpoints, std::vector<COutput>& vOutputs)
{
    LOCK2(cs_main, wallet->cs_wallet);
    BOOST_FOREACH(const COutPoint& outpoint, vOutpoints)
    {
        if (!wallet->mapWallet.count(outpoint.hash)) continue;
        int nDepth = wallet->mapWallet[outpoint.hash].GetDepthInMainChain();
        if (nDepth < 0) continue;
        COutput out(&wallet->mapWallet[outpoint.hash], outpoint.n, nDepth, true);
        vOutputs.push_back(out);
    }
}

bool WalletModel::isSpent(const COutPoint& outpoint) const
{
    LOCK2(cs_main, wallet->cs_wallet);
    return wallet->IsSpent(outpoint.hash, outpoint.n);
}

// AvailableCoins + LockedCoins grouped by wallet address (put change in one group with wallet address)
void WalletModel::listCoins(std::map<QString, std::vector<COutput> >& mapCoins) const
{
    std::vector<COutput> vCoins;
    wallet->AvailableCoins(vCoins);

    LOCK2(cs_main, wallet->cs_wallet); // ListLockedCoins, mapWallet
    std::vector<COutPoint> vLockedCoins;
    wallet->ListLockedCoins(vLockedCoins);

    // add locked coins
    BOOST_FOREACH(const COutPoint& outpoint, vLockedCoins)
    {
        if (!wallet->mapWallet.count(outpoint.hash)) continue;
        int nDepth = wallet->mapWallet[outpoint.hash].GetDepthInMainChain();
        if (nDepth < 0) continue;
        COutput out(&wallet->mapWallet[outpoint.hash], outpoint.n, nDepth, true);
        if (outpoint.n < out.tx->vout.size() && wallet->IsMine(out.tx->vout[outpoint.n]) == ISMINE_SPENDABLE)
            vCoins.push_back(out);
    }

    BOOST_FOREACH(const COutput& out, vCoins)
    {
        COutput cout = out;

        while (wallet->IsChange(cout.tx->vout[cout.i]) && cout.tx->vin.size() > 0 && wallet->IsMine(cout.tx->vin[0]))
        {
            if (!wallet->mapWallet.count(cout.tx->vin[0].prevout.hash)) break;
            cout = COutput(&wallet->mapWallet[cout.tx->vin[0].prevout.hash], cout.tx->vin[0].prevout.n, 0, true);
        }

        CTxDestination address;
        if(!out.fSpendable || !ExtractDestination(cout.tx->vout[cout.i].scriptPubKey, address))
            continue;
        mapCoins[QString::fromStdString(CBitcoinAddress(address).ToString())].push_back(out);
    }
}

bool WalletModel::isLockedCoin(uint256 hash, unsigned int n) const
{
    LOCK2(cs_main, wallet->cs_wallet);
    return wallet->IsLockedCoin(hash, n);
}

void WalletModel::lockCoin(COutPoint& output)
{
    LOCK2(cs_main, wallet->cs_wallet);
    wallet->LockCoin(output);
}

void WalletModel::unlockCoin(COutPoint& output)
{
    LOCK2(cs_main, wallet->cs_wallet);
    wallet->UnlockCoin(output);
}

void WalletModel::listLockedCoins(std::vector<COutPoint>& vOutpts)
{
    LOCK2(cs_main, wallet->cs_wallet);
    wallet->ListLockedCoins(vOutpts);
}

void WalletModel::loadReceiveRequests(std::vector<std::string>& vReceiveRequests)
{
    LOCK(wallet->cs_wallet);
    BOOST_FOREACH(const PAIRTYPE(CTxDestination, CAddressBookData)& item, wallet->mapAddressBook)
        BOOST_FOREACH(const PAIRTYPE(std::string, std::string)& item2, item.second.destdata)
            if (item2.first.size() > 2 && item2.first.substr(0,2) == "rr") // receive request
                vReceiveRequests.push_back(item2.second);
}

bool WalletModel::saveReceiveRequest(const std::string &sAddress, const int64_t nId, const std::string &sRequest)
{
    CTxDestination dest = CBitcoinAddress(sAddress).Get();

    std::stringstream ss;
    ss << nId;
    std::string key = "rr" + ss.str(); // "rr" prefix = "receive request" in destdata

    LOCK(wallet->cs_wallet);
    if (sRequest.empty())
        return wallet->EraseDestData(dest, key);
    else
        return wallet->AddDestData(dest, key, sRequest);
}

qint64 WalletModel::getSharedBalance(const CCoinControl *coinControl) const
{
    if (coinControl)
    {
        qint64 nBalance = 0;
        std::vector<COutput> vCoins;
        wallet->AvailableSharedCoins(vCoins, true, coinControl);
        BOOST_FOREACH(const COutput& out, vCoins)
            nBalance += out.tx->vout[out.i].nValue;   
        
        return nBalance;
    }

    return wallet->GetSharedBalance();
}

qint64 WalletModel::getSharedUnconfirmedBalance() const
{
    return wallet->GetSharedUnconfirmedBalance();
}

qint64 WalletModel::getSharedImmatureBalance() const
{
    return wallet->GetSharedImmatureBalance();
}

WalletModel::SendCoinsReturn WalletModel::createRawTransaction(
    const QList<SendCoinsRecipient> &recipients, CTransaction& txNew, const CCoinControl *coinControl, bool isMultiSig)
{
    qint64 total = 0;
    QSet<QString> setAddress;
    QString hex;

    if(recipients.empty())
    {
        return OK;
    }

    // Pre-check input data for validity
    Q_FOREACH(const SendCoinsRecipient &rcp, recipients)
    {
        if(!validateAddress(rcp.address))
        {
            return InvalidAddress;
        }
        setAddress.insert(rcp.address);

        if(rcp.amount <= 0)
        {
            return InvalidAmount;
        }
        total += rcp.amount;
    }

    if(recipients.size() > setAddress.size())
    {
        return DuplicateAddress;
    }

    qint64 nBalance;
    if ( isMultiSig )
        nBalance = getSharedBalance(coinControl);
    else
        nBalance = getBalance(coinControl);


		LogPrintf("createRawTransaction,500\n");
    if(total > nBalance)
    {
        return AmountExceedsBalance;
    }

    if((total + DEFAULT_TRANSACTION_FEE) > nBalance)
    {
        return SendCoinsReturn(AmountWithFeeExceedsBalance, DEFAULT_TRANSACTION_FEE);
    }

    {
        LOCK2(cs_main, wallet->cs_wallet);

        // Sendmany
        std::vector<std::pair<CScript, int64_t> > vecSend;
        Q_FOREACH(const SendCoinsRecipient &rcp, recipients)
        {
            CScript scriptPubKey;
            //scriptPubKey.SetDestination(CBitcoinAddress(rcp.address.toStdString()).Get());
            scriptPubKey = GetScriptForDestination(CBitcoinAddress(rcp.address.toStdString()).Get());
            vecSend.push_back(make_pair(scriptPubKey, rcp.amount));
        }

        int64_t nFeeRequired = CENT;
        std::string strFailReason;
        CReserveKey reservekey(wallet);
        bool fCreated = wallet->CreateRawTransaction(vecSend, txNew, nFeeRequired, strFailReason, isMultiSig, reservekey, coinControl);

        if(!fCreated)
        {
            if((total + nFeeRequired) > nBalance)
            {
                return SendCoinsReturn(AmountWithFeeExceedsBalance, nFeeRequired);
            }
            Q_EMIT message(tr("Send Coins"), QString::fromStdString(strFailReason),
                         CClientUIInterface::MSG_ERROR);
            return TransactionCreationFailed;
        }
        /*if(!uiInterface.ThreadSafeAskFee(nFeeRequired))
        {
            return Aborted;
        }*/
        hex = QString::fromStdString(txNew.GetHash().GetHex());
    }

    // Add addresses / update labels that we've sent to to the address book
    Q_FOREACH(const SendCoinsRecipient &rcp, recipients)
    {
        std::string strAddress = rcp.address.toStdString();
        CTxDestination dest = CBitcoinAddress(strAddress).Get();
        std::string strLabel = rcp.label.toStdString();
        {
            LOCK(wallet->cs_wallet);

            //std::map<CTxDestination, std::string>::iterator mi = wallet->mapAddressBook.find(dest);
            std::map<CTxDestination, CAddressBookData>::iterator mi = wallet->mapAddressBook.find(dest);

            // Check if we have a new address or an updated label
            if (mi == wallet->mapAddressBook.end() || mi->second.name != strLabel)
            {
            		std::string purpose;
                //wallet->SetAddressBookName(dest, strLabel);
                wallet->SetAddressBook(dest, strLabel,purpose);
            }
        }
    }

    return SendCoinsReturn(OK, 0, hex);
}


bool WalletModel::importPrivateKey(QString privKey)
{
    CBitcoinSecret vchSecret;
    bool fGood = vchSecret.SetString(privKey.toStdString());
    if (!fGood)
        return false;
    CKey key = vchSecret.GetKey();
    CPubKey pubkey = key.GetPubKey();
    CKeyID vchAddress = pubkey.GetID();
    //CKey key;
    //bool fCompressed;
    //CSecret secret = vchSecret.GetSecret(fCompressed);
    //key.SetSecret(secret, fCompressed);
    //CKeyID vchAddress = key.GetPubKey().GetID();
    {
        LOCK2(cs_main, wallet->cs_wallet); 
        wallet->MarkDirty();
        wallet->SetAddressBook(vchAddress, ("imported wallet"),"send");
        if (!wallet->AddKeyPubKey(key, pubkey))
        //if (!pwalletMain->AddKey(key))
            return false;
        wallet->ScanForWalletTransactions(chainActive.Genesis(), true);
        wallet->ReacceptWalletTransactions();
    }
    return true;
}
