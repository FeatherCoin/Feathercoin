// Copyright (c) 2013-2014 The Feathercoin developers
#ifndef REPORTVIEW_H
#define REPORTVIEW_H

#include "guiutil.h"

#include <QWidget>

class WalletModel;
class TransactionFilterProxy;

QT_BEGIN_NAMESPACE
class QTableView;
class QStandardItemModel;
class QComboBox;
class QLineEdit;
class QModelIndex;
class QMenu;
class QFrame;
class QDateTimeEdit;
class QLabel;
QT_END_NAMESPACE

/** Widget showing the transaction list for a wallet, including a filter row.
    Using the filter row, the user can view or export a subset of the transactions.
  */
class ReportView : public QWidget
{
    Q_OBJECT

public:
    explicit ReportView(QWidget *parent = 0);

    void setModel(WalletModel *model);

    // Date ranges for filter
    enum DateEnum
    {
        All,
        Today,
        Yesterday,
        ThisWeek,
        LastWeek,
        ThisMonth,
        LastMonth,
        ThisYear,
        Range
    };

private:
    WalletModel *model;
    TransactionFilterProxy *transactionProxyModel;
    QTableView *reportView;
    QStandardItemModel *reportModel;

    QComboBox *dateWidget;
    QComboBox *typeWidget;
    QLineEdit *addressWidget;
    QLineEdit *amountWidget;
    QLabel *totalnumslWidget;
    QLabel *totaltimesWidget;
    QLabel *rptTotal;  
    QMenu *contextMenu;

    QFrame *dateRangeWidget;
    QDateTimeEdit *dateFrom;
    QDateTimeEdit *dateTo;

    QWidget *createDateRangeWidget();

private slots:
    void dateRangeChanged();
    void showTotal();

signals:
    //void doubleClicked(const QModelIndex&);

    /**  Fired when a message should be reported to the user */
    void message(const QString &title, const QString &message, unsigned int style);


public slots:
    void chooseDate(int idx);
    void chooseType(int idx);
    void changedPrefix(const QString &prefix);
    void changedAmount(const QString &amount);
    void exportClicked();
    void focusTransaction(const QModelIndex&);

};

#endif // REPORTVIEW_H