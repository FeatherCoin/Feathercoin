#include "createmultisigaddrdialog.h"
#include "ui_createmultisigaddrdialog.h"

#include "addresstablemodel.h"
#include "guiutil.h"

#include "wallet.h"
#include "base58.h"
#include "init.h"
#include "util.h"

#include "json/json_spirit.h"
#include "json/json_spirit_reader_template.h"
#include "json/json_spirit_writer_template.h"
using namespace json_spirit;

#include <QSortFilterProxyModel>
#include <QClipboard>
#include <QMessageBox>
#include <QMenu>
#include <QTextDocument>
#include <QScrollBar>
#include <QFile>
#include <QTextStream>
#include <QDataWidgetMapper>

CreateMultiSigAddrDialog::CreateMultiSigAddrDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateMultiSigAddrDialog)
{
    ui->setupUi(this);

    connect(ui->btnCreate, SIGNAL(clicked()), this, SLOT(create()));
    connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(cancel()));

    ui->comboBoxRequire->addItem(QString("1"), QVariant(1));
    ui->comboBoxRequire->addItem(QString("2"), QVariant(2));
    ui->comboBoxRequire->addItem(QString("3"), QVariant(3));
    ui->comboBoxRequire->setCurrentIndex(1);
    
    ui->comboBoxTotal->addItem(QString("2"), QVariant(2));
    ui->comboBoxTotal->addItem(QString("3"), QVariant(3));
    ui->comboBoxTotal->setCurrentIndex(0);
    currentPubkeyNum = 2;
    connect(ui->comboBoxTotal, SIGNAL(currentIndexChanged(int)), this, SLOT(handleSelectionChanged(int)));

    connect(ui->btnImport, SIGNAL(clicked()), this, SLOT(importAddress()));
    
    connect(ui->pubkeyEdit0, SIGNAL(textChanged( const QString &)), this, SLOT(onTextChanged0(const QString &)));
    connect(ui->pubkeyEdit1, SIGNAL(textChanged( const QString &)), this, SLOT(onTextChanged1(const QString &)));
    connect(ui->pubkeyEdit2, SIGNAL(textChanged( const QString &)), this, SLOT(onTextChanged2(const QString &)));
    
    ui->pubkeyEdit2->setVisible(false);
    ui->label2->setVisible(false);
    updatePromptText();
}

CreateMultiSigAddrDialog::~CreateMultiSigAddrDialog()
{
    delete ui;
}

void CreateMultiSigAddrDialog::create()
{
    int nRequired = ui->comboBoxRequire->itemData(ui->comboBoxRequire->currentIndex()).toInt();
    int total = ui->comboBoxTotal->itemData(ui->comboBoxTotal->currentIndex()).toInt();

    QString pubkeyHex[3];
    pubkeyHex[0] = ui->pubkeyEdit0->text();
    pubkeyHex[1] = ui->pubkeyEdit1->text();
    if ( 3 == total )
        pubkeyHex[2] = ui->pubkeyEdit2->text();

    int myKeyNum = 0;
    //std::vector<CPubKey> pubkeys;
    std::vector<CKey> pubkeys;
    pubkeys.resize(total);
    for (unsigned int i = 0; i < total; i++)
    {
        const std::string& ks = pubkeyHex[i].toStdString();
        if (IsHex(ks))
        {
            CPubKey vchPubKey(ParseHex(ks));
            //if (!vchPubKey.IsFullyValid())
            if (!vchPubKey.IsValid() || !pubkeys[i].SetPubKey(vchPubKey))
            {
                QMessageBox::warning(this, windowTitle(),
                    tr("Invalid public key  \"%1\" ").arg(pubkeyHex[i]),
                    QMessageBox::Ok, QMessageBox::Ok);
                return;
            }
            //pubkeys[i] = vchPubKey;
            if (IsMine(*pwalletMain, vchPubKey.GetID()))
                myKeyNum += 1;
        }
        else
        {
            QMessageBox::warning(this, windowTitle(),
                tr("Invalid public key  \"%1\" ").arg(pubkeyHex[i]),
                QMessageBox::Ok, QMessageBox::Ok);
            return;
        }
    }
/*
    if ( 0 == myKeyNum )
    {
        QMessageBox::warning(this, windowTitle(),
            tr("No public key belongs to this wallet!"),
            QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
    */
    if ( total == myKeyNum )
    {
        QMessageBox::warning(this, windowTitle(),
            tr("All public keys belong to this wallet!"),
            QMessageBox::Ok, QMessageBox::Ok);
        return;
    }

    CScript inner;
    inner.SetMultisig(nRequired, pubkeys);
    CScriptID innerID = inner.GetID();
    pwalletMain->AddCScript(inner);
    std::string strAccount;
    //pwalletMain->SetAddressBookName(innerID, strAccount);
    std::string purpose;
    pwalletMain->SetAddressBook(innerID, strAccount, purpose);

    accept();
}

void CreateMultiSigAddrDialog::cancel()
{
    reject();
}

void CreateMultiSigAddrDialog::updatePromptText()
{
    int nRequired = ui->comboBoxRequire->itemData(ui->comboBoxRequire->currentIndex()).toInt();
    int total = ui->comboBoxTotal->itemData(ui->comboBoxTotal->currentIndex()).toInt();

    QString text = QString(tr("You need ")) + QString::number(total) + QString(tr(" keys to create a ")) 
        + QString::number(nRequired) + QString(tr("-of-")) + QString::number(total) + QString(tr(" MultiSig address"));

    ui->labelPromptText->setText(text);
}

void CreateMultiSigAddrDialog::handleSelectionChanged(int idx)
{
    int num = idx + 2;
    if ( currentPubkeyNum != num )
    {
        currentPubkeyNum = num;
        if ( 3 == currentPubkeyNum )
        {
            ui->pubkeyEdit2->setVisible(true);
            ui->label2->setVisible(true);
        }
        else
        {
            ui->pubkeyEdit2->setVisible(false);
            ui->label2->setVisible(false);
        }
    }
    updatePromptText();
}

void CreateMultiSigAddrDialog::onTextChanged0(const QString & text)
{
    onTextChanged(ui->label0, text);
}

void CreateMultiSigAddrDialog::onTextChanged1(const QString & text)
{
    onTextChanged(ui->label1, text);
}

void CreateMultiSigAddrDialog::onTextChanged2(const QString & text)
{
    onTextChanged(ui->label2, text);
}

void CreateMultiSigAddrDialog::onTextChanged(QLabel* label, const QString & text)
{
    label->setText("");
    const std::string& ks = text.toStdString();
    if (IsHex(ks))
    {
        CPubKey vchPubKey(ParseHex(ks));
        //if (!vchPubKey.IsFullyValid())
        if (!vchPubKey.IsValid())
        {
            return;
        }

        CBitcoinAddress address(vchPubKey.GetID());
        label->setText(QString::fromStdString(address.ToString()));
    }
}

void CreateMultiSigAddrDialog::importAddress()
{
    QString filename = GUIUtil::getLoadFileName(
            this,
            tr("Load MultiSig Address"), QString(),
            tr("MultiSig Address file (*.msa)"));

    if (filename.isNull()) return;
    
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QString addrJsonStr;
    QTextStream strin(&file);
    strin >> addrJsonStr;

    Value addrJsonV;
    if (!json_spirit::read_string(addrJsonStr.toStdString(), addrJsonV))
        return;

    const json_spirit::Object& addrJson = addrJsonV.get_obj();
    if (addrJson.empty())
        return;

    const json_spirit::Value& addressV = json_spirit::find_value(addrJson, "address");
    const json_spirit::Value& scriptV  = json_spirit::find_value(addrJson, "redeemScript");

    printf("importAddress redeemScript=%s\n", scriptV.get_str().c_str());
    std::vector<unsigned char> scriptData(ParseHex(scriptV.get_str()));
    CScript scriptPubKey(scriptData.begin(), scriptData.end());

    txnouttype whichType;
    if (!IsStandard(scriptPubKey, whichType))
        return;

    //if ( !scriptPubKey.IsPayToScriptHash())
        //return;

    CScriptID innerID = scriptPubKey.GetID();
    CBitcoinAddress address(innerID);
    if ( addressV.get_str() != address.ToString() )
        return;
    
    int nRequired = 0;
    txnouttype typeRet = TX_NONSTANDARD;
    std::vector<std::vector<unsigned char> > vSolutions;
    if (!Solver(scriptPubKey, typeRet, vSolutions))
        return;

    if (typeRet != TX_MULTISIG)
        return;

    std::vector<std::string> vPubKeys;
    nRequired = vSolutions.front()[0];
    for (unsigned int i = 1; i < vSolutions.size()-1; i++)
    {
        CPubKey vchPubKey(vSolutions[i]);
        vPubKeys.push_back(HexStr(vchPubKey));
    }
    

    if ( vPubKeys.size() == 3 )
    {
        ui->comboBoxTotal->setCurrentIndex(1);
        ui->pubkeyEdit2->setText(QString::fromStdString(vPubKeys[2]));
        ui->pubkeyEdit1->setText(QString::fromStdString(vPubKeys[1]));
        ui->pubkeyEdit0->setText(QString::fromStdString(vPubKeys[0]));
    }
    else if ( vPubKeys.size() == 2 )
    {
        ui->comboBoxTotal->setCurrentIndex(0);
        ui->pubkeyEdit1->setText(QString::fromStdString(vPubKeys[1]));
        ui->pubkeyEdit0->setText(QString::fromStdString(vPubKeys[0]));
    }
    else
        return;
    
    if ( nRequired <= 3 && nRequired >= 1 )
        ui->comboBoxRequire->setCurrentIndex(nRequired - 1);
    else
        return;
    

#if 0
    CScriptID innerID = scriptPubKey.GetID();
    CBitcoinAddress address(innerID);
    if ( addressV.get_str() == address.ToString() )
    {
        printf("importAddress %s\n", address.ToString().c_str());
        pwalletMain->AddCScript(scriptPubKey);
        std::string strAccount;
        pwalletMain->SetAddressBookName(innerID, strAccount);
    }
#endif
}

