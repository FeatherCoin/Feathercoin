#ifndef OVERVIEWPAGE_H
#define OVERVIEWPAGE_H

#include <QWidget>
#include <QAbstractItemDelegate>
#include <QPainter>

#include "clientmodel.h"
#include "walletmodel.h"
#include "bitcoinunits.h"
#include "optionsmodel.h"
#include "transactiontablemodel.h"
#include "transactionfilterproxy.h"
#include "guiutil.h"
#include "guiconstants.h"

#define DECORATION_SIZE 64
#define NUM_ITEMS 3

namespace Ui {
    class OverviewPage;
}
class ClientModel;
class WalletModel;
class TxViewDelegate;
class TransactionFilterProxy;

QT_BEGIN_NAMESPACE
class QModelIndex;
QT_END_NAMESPACE

/** Overview ("home") page widget */
class OverviewPage : public QWidget
{
    Q_OBJECT

public:
    explicit OverviewPage(QWidget *parent = 0);
    ~OverviewPage();

    void setClientModel(ClientModel *clientModel);
    void setWalletModel(WalletModel *walletModel);
    void showOutOfSyncWarning(bool fShow);

public slots:
    void setBalance(qint64 balance, qint64 unconfirmedBalance, qint64 immatureBalance);

signals:
    void transactionClicked(const QModelIndex &index);

private:
    Ui::OverviewPage *ui;
    ClientModel *clientModel;
    WalletModel *walletModel;
    qint64 currentBalance;
    qint64 currentUnconfirmedBalance;
    qint64 currentImmatureBalance;

    TxViewDelegate *txdelegate;
    TransactionFilterProxy *filter;

private slots:
    void updateDisplayUnit();
    void handleTransactionClicked(const QModelIndex &index);
    void updateAlerts(const QString &warnings);
};



class TxViewDelegate : public QAbstractItemDelegate
{
   Q_OBJECT
public:
   TxViewDelegate() : QAbstractItemDelegate(), unit(BitcoinUnits::BTC)
   {

   }

   inline void paint(QPainter *painter, const QStyleOptionViewItem &option,
      const QModelIndex &index) const
   {
      painter->save();

      QIcon icon = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));
      QRect mainRect = option.rect;
      QRect decorationRect(mainRect.topLeft(), QSize(DECORATION_SIZE, DECORATION_SIZE));
      int xspace = DECORATION_SIZE + 8;
      int ypad = 6;
      int halfheight = (mainRect.height() - 2 * ypad) / 2;
      QRect amountRect(mainRect.left() + xspace, mainRect.top() + ypad, mainRect.width() - xspace, halfheight);
      QRect addressRect(mainRect.left() + xspace, mainRect.top() + ypad + halfheight, mainRect.width() - xspace, halfheight);
      icon.paint(painter, decorationRect);

      QDateTime date = index.data(TransactionTableModel::DateRole).toDateTime();
      QString address = index.data(Qt::DisplayRole).toString();
      qint64 amount = index.data(TransactionTableModel::AmountRole).toLongLong();
      bool confirmed = index.data(TransactionTableModel::ConfirmedRole).toBool();
      QVariant value = index.data(Qt::ForegroundRole);
      QColor foreground = option.palette.color(QPalette::Text);
      if (value.canConvert<QBrush>())
      {
         QBrush brush = qvariant_cast<QBrush>(value);
         foreground = brush.color();
      }

      painter->setPen(foreground);
      painter->drawText(addressRect, Qt::AlignLeft | Qt::AlignVCenter, address);

      if (amount < 0)
      {
         foreground = COLOR_NEGATIVE;
      }
      else if (!confirmed)
      {
         foreground = COLOR_UNCONFIRMED;
      }
      else
      {
         foreground = option.palette.color(QPalette::Text);
      }
      painter->setPen(foreground);
      QString amountText = BitcoinUnits::formatWithUnit(unit, amount, true);
      if (!confirmed)
      {
         amountText = QString("[") + amountText + QString("]");
      }
      painter->drawText(amountRect, Qt::AlignRight | Qt::AlignVCenter, amountText);

      painter->setPen(option.palette.color(QPalette::Text));
      painter->drawText(amountRect, Qt::AlignLeft | Qt::AlignVCenter, GUIUtil::dateTimeStr(date));

      painter->restore();
   }

   inline QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
   {
      return QSize(DECORATION_SIZE, DECORATION_SIZE);
   }

   int unit;

};


#endif // OVERVIEWPAGE_H
