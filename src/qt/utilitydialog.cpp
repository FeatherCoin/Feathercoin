// Copyright (c) 2011-2014 The Bitcoin Core developers
// Copyright (c) 2013-2017 The Feathercoin developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "utilitydialog.h"

#include "ui_helpmessagedialog.h"
#include "ui_commentdialog.h"
#include "ui_paperwalletdialog.h"
#include "ui_debugdialog.h"
#include "ui_interface.h"

#include "bitcoingui.h"
#include "clientmodel.h"
#include "guiutil.h"

#include "clientversion.h"
#include "init.h"
#include "util.h"
#include "net.h"
#include "main.h"

#include "addresstablemodel.h"
#include "optionsmodel.h"
#include "script/standard.h"
#include "amount.h"
#include "bitcoinunits.h"
#include "core_io.h"
#include "rpcserver.h"
#include "consensus/validation.h"

#ifdef ENABLE_WALLET
#include "sendcoinsdialog.h"
#include "sendcoinsentry.h"
#include "coincontrol.h"
#include "coincontroldialog.h"
#include "walletmodel.h"
#endif

#include <stdio.h>

#include <QCloseEvent>
#include <QLabel>
#include <QFont>
#include <QRegExp>
#include <QTextTable>
#include <QTextCursor>
#include <QVBoxLayout>
#include <QInputDialog>
#include <QStringListModel>
#include <QClipboard>


// Use QT5's new modular classes
#if QT_VERSION >= 0x050000
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrintPreviewDialog>

#else

#include <QPrinter>
#include <QPrintDialog>
#include <QPrintPreviewDialog>

#endif
#include <QPainter>

#ifdef USE_QRCODE
#include <qrencode.h>
#endif

using namespace std;
using namespace json_spirit;

extern void ScriptPubKeyToJSON(const CScript& scriptPubKey, Object& out, bool fIncludeHex);

/** "Help message" or "About" dialog box */
HelpMessageDialog::HelpMessageDialog(QWidget *parent, bool about) :
    QDialog(parent),
    ui(new Ui::HelpMessageDialog)
{
    ui->setupUi(this);

    QString version = tr("Feathercoin Core") + " " + tr("version") + " " + QString::fromStdString(FormatFullVersion());
    /* On x86 add a bit specifier to the version so that users can distinguish between
     * 32 and 64 bit builds. On other architectures, 32/64 bit may be more ambigious.
     */
#if defined(__x86_64__)
    version += " " + tr("(%1-bit)").arg(64);
#elif defined(__i386__ )
    version += " " + tr("(%1-bit)").arg(32);
#endif

    if (about)
    {
        setWindowTitle(tr("About Feathercoin Core"));

        /// HTML-format the license message from the core
        QString licenseInfo = QString::fromStdString(LicenseInfo());
        QString licenseInfoHTML = licenseInfo;
        // Make URLs clickable
        QRegExp uri("<(.*)>", Qt::CaseSensitive, QRegExp::RegExp2);
        uri.setMinimal(true); // use non-greedy matching
        licenseInfoHTML.replace(uri, "<a href=\"\\1\">\\1</a>");
        // Replace newlines with HTML breaks
        licenseInfoHTML.replace("\n\n", "<br><br>");

        ui->aboutMessage->setTextFormat(Qt::RichText);
        ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        text = version + "\n" + licenseInfo;
        ui->aboutMessage->setText(version + "<br><br>" + licenseInfoHTML);
        ui->aboutMessage->setWordWrap(true);
        ui->helpMessage->setVisible(false);
        ui->aboutLogo->setVisible(true);
        ui->aboutLogo2->setVisible(false);
    } else {
        setWindowTitle(tr("Command-line options"));
        QString header = tr("Usage:") + "\n" +
            "  feathercoin-qt [" + tr("command-line options") + "]                     " + "\n";
        QTextCursor cursor(ui->helpMessage->document());
        cursor.insertText(version);
        cursor.insertBlock();
        cursor.insertText(header);
        cursor.insertBlock();

        QString coreOptions = QString::fromStdString(HelpMessage(HMM_BITCOIN_QT));
        text = version + "\n" + header + "\n" + coreOptions;

        QTextTableFormat tf;
        tf.setBorderStyle(QTextFrameFormat::BorderStyle_None);
        tf.setCellPadding(2);
        QVector<QTextLength> widths;
        widths << QTextLength(QTextLength::PercentageLength, 35);
        widths << QTextLength(QTextLength::PercentageLength, 65);
        tf.setColumnWidthConstraints(widths);

        QTextCharFormat bold;
        bold.setFontWeight(QFont::Bold);

        Q_FOREACH (const QString &line, coreOptions.split("\n")) {
            if (line.startsWith("  -"))
            {
                cursor.currentTable()->appendRows(1);
                cursor.movePosition(QTextCursor::PreviousCell);
                cursor.movePosition(QTextCursor::NextRow);
                cursor.insertText(line.trimmed());
                cursor.movePosition(QTextCursor::NextCell);
            } else if (line.startsWith("   ")) {
                cursor.insertText(line.trimmed()+' ');
            } else if (line.size() > 0) {
                //Title of a group
                if (cursor.currentTable())
                    cursor.currentTable()->appendRows(1);
                cursor.movePosition(QTextCursor::Down);
                cursor.insertText(line.trimmed(), bold);
                cursor.insertTable(1, 2, tf);
            }
        }

        ui->helpMessage->moveCursor(QTextCursor::Start);
        ui->scrollArea->setVisible(false);
        ui->aboutLogo->setVisible(false);
        ui->aboutLogo2->setVisible(true);
    }
}

HelpMessageDialog::~HelpMessageDialog()
{
    delete ui;
}

void HelpMessageDialog::printToConsole()
{
    // On other operating systems, the expected action is to print the message to the console.
    fprintf(stdout, "%s\n", qPrintable(text));
}

void HelpMessageDialog::showOrPrint()
{
#if defined(WIN32)
    // On Windows, show a message box, as there is no stderr/stdout in windowed applications
    exec();
#else
    // On other operating systems, print help text to console
    printToConsole();
#endif
}

void HelpMessageDialog::on_okButton_accepted()
{
    close();
}


/** "Shutdown" window */
ShutdownWindow::ShutdownWindow(QWidget *parent, Qt::WindowFlags f):
    QWidget(parent, f)
{
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(new QLabel(
        tr("Feathercoin Core is shutting down...") + "<br /><br />" +
        tr("Do not shut down the computer until this window disappears.")));
    setLayout(layout);
}

void ShutdownWindow::showShutdownWindow(BitcoinGUI *window)
{
    if (!window)
        return;

    // Show a simple window indicating shutdown status
    QWidget *shutdownWindow = new ShutdownWindow();
    // We don't hold a direct pointer to the shutdown window after creation, so use
    // Qt::WA_DeleteOnClose to make sure that the window will be deleted eventually.
    shutdownWindow->setAttribute(Qt::WA_DeleteOnClose);
    shutdownWindow->setWindowTitle(window->windowTitle());

    // Center shutdown window at where main window was
    const QPoint global = window->mapToGlobal(window->rect().center());
    shutdownWindow->move(global.x() - shutdownWindow->width() / 2, global.y() - shutdownWindow->height() / 2);
    shutdownWindow->show();
}

void ShutdownWindow::closeEvent(QCloseEvent *event)
{
    event->ignore();
}



/** class "CommentDialog" dialog box */
CommentDialog::CommentDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CommentDialog)
{
    ui->setupUi(this);
}

void CommentDialog::setModel(WalletModel *model)
{
    this->model = model;    
}

CommentDialog::~CommentDialog()
{
    delete ui;
}

void CommentDialog::on_insertButton_clicked()
{
    if(!model || !model->getOptionsModel())
        return;
        
    QString addrOP=ui->addrEdit->text();
    QString textOP=ui->txtComment->text();
    if (textOP.length() > MAX_OP_RETURN_RELAY)
    {
        QMessageBox::information(NULL, tr("Wallet Message"), tr("The comment length can not be above 80 charset !"), QMessageBox::Yes , QMessageBox::Yes);
        return;
    }
    
    QList<SendCoinsRecipient> recipients;
    SendCoinsRecipient rcptmp;
    // Payment request
    if (rcptmp.paymentRequest.IsInitialized())
        return ;
    rcptmp.typeInd = AddressTableModel::AT_Normal;
		rcptmp.address = addrOP;
		rcptmp.label = "comment";
    rcptmp.amount = DUST_HARD_LIMIT;
    rcptmp.message = textOP;
    recipients.append(rcptmp);
    
    // Format confirmation message
    QStringList formatted;
    Q_FOREACH(const SendCoinsRecipient &rcp, recipients)
    {
        // generate bold amount string        
        QString amount = "<b>" + BitcoinUnits::formatWithUnit(model->getOptionsModel()->getDisplayUnit(), rcp.amount);
        amount.append("</b>");
        // generate monospace address string
        QString address = "<span style='font-family: monospace;'>" + rcp.address;
        address.append("</span>");

        QString recipientElement;
        if (!rcp.paymentRequest.IsInitialized()) // normal payment
        {
            if(rcp.label.length() > 0) // label with address
            {
                recipientElement = tr("%1 to %2").arg(amount, GUIUtil::HtmlEscape(rcp.label));
                recipientElement.append(QString(" (%1)").arg(address));
            }
            else // just address
            {
                recipientElement = tr("%1 to %2").arg(amount, address);
            }
        }
        else if(!rcp.authenticatedMerchant.isEmpty()) // secure payment request
        {
            recipientElement = tr("%1 to %2").arg(amount, GUIUtil::HtmlEscape(rcp.authenticatedMerchant));
        }
        else // insecure payment request
        {
            recipientElement = tr("%1 to %2").arg(amount, address);
        }

        formatted.append(recipientElement);
    }
      
    // prepare transaction for getting txFee earlier
    WalletModelTransaction currentTransaction(recipients);
    WalletModel::SendCoinsReturn prepareStatus;
    if (model->getOptionsModel()->getCoinControlFeatures()) // coin control enabled
        prepareStatus = model->prepareTransaction(currentTransaction, CoinControlDialog::coinControl);
    else
        prepareStatus = model->prepareTransaction(currentTransaction);

    // process prepareStatus and on error generate message shown to user
    processSendCoinsReturn(prepareStatus,
        BitcoinUnits::formatWithUnit(model->getOptionsModel()->getDisplayUnit(), currentTransaction.getTransactionFee()));
        	
    if(prepareStatus.status != WalletModel::OK) {
        return;
    }
    
    QString questionString = tr("Are you sure you want to send?");
    questionString.append("<br /><br />%1");
		qint64 txFee = currentTransaction.getTransactionFee();
    if(txFee > 0)
    {
        // append fee string if a fee is required
        questionString.append("<hr /><span style='color:#aa0000;'>");
        questionString.append(BitcoinUnits::formatWithUnit(model->getOptionsModel()->getDisplayUnit(), txFee));
        questionString.append("</span> ");
        questionString.append(tr("added as transaction fee"));
    }

    // add total amount in all subdivision units
    questionString.append("<hr />");
    qint64 totalAmount = currentTransaction.getTotalTransactionAmount() + txFee;
    QStringList alternativeUnits;
    Q_FOREACH(BitcoinUnits::Unit u, BitcoinUnits::availableUnits())
    {
        if(u != model->getOptionsModel()->getDisplayUnit())
            alternativeUnits.append(BitcoinUnits::formatWithUnit(u, totalAmount));
    }
    questionString.append(tr("Total Amount %1 (= %2)")
        .arg(BitcoinUnits::formatWithUnit(model->getOptionsModel()->getDisplayUnit(), totalAmount))
        .arg(alternativeUnits.join(" " + tr("or") + " ")));
    QMessageBox::StandardButton retval = QMessageBox::question(this, tr("Confirm send coins"),
        questionString.arg(formatted.join("<br />")),
        QMessageBox::Yes | QMessageBox::Cancel,
        QMessageBox::Cancel);
    if(retval != QMessageBox::Yes)
    {
        return;
    }
    
    // now send the prepared transaction
    WalletModel::SendCoinsReturn sendStatus = model->sendCoins(currentTransaction,true);
    if (sendStatus.status == WalletModel::OK)
    {
        QMessageBox::information(NULL, tr("Wallet Message"), tr("Insert into blockchain ,Yes!!!"), QMessageBox::Yes , QMessageBox::Yes);
        ui->txtComment->setText("");
    }
}

void CommentDialog::processSendCoinsReturn(const WalletModel::SendCoinsReturn &sendCoinsReturn, const QString &msgArg)
{
    QPair<QString, CClientUIInterface::MessageBoxFlags> msgParams;
    // Default to a warning message, override if error message is needed
    msgParams.second = CClientUIInterface::MSG_WARNING;

    // This comment is specific to SendCoinsDialog usage of WalletModel::SendCoinsReturn.
    // WalletModel::TransactionCommitFailed is used only in WalletModel::sendCoins()
    // all others are used only in WalletModel::prepareTransaction()
    switch(sendCoinsReturn.status)
    {
    case WalletModel::InvalidAddress:
        msgParams.first = tr("The recipient address is not valid. Please recheck.");
        break;
    case WalletModel::InvalidAmount:
        msgParams.first = tr("The amount to pay must be larger than 0.");
        break;
    case WalletModel::AmountExceedsBalance:
        msgParams.first = tr("The amount exceeds your balance.");
        break;
    case WalletModel::AmountWithFeeExceedsBalance:
        msgParams.first = tr("The total exceeds your balance when the %1 transaction fee is included.").arg(msgArg);
        break;
    case WalletModel::DuplicateAddress:
        msgParams.first = tr("Duplicate address found: addresses should only be used once each.");
        break;
    case WalletModel::TransactionCreationFailed:
        msgParams.first = tr("Transaction creation failed!");
        msgParams.second = CClientUIInterface::MSG_ERROR;
        break;
    case WalletModel::TransactionCommitFailed:
        msgParams.first = tr("The transaction was rejected! This might happen if some of the coins in your wallet were already spent, such as if you used a copy of wallet.dat and coins were spent in the copy but not marked as spent here.");
        msgParams.second = CClientUIInterface::MSG_ERROR;
        break;
    case WalletModel::AbsurdFee:
        msgParams.first = tr("A fee higher than %1 is considered an absurdly high fee.").arg(BitcoinUnits::formatWithUnit(model->getOptionsModel()->getDisplayUnit(), 10000000));
        break;
    case WalletModel::PaymentRequestExpired:
        msgParams.first = tr("Payment request expired.");
        msgParams.second = CClientUIInterface::MSG_ERROR;
        break;
    // included to prevent a compiler warning.
    case WalletModel::OK:
    default:
        return;
    }

    Q_EMIT message(tr("Send Coins"), msgParams.first, msgParams.second);
}

void CommentDialog::on_pushButton_clicked()
{
    close();
}




/** "PaperWallet" dialog box */
PaperWalletDialog::PaperWalletDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PaperWalletDialog)
{
    ui->setupUi(this);

    ui->buttonBox->addButton(tr("Close"), QDialogButtonBox::RejectRole);

    // Begin with a small bold monospace font for the textual version of the key and address.
    QFont font("Monospace");
    font.setBold(true);
    font.setStyleHint(QFont::TypeWriter);
    font.setPixelSize(1);
    ui->addressText->setFont(font);
    ui->privateKeyText->setFont(font);
    ui->addressText->setAlignment(Qt::AlignJustify);
    ui->privateKeyText->setAlignment(Qt::AlignJustify);

    if (vNodes.size() > 0) {

		QMessageBox::critical(this, tr("Warning: Network Activity Detected"), tr("It is recommended to disconnect from the internet before printing paper wallets. Even though paper wallets are generated on your local computer, it is still possible to unknowingly have malware that transmits your screen to a remote location. It is also recommended to print to a local printer vs a network printer since that network traffic can be monitored. Some advanced printers also store copies of each printed document. Proceed with caution relative to the amount of value you plan to store on each address."), QMessageBox::Ok, QMessageBox::Ok);

    }

}

void PaperWalletDialog::setModel(WalletModel *model)
{
    RandAddSeed();
    this->model = model;
    this->on_getNewAddress_clicked();
}

PaperWalletDialog::~PaperWalletDialog()
{
    delete ui;
}

void PaperWalletDialog::on_getNewAddress_clicked()
{
    // Create a new private key
    CKey privKey;
    privKey.MakeNewKey(true);
    // CSecret is a serialization of just the secret parameter (32 bytes)
    // CSecret vchSecret = privKey.GetSecret(fCompressed);
    // CPrivKey is a serialized private key, with all parameters included (279 bytes),use secp256k1 in key.cpp NID_secp256k1
    // CPrivKey vchPrivKey=privKey.GetPrivKey();
    // if (fDebug) LogPrintf("PaperWalletDialog CPrivKey=%s\n", HexStr(vchPrivKey).c_str());
    // if (fDebug) LogPrintf("PaperWalletDialog CSecret=%s\n", HexStr(vchSecret).c_str());

    // Derive the public key
    CPubKey pubkey = privKey.GetPubKey();

    // Derive the public key hash
    CBitcoinAddress pubkeyhash;
    pubkeyhash.Set(pubkey.GetID());

    // Create String versions of each
    std::string myPrivKey = CBitcoinSecret(privKey).ToString();
    std::string myPubKey = HexStr(pubkey.begin(), pubkey.end());
    //string myPubKey = HexStr(privKey.GetPubKey().Raw());
    std::string myAddress = pubkeyhash.ToString();


#ifdef USE_QRCODE
    // Generate the address QR code
    QRcode *code = QRcode_encodeString(myAddress.c_str(), 0, QR_ECLEVEL_M, QR_MODE_8, 1);
    if (!code)
    {
        ui->addressQRCode->setText(tr("Error encoding Address into QR Code."));
        return;
    }
    QImage publicKeyImage = QImage(code->width, code->width, QImage::Format_ARGB32);
    publicKeyImage.fill(0x000000);
    unsigned char *p = code->data;
    for (int y = 0; y < code->width; y++)
    {
        for (int x = 0; x < code->width; x++)
        {
            publicKeyImage.setPixel(x, y, ((*p & 1) ? 0xff000000 : 0x0));
            p++;
        }
    }
    QRcode_free(code);


    // Generate the private key QR code
    code = QRcode_encodeString(myPrivKey.c_str(), 0, QR_ECLEVEL_M, QR_MODE_8, 1);
    if (!code)
    {
        ui->privateKeyQRCode->setText(tr("Error encoding private key into QR Code."));
        return;
    }
    QImage privateKeyImage = QImage(code->width, code->width, QImage::Format_ARGB32);
    privateKeyImage.fill(0x000000);
    p = code->data;
    for (int y = 0; y < code->width; y++)
    {
        for (int x = 0; x < code->width; x++)
        {
            privateKeyImage.setPixel(x, y, ((*p & 1) ? 0xff000000 : 0x0));
            p++;
        }
    }
    QRcode_free(code);

    // Populate the QR Codes
    ui->addressQRCode->setPixmap(QPixmap::fromImage(publicKeyImage).scaled(ui->addressQRCode->width(), ui->addressQRCode->height()));
    ui->privateKeyQRCode->setPixmap(QPixmap::fromImage(privateKeyImage).scaled(ui->privateKeyQRCode->width(), ui->privateKeyQRCode->height()));
#endif

    // Populate the Texts
    ui->addressText->setText(myAddress.c_str());
    ui->privateKeyText->setText(tr(myPrivKey.c_str()));

    ui->publicKey->setHtml(myPubKey.c_str());

    // Update the fonts to fit the height of the wallet.
    // This should only really trigger the first time since the font size persists.
    double paperHeight = (double) ui->paperTemplate->height();
    double maxTextWidth = paperHeight * 0.99;   
    double minTextWidth = paperHeight * 0.95;
    int pixelSizeStep = 1;

    int addressTextLength = ui->addressText->fontMetrics().boundingRect(ui->addressText->text()).width();
    QFont font = ui->addressText->font();
    for(int i = 0; i < PAPER_WALLET_READJUST_LIMIT; i++) {
        if ( addressTextLength < minTextWidth) {
            font.setPixelSize(font.pixelSize() + pixelSizeStep);
            ui->addressText->setFont(font);
            addressTextLength = ui->addressText->fontMetrics().boundingRect(ui->addressText->text()).width();
        } else {
            break;
        }

    }
    if ( addressTextLength > maxTextWidth ) {
        font.setPixelSize(font.pixelSize() - pixelSizeStep);
        ui->addressText->setFont(font);
        addressTextLength = ui->addressText->fontMetrics().boundingRect(ui->addressText->text()).width();
    }

    int privateKeyTextLength = ui->privateKeyText->fontMetrics().boundingRect(ui->privateKeyText->text()).width();
    font = ui->privateKeyText->font();
    for(int i = 0; i < PAPER_WALLET_READJUST_LIMIT; i++) {
        if ( privateKeyTextLength < minTextWidth) {
            font.setPixelSize(font.pixelSize() + pixelSizeStep);
            ui->privateKeyText->setFont(font);
            privateKeyTextLength = ui->privateKeyText->fontMetrics().boundingRect(ui->privateKeyText->text()).width();
        } else {
            break;
        }
    }
    if ( privateKeyTextLength > maxTextWidth ) {
        font.setPixelSize(font.pixelSize() - pixelSizeStep);
        ui->privateKeyText->setFont(font);
        privateKeyTextLength = ui->privateKeyText->fontMetrics().boundingRect(ui->privateKeyText->text()).width();
    }

}

void PaperWalletDialog::on_printButton_clicked()
{

    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog *qpd = new QPrintDialog(&printer, this);

    qpd->setPrintRange(QAbstractPrintDialog::AllPages);

    QList<QString> recipientPubKeyHashes;

    if ( qpd->exec() != QDialog::Accepted ) {
        return;
    }

    // Hardcode these values
    printer.setOrientation(QPrinter::Portrait);
    printer.setPaperSize(QPrinter::A4);
    printer.setFullPage(true);

    QPainter painter;
    if (! painter.begin(&printer)) { // failed to open file
        QMessageBox::critical(this, "Printing Error", tr("failed to open file, is it writable?"), QMessageBox::Ok, QMessageBox::Ok);
        return;
    }

    int walletCount = ui->walletCount->currentIndex() + 1;
    int walletsPerPage = 4;

    int pageHeight = printer.pageRect().height() - PAPER_WALLET_PAGE_MARGIN;
    int walletHeight = ui->paperTemplate->height();
    double computedWalletHeight = 0.9 * pageHeight / walletsPerPage;
    double scale = computedWalletHeight / walletHeight;
    double walletPadding = pageHeight * 0.05 / (walletsPerPage - 1) / scale;

    QRegion walletRegion = QRegion(ui->paperTemplate->x(), ui->paperTemplate->y(),
    ui->paperTemplate->width(), ui->paperTemplate->height());
        painter.scale(scale, scale);

    for(int i = 0; i < walletCount; i++) {

        QPoint point = QPoint(PAPER_WALLET_PAGE_MARGIN, (PAPER_WALLET_PAGE_MARGIN / 2) + ( i % walletsPerPage ) * (walletHeight + walletPadding));
        this->render(&painter, point, walletRegion);
        recipientPubKeyHashes.append(ui->addressText->text());

        if ( i % walletsPerPage == ( walletsPerPage - 1 ) ) {

            printer.newPage();

        }

        this->on_getNewAddress_clicked();

    }

    painter.end();

#ifdef ENABLE_WALLET
    QStringList formatted;

    WalletModelTransaction *tx;
    while( true ) {
        bool ok;

        // Ask for an amount to send to each paper wallet. It might be better to try to use the BitcoinAmountField, but this works fine.
        double amountInput = QInputDialog::getDouble(this, tr("Load Paper Wallets"), tr("The paper wallet printing process has begun.<br/>Please wait for the wallets to print completely and verify that everything printed correctly.<br/>Check for misalignments, ink bleeding, smears, or anything else that could make the private keys unreadable.<br/>Now, enter the number of FTC you wish to send to each wallet:"), 0, 0, 2147483647, 8, &ok);

        if(!ok) {
            return;
        }


        WalletModel::UnlockContext ctx(this->model->requestUnlock());
        if(!ctx.isValid())
        {
            return;
        }

        QList<SendCoinsRecipient> recipients;
        quint64 amount = (quint64) ( amountInput * COIN );
        Q_FOREACH(const QString &dest, recipientPubKeyHashes)
        {
            recipients.append(SendCoinsRecipient(dest,tr("Paper wallet %1").arg(dest), amount,""));
            formatted.append(tr("<b>%1</b> to Paper Wallet <span style='font-family: monospace;'>%2</span>").arg(QString::number(amountInput, 'f', 8), GUIUtil::HtmlEscape(dest)));
        }

        tx = new WalletModelTransaction(recipients);

        WalletModel::SendCoinsReturn prepareStatus;
        if (this->model->getOptionsModel()->getCoinControlFeatures()) // coin control enabled
            prepareStatus = this->model->prepareTransaction(*tx, CoinControlDialog::coinControl);
        else
            prepareStatus = this->model->prepareTransaction(*tx);

        if (prepareStatus.status == WalletModel::InvalidAddress) {
            QMessageBox::critical(this, tr("Send Coins"), tr("The recipient address is not valid, please recheck."), QMessageBox::Ok, QMessageBox::Ok);
        } else if (prepareStatus.status == WalletModel::InvalidAmount) {
            QMessageBox::critical(this, tr("Send Coins"), tr("The amount to pay must be larger than 0"), QMessageBox::Ok, QMessageBox::Ok);
        } else if (prepareStatus.status == WalletModel::AmountExceedsBalance) {
            QMessageBox::critical(this, tr("Send Coins"), tr("The amount exceeds your balance."), QMessageBox::Ok, QMessageBox::Ok);
        } else if (prepareStatus.status == WalletModel::AmountWithFeeExceedsBalance) {
            QMessageBox::critical(this, tr("Send Coins"), tr("The total exceeds your balance when the transaction fee is included"), QMessageBox::Ok, QMessageBox::Ok);
        } else if (prepareStatus.status == WalletModel::DuplicateAddress) {
            QMessageBox::critical(this, tr("Send Coins"), tr("Duplicate address found, can only send to each address once per send operation."), QMessageBox::Ok, QMessageBox::Ok);
        } else if (prepareStatus.status == WalletModel::TransactionCreationFailed) {
            QMessageBox::critical(this, tr("Send Coins"), tr("Transaction creation failed!"), QMessageBox::Ok, QMessageBox::Ok);
        } else if (prepareStatus.status == WalletModel::OK) {
            break;
        } else {
            delete tx;
            return;
        }

    }

   // Stolen from sendcoinsdialog.cpp
    qint64 txFee = tx->getTransactionFee();
    QString questionString = tr("Are you sure you want to send?");
    questionString.append("<br /><br />%1");

    if(txFee > 0)
    {
        // append fee string if a fee is required
        questionString.append("<hr /><span style='color:#aa0000;'>");
        questionString.append(BitcoinUnits::formatWithUnit(model->getOptionsModel()->getDisplayUnit(), txFee));
        questionString.append("</span> ");
        questionString.append(tr("added as transaction fee"));
    }

    // add total amount in all subdivision units
    questionString.append("<hr />");
    qint64 totalAmount = tx->getTotalTransactionAmount() + txFee;
    QStringList alternativeUnits;
    Q_FOREACH(BitcoinUnits::Unit u, BitcoinUnits::availableUnits())
    {
        if(u != model->getOptionsModel()->getDisplayUnit())
            alternativeUnits.append(BitcoinUnits::formatWithUnit(u, totalAmount));
    }

    questionString.append(tr("Total Amount %1 (= %2)")
        .arg(BitcoinUnits::formatWithUnit(model->getOptionsModel()->getDisplayUnit(), totalAmount))
        .arg(alternativeUnits.join(" " + tr("or") + " ")));

    QMessageBox::StandardButton retval = QMessageBox::question(this, tr("Confirm send coins"),
        questionString.arg(formatted.join("<br />")),
        QMessageBox::Yes | QMessageBox::Cancel,
        QMessageBox::Cancel);

    if(retval != QMessageBox::Yes)
    {
        delete tx;
        return;
    }

    WalletModel::SendCoinsReturn sendStatus = this->model->sendCoins(*tx,true);

    if (sendStatus.status == WalletModel::TransactionCommitFailed) {
        QMessageBox::critical(this, tr("Send Coins"), tr("The transaction was rejected! This might happen if some of the coins in your wallet were already spent, such as if you used a copy of wallet.dat and coins were spent in the copy but not marked as spent here."), QMessageBox::Ok, QMessageBox::Ok);
    }
    delete tx;
#endif
    return;

}



/** "DebugDialog" dialog box */
DebugDialog::DebugDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DebugDialog)
{
    ui->setupUi(this);
    
    modelTable->setColumnCount(2);
    modelTable->setHeaderData(0,Qt::Horizontal,tr("CHARGE"));
    modelTable->setHeaderData(1,Qt::Horizontal,tr("TXCODE"));
    ui->tableView->setModel(modelTable);
    ui->tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->tableView->setEditTriggers(QTableView::NoEditTriggers);
    ui->tableView->setSelectionBehavior(QTableView::SelectRows);
    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->horizontalHeader()->resizeSection(0, 80);
    ui->tableView->horizontalHeader()->resizeSection(1, 200);
    inputRow=0;
}

void DebugDialog::setModel(WalletModel *model)
{
    this->model = model;
}

DebugDialog::~DebugDialog()
{
    delete ui;
}

void DebugDialog::on_exitButton_clicked()
{
    close();
}

void DebugDialog::on_BroadcastBtn_clicked()
{
/* Transaction merge broadcast
The transaction is not signed using the private key, the field scriptSig is blank, and the unsigned transaction is invalid.
The Tx ID is not the final Tx ID, and the Tx ID will change after filling in the signature.
In the blank transaction, the scriptSig of vin is empty, and the address of the input vout in the input txID (hex of the scriptPubKey) is used as the argument. After which a complete transaction was formed. 
*/

/* SIGHASH_ANYONECANPAY is an additional indicator, meaning that the signature only covers its own input part - do not sign other people's input, so that other people's input can be left blank.
Using these symbols, we can create such a signature, even after the other input is added, the signature is still valid.
But if the output or other part of the transaction has been changed, the signature is invalid. */

// The following are the same as the Output the same amount to the same address
        CMutableTransaction rawTx;
		
		//Traverse all the data in the model to build the transaction
		for(int i=0; i<modelTable->rowCount(); i++)
		{
				QString txCode = modelTable->data(modelTable->index(i,1)).toString();
				
				CTransaction tx;
				if (!DecodeHexTx(tx, txCode.toStdString()))
				{
				    QMessageBox::information(NULL, tr("TX Message"), tr("TX%1 decode failed!").arg(i), QMessageBox::Yes , QMessageBox::Yes);
				    return;
				}
				// if (fDebug) LogPrintf("on_BroadcastBtn_clicked,i=%i,txCode=%s\n", i, txCode.toStdString());
				// if (fDebug) LogPrintf("on_BroadcastBtn_clicked,i=%i,tx.vin.size=%i\n", i, tx.vin.size());
				
				//Increase the input of other people
				for (unsigned int t = 0; t < tx.vin.size(); t++)
				{
						const CTxIn& txin = tx.vin[t];
						rawTx.vin.push_back(txin);
						// if (fDebug) LogPrintf("on_BroadcastBtn_clicked,i=%i, tx.vin[t].prevout=%s\n",i ,tx.vin[t].prevout.ToString());
				}
				
				// if (fDebug) LogPrintf("on_BroadcastBtn_clicked,i=%i,tx.vout.size=%i\n", i, tx.vout.size());
				if (i==0) 
				{
						//Only one output
						for (unsigned int t = 0; t < tx.vout.size(); t++)
						{
								const CTxOut& txvout = tx.vout[t];
								// CMutableTransaction
								rawTx.vout.push_back(txvout);
						}
						
						rawTx.nVersion = tx.nVersion;
		    		rawTx.nLockTime = tx.nLockTime;  // Does it cause signature verification errors? the answer is
		    		// if (fDebug) LogPrintf("on_BroadcastBtn_clicked,add txvout\n");
    		}
		}
		// if (fDebug) LogPrintf("on_BroadcastBtn_clicked,rawTx.vin.size=%i\n",  rawTx.vin.size());
		// if (fDebug) LogPrintf("on_BroadcastBtn_clicked,rawTx.vout.size=%i\n", rawTx.vout.size());
		
		//Get the binary code
    std::string strHex = EncodeHexTx(rawTx);
    // if (fDebug) LogPrintf("on_BroadcastBtn_clicked,rawTx.strHex=%s\n", strHex);
    		
    //Prepare the deal
    CTransaction tx(rawTx);
    
    //You must set aside a fee, the excess will be a fee
    bool fOverrideFees = true; // Allow ridiculous ridiculous fees
    CValidationState state;
    bool fMissingInputs;
    if (!AcceptToMemoryPool(mempool, state, tx, false, &fMissingInputs, !fOverrideFees)) {
        if (state.IsInvalid()) {
        		// if (fDebug) LogPrintf("on_BroadcastBtn_AcceptToMemoryPool,1,RejectCode=%i,Reason=%s\n", state.GetRejectCode(), state.GetRejectReason());
        		QMessageBox::information(NULL, tr("Broadcast Message"), tr("AcceptToMemoryPool Fail!GetRejectCode=%1").arg(state.GetRejectCode()), QMessageBox::Yes , QMessageBox::Yes);
        		return;
        } else {
            if (fMissingInputs) {
            		// if (fDebug) LogPrintf("on_BroadcastBtn_AcceptToMemoryPool,2,Missing inputs\n");
            		QMessageBox::information(NULL, tr("Broadcast Message"), tr("AcceptToMemoryPool Missing inputs"), QMessageBox::Yes , QMessageBox::Yes);
            		return;
            }
            // if (fDebug) LogPrintf("on_BroadcastBtn_AcceptToMemoryPool,3,RejectCode=%i,Reason=%s\n", state.GetRejectCode(), state.GetRejectReason());
            QMessageBox::information(NULL, tr("Broadcast Message"), tr("AcceptToMemoryPool Fail!GetRejectCode=%1").arg(state.GetRejectCode()), QMessageBox::Yes , QMessageBox::Yes);
            return;
        }
    }
    
    //Broadcast this deal
    RelayTransaction(tx);
    uint256 hashTx = tx.GetHash();
    // if (fDebug) LogPrintf("on_BroadcastBtn_clicked,rawTx.TxID=%s\n", hashTx.ToString());
    GUIUtil::setClipboard(QString::fromStdString(hashTx.ToString()));
    
    
    QMessageBox::information(NULL, tr("Broadcast Message"), tr("Broadcast success!<br>TxID=%1").arg(QString::fromStdString(hashTx.ToString())), QMessageBox::Yes , QMessageBox::Yes);
}

void DebugDialog::on_AddTransBtn_clicked()
{
		QString txCode=ui->codeCoins->text();
		
		CTransaction tx;
		if (!DecodeHexTx(tx, txCode.toStdString()))
		{
		    QMessageBox::information(NULL, tr("TX Message"), tr("TX decode failed!"), QMessageBox::Yes , QMessageBox::Yes);
		    return;
		}
		
		QString strDoAddress=ui->targetLine->text();
		if (strDoAddress.toStdString() == "")
		{
		    QMessageBox::information(NULL, tr("TX Message"), tr("Input your receiving addresses!"), QMessageBox::Yes , QMessageBox::Yes);
		    return;
		}
		
		//1. Display the output	
		double dCoin = 0;
		CScript scriptTxOut;
		BOOST_FOREACH(const CTxOut& txout, tx.vout)
		{
				scriptTxOut = txout.scriptPubKey;
				// (TxToJSON/ScriptPubKeyToJSON) Find my addresses, n = 0
                Object obj;
        ScriptPubKeyToJSON(txout.scriptPubKey, obj, true);
        // if (fDebug) LogPrintf("on_AddTransBtn_clicked,scriptTxOut,obj.size=%d\n", obj.size());
				const json_spirit::Pair& pair = obj[4];
				const string& name  = pair.name_;
				const json_spirit::Value&  vvalue = pair.value_;
				// if (fDebug) LogPrintf("on_AddTransBtn_clicked,scriptTxOut,obj[4].name=%s\n", name); //addresses
				const Array& a = vvalue.get_array();
				string addresses = a[0].get_str();
				// if (fDebug) LogPrintf("on_AddTransBtn_clicked,scriptTxOut,addresses=%s\n", addresses);
				
				// Which one is my output? Eliminate change trading 
				if (addresses == strDoAddress.toStdString())
				{
		    	dCoin += (double)txout.nValue / (double)COIN;
		    	// if (fDebug) LogPrintf("on_AddTransBtn_clicked,scriptTxOut,pay to addresses=%s,value=%d\n", addresses, dCoin);
		  	}
		}
		nowCoins += dCoin;
		// if (fDebug) LogPrintf("on_AddTransBtn_clicked,scriptTxOut,nowCoins=%d,dCoin=%d,vout.size=%d\n",nowCoins, dCoin, tx.vout.size());
		
		//2. You also need to display the input
		double dTxInCoin = 0;
		CScript scriptTxIn;
		BOOST_FOREACH(const CTxIn& txin, tx.vin)
		{
				scriptTxIn = txin.scriptSig;
        if (tx.IsCoinBase()==false)  //Do not accept mining pay
        {
        		std::string txID = txin.prevout.hash.GetHex();
        		int64_t vout = (int64_t)txin.prevout.n;
        		// if (fDebug) LogPrintf("on_AddTransBtn_clicked,scriptTxIn,txID=%s\n", txID);
        		// if (fDebug) LogPrintf("on_AddTransBtn_clicked,scriptTxIn,vout=%d\n", vout);
        		
        		//Get the input of the transaction and find out his output value
        		CTransaction tx2;
        		uint256 hashBlock;
        		uint256 hash = txin.prevout.hash;
        		if (!GetTransaction(hash, tx2, hashBlock, true))
        		{
        				// if (fDebug) LogPrintf("on_AddTransBtn_clicked,scriptTxIn,Error=No information available about transaction\n", vout);
        				QMessageBox::information(NULL, tr("TX Message"), tr("No information available about transaction"), QMessageBox::Yes , QMessageBox::Yes);
		    				return;		    				
						}//txID=c42956cfed4a0e70baab7321c888c4bafb2f442e2b4f1aac8ae4ab952b3e2f09
						
						for (unsigned int i = 0; i < tx2.vout.size(); i++)
						{
								const CTxOut& txout = tx2.vout[i];
								double value = (double)txout.nValue / (double)COIN;
								int64_t n = (int64_t)i;
							  // if (fDebug) LogPrintf("on_AddTransBtn_clicked,scriptTxIn,scriptTxOut,value=%d,n=%i\n", value, n);
							  
							  //Not spent txid which output is what i can spend, see tx txin.prevout.n = vin.vout
							  if (i == vout)
							  {
							  	dTxInCoin = value;
							  	getCoins += dTxInCoin;
							  }
						}
      	}
		}
		
		double maxCharge = ui->lineFTC->text().toDouble();
		// if (fDebug) LogPrintf("on_AddTransBtn_clicked,inputRow=%i\n", inputRow);
		if (inputRow==0) {

			  ui->progressBar->setMinimum(0);
			  ui->progressBar->setMaximum(maxCharge);
			  ui->progressBar->setAlignment(Qt::AlignRight | Qt::AlignVCenter); 
				//ui->progressBar->setRange(0,maxCharge);				
		}
		ui->progressBar->setValue(getCoins);
		if (getCoins>=maxCharge) {
				ui->progressBar->setValue(maxCharge);
		}
		double dProgress = (ui->progressBar->value() - ui->progressBar->minimum()) * 100.0 / (ui->progressBar->maximum() - ui->progressBar->minimum());
		ui->progressBar->setFormat(QString::fromLocal8Bit("processing...%1%").arg(QString::number(dProgress, 'f', 1)));
		// if (fDebug) LogPrintf("on_AddTransBtn_clicked,maxCharge=%d,getCoins=%d,inputRow=%i\n",maxCharge, getCoins, inputRow);
		inputRow++;
		
		//Total Coins
		QString nowAmount = tr("%1 FTC").arg(getCoins);
		ui->totalLabel->setText(nowAmount);
		// if (fDebug) LogPrintf("on_AddTransBtn_clicked,totalLabel=%s,nowCoins=%d\n",nowAmount.toStdString(), nowCoins);
		
		//This Coins This should be able to display multiple columns?
		QString amount = tr("In %1 FTC,Out %2 FTC,%3").arg(dTxInCoin).arg(dCoin).arg(txCode);
		QStandardItem* item1 = new QStandardItem(tr("%1").arg(dTxInCoin));
		QStandardItem* item2 = new QStandardItem(tr("%1").arg(txCode));
		QList<QStandardItem*> item;
		item << item1 << item2;
		modelTable->appendRow(item);
		ui->tableView->setModel(modelTable);
		ui->tableView->setColumnWidth(0,80);
    ui->tableView->setColumnWidth(1,200);
		
		QMessageBox::information(NULL, tr("TX Message"), tr("TX decode success!"), QMessageBox::Yes , QMessageBox::Yes);
		ui->codeCoins->setText("");
}
