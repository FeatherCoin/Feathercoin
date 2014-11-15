// Copyright (c) 2013-2014 The Feathercoin developers
// from overviewpage

#include "merchantlist.h"
#include "ui_merchantlist.h"

#include "bitcoinunits.h"
#include "clientmodel.h"
#include "guiconstants.h"
#include "guiutil.h"
#include "optionsmodel.h"
#include "transactionfilterproxy.h"
#include "transactiontablemodel.h"
#include "walletmodel.h"

#include <QAbstractItemDelegate>
#include <QPainter>
#include <QDesktopServices>
#include <QUrl>

#define DECORATION_SIZE 64
#define NUM_ITEMS 3

class MerViewDelegate : public QAbstractItemDelegate
{
    Q_OBJECT
public:
    MerViewDelegate(): QAbstractItemDelegate(), unit(BitcoinUnits::FTC)
    {

    }

    inline void paint(QPainter *painter, const QStyleOptionViewItem &option,
                      const QModelIndex &index ) const
    {
        painter->save();

        QIcon icon = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));
        QRect mainRect = option.rect;
        QRect decorationRect(mainRect.topLeft(), QSize(DECORATION_SIZE, DECORATION_SIZE));
        int xspace = DECORATION_SIZE + 8;
        int ypad = 6;
        int halfheight = (mainRect.height() - 2*ypad)/2;
        QRect amountRect(mainRect.left() + xspace, mainRect.top()+ypad, mainRect.width() - xspace, halfheight);
        QRect addressRect(mainRect.left() + xspace, mainRect.top()+ypad+halfheight, mainRect.width() - xspace, halfheight);
        icon.paint(painter, decorationRect);

        QDateTime date = index.data(TransactionTableModel::DateRole).toDateTime();
        QString address = index.data(Qt::DisplayRole).toString();
        qint64 amount = index.data(TransactionTableModel::AmountRole).toLongLong();
        bool confirmed = index.data(TransactionTableModel::ConfirmedRole).toBool();
        QVariant value = index.data(Qt::ForegroundRole);
        QColor foreground = option.palette.color(QPalette::Text);
        if(value.canConvert<QBrush>())
        {
            QBrush brush = qvariant_cast<QBrush>(value);
            foreground = brush.color();
        }

        painter->setPen(foreground);
        painter->drawText(addressRect, Qt::AlignLeft|Qt::AlignVCenter, address);

        if(amount < 0)
        {
            foreground = COLOR_NEGATIVE;
        }
        else if(!confirmed)
        {
            foreground = COLOR_UNCONFIRMED;
        }
        else
        {
            foreground = option.palette.color(QPalette::Text);
        }
        painter->setPen(foreground);
        QString amountText = BitcoinUnits::formatWithUnit(unit, amount, true);
        if(!confirmed)
        {
            amountText = QString("[") + amountText + QString("]");
        }
        painter->drawText(amountRect, Qt::AlignRight|Qt::AlignVCenter, amountText);

        painter->setPen(option.palette.color(QPalette::Text));
        painter->drawText(amountRect, Qt::AlignLeft|Qt::AlignVCenter, GUIUtil::dateTimeStr(date));

        painter->restore();
    }

    inline QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        return QSize(DECORATION_SIZE, DECORATION_SIZE);
    }

    int unit;

};
#include "merchantlist.moc"

MerchantListView::MerchantListView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MerchantListView),
    clientModel(0),
    walletModel(0),
    currentBalance(-1),
    currentUnconfirmedBalance(-1),
    currentImmatureBalance(-1),
    txdelegate(new MerViewDelegate()),
    filter(0)
{
    ui->setupUi(this);
    
    ui->label_Map->installEventFilter(this);
    ui->label_left_1->installEventFilter(this);
    ui->label_left_2->installEventFilter(this);
    ui->label_left_3->installEventFilter(this);
    ui->label_left_4->installEventFilter(this);
    ui->label_right_1->installEventFilter(this);
    ui->label_right_2->installEventFilter(this);
    ui->label_right_3->installEventFilter(this);
    ui->label_right_4->installEventFilter(this);
    ui->label_right_5->installEventFilter(this);
    ui->label_right_6->installEventFilter(this);
    ui->label_right_7->installEventFilter(this);
    
    // start with displaying the "out of sync" warnings
    showOutOfSyncWarning(true);
}

void MerchantListView::handleTransactionClicked(const QModelIndex &index)
{
    if(filter)
        emit transactionClicked(filter->mapToSource(index));
}

bool MerchantListView::eventFilter(QObject *obj, QEvent *event)
{
 	 if (event->type() == QEvent::MouseButtonPress) {
	 	 	if (obj == ui->label_Map) { 
	 	 	  QDesktopServices::openUrl(QUrl("http://map.ftc-c.com"));
	 			return true; 
	 		}
	 	 	if (obj == ui->label_left_1) { 
	 	 	  QDesktopServices::openUrl(QUrl("http://www.bitcoinbazaar.co.uk"));
	 			return true; 
	 		}
	 	 	if (obj == ui->label_left_2) { 
	 	 	  QDesktopServices::openUrl(QUrl("https://pock.io"));
	 			return true; 
	 		}
	 	 	if (obj == ui->label_left_3) { 
	 	 	  QDesktopServices::openUrl(QUrl("http://bitbooks.co"));
	 			return true; 
	 		}
	 	 	if (obj == ui->label_left_4) { 
	 	 	  QDesktopServices::openUrl(QUrl("http://coinverted.com"));
	 			return true; 
	 		}
	 	 	if (obj == ui->label_right_1) { 
	 	 	  QDesktopServices::openUrl(QUrl("https://crypto-trade.com/currencies/trade/ftc_usd"));
	 			return true; 
	 		}
	 		if (obj == ui->label_right_2) { 
	 	 	  QDesktopServices::openUrl(QUrl("https://btc-e.com/exchange/ftc_btc"));
	 			return true; 
	 		}
	 		if (obj == ui->label_right_3) { 
	 	 	  QDesktopServices::openUrl(QUrl("https://www.cryptsy.com/markets/view/5"));
	 			return true; 
	 		}
	 		if (obj == ui->label_right_4) { 
	 	 	  QDesktopServices::openUrl(QUrl("https://coinmkt.com"));
	 			return true; 
	 		}
	 		if (obj == ui->label_right_5) { 
	 	 	  QDesktopServices::openUrl(QUrl("https://bter.com/trade/ftc_cny"));
	 			return true; 
	 		}
	 		if (obj == ui->label_right_6) { 
	 	 	  QDesktopServices::openUrl(QUrl("https://mcxnow.com/exchange/FTC"));
	 			return true; 
	 		}
	 		if (obj == ui->label_right_7) { 
	 	 	  QDesktopServices::openUrl(QUrl("http://shapeshift.io"));
	 			return true; 
	 		}
 		}
 		return QWidget::eventFilter(obj, event);    
}

MerchantListView::~MerchantListView()
{
    delete ui;
}

void MerchantListView::setBalance(qint64 balance, qint64 unconfirmedBalance, qint64 immatureBalance)
{
    int unit = walletModel->getOptionsModel()->getDisplayUnit();
    int shareBalance=0; //Ready for POS
    currentBalance = balance;
    currentUnconfirmedBalance = unconfirmedBalance;
    currentImmatureBalance = immatureBalance;
}

void MerchantListView::setOverview()
{

}

void MerchantListView::setClientModel(ClientModel *model)
{
    this->clientModel = model;
    if(model)
    {
        // Show warning if this is a prerelease version
        connect(model, SIGNAL(alertsChanged(QString)), this, SLOT(updateAlerts(QString)));
        updateAlerts(model->getStatusBarWarnings());
    }
}

void MerchantListView::setWalletModel(WalletModel *model)
{
    this->walletModel = model;
    if(model && model->getOptionsModel())
    {
        // Set up transaction list
        filter = new TransactionFilterProxy();
        filter->setSourceModel(model->getTransactionTableModel());
        filter->setLimit(NUM_ITEMS);
        filter->setDynamicSortFilter(true);
        filter->setSortRole(Qt::EditRole);
        filter->setShowInactive(false);
        filter->sort(TransactionTableModel::Status, Qt::DescendingOrder);

        // Keep up to date with wallet
        setBalance(model->getBalance(), model->getUnconfirmedBalance(), model->getImmatureBalance());
        connect(model, SIGNAL(balanceChanged(qint64, qint64, qint64)), this, SLOT(setBalance(qint64, qint64, qint64)));

        connect(model->getOptionsModel(), SIGNAL(displayUnitChanged(int)), this, SLOT(updateDisplayUnit()));
    }

    // update the display unit, to not use the default ("FTC")
    updateDisplayUnit();
}

void MerchantListView::updateDisplayUnit()
{
    if(walletModel && walletModel->getOptionsModel())
    {
        if(currentBalance != -1)
            setBalance(currentBalance, currentUnconfirmedBalance, currentImmatureBalance);
            
        // Update txdelegate->unit with the current unit
        txdelegate->unit = walletModel->getOptionsModel()->getDisplayUnit();            
    }
}

void MerchantListView::updateAlerts(const QString &warnings)
{

}

void MerchantListView::showOutOfSyncWarning(bool fShow)
{

}
