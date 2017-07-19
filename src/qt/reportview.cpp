#include "reportview.h"

#include "addresstablemodel.h"
#include "bitcoinunits.h"
#include "csvmodelwriter.h"
#include "editaddressdialog.h"
#include "guiutil.h"
#include "optionsmodel.h"
#include "transactiondescdialog.h"
#include "transactionfilterproxy.h"
#include "transactionrecord.h"
#include "transactiontablemodel.h"
#include "walletmodel.h"

#include "ui_interface.h"

#include <QScrollBar>
#include <QComboBox>
#include <QDoubleValidator>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QTableView>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QMessageBox>
#include <QPoint>
#include <QMenu>
#include <QLabel>
#include <QDateTimeEdit>

ReportView::ReportView(QWidget *parent) :
    QWidget(parent), model(0), transactionProxyModel(0),
    reportView(0)
{
    // Build filter row
    setContentsMargins(0,0,0,0);

    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->setContentsMargins(0,0,0,0);
#ifdef Q_OS_MAC
    hlayout->setSpacing(5);
    hlayout->addSpacing(26);
#else
    hlayout->setSpacing(0);
    hlayout->addSpacing(23);
#endif

    dateWidget = new QComboBox(this);
#ifdef Q_OS_MAC
    dateWidget->setFixedWidth(121);
#else
    dateWidget->setFixedWidth(120);
#endif
    dateWidget->addItem(tr("All"), All);
    dateWidget->addItem(tr("Today"), Today);
    dateWidget->addItem(tr("Yesterday"), Yesterday);
    dateWidget->addItem(tr("This week"), ThisWeek);
    dateWidget->addItem(tr("Last week"), LastWeek);
    dateWidget->addItem(tr("This month"), ThisMonth);
    dateWidget->addItem(tr("Last month"), LastMonth);
    dateWidget->addItem(tr("This year"), ThisYear);
    dateWidget->addItem(tr("Range..."), Range);
    hlayout->addWidget(dateWidget);

    typeWidget = new QComboBox(this);
#ifdef Q_OS_MAC
    typeWidget->setFixedWidth(121);
#else
    typeWidget->setFixedWidth(120);
#endif

    typeWidget->addItem(tr("All"), TransactionFilterProxy::ALL_TYPES);
    typeWidget->addItem(tr("Received with"), TransactionFilterProxy::TYPE(TransactionRecord::RecvWithAddress) |
                                        TransactionFilterProxy::TYPE(TransactionRecord::RecvFromOther));
    typeWidget->addItem(tr("Sent to"), TransactionFilterProxy::TYPE(TransactionRecord::SendToAddress) |
                                  TransactionFilterProxy::TYPE(TransactionRecord::SendToOther));
    typeWidget->addItem(tr("To yourself"), TransactionFilterProxy::TYPE(TransactionRecord::SendToSelf));
    typeWidget->addItem(tr("Mined"), TransactionFilterProxy::TYPE(TransactionRecord::Generated));
    typeWidget->addItem(tr("Other"), TransactionFilterProxy::TYPE(TransactionRecord::Other));

    hlayout->addWidget(typeWidget);

    addressWidget = new QLineEdit(this);
#if QT_VERSION >= 0x040700
    /* Do not move this to the XML file, Qt before 4.7 will choke on it */
    addressWidget->setPlaceholderText(tr("Enter address or label to search"));
#endif
    hlayout->addWidget(addressWidget);

    amountWidget = new QLineEdit(this);
#if QT_VERSION >= 0x040700
    /* Do not move this to the XML file, Qt before 4.7 will choke on it */
    amountWidget->setPlaceholderText(tr("Min amount"));
#endif
#ifdef Q_OS_MAC
    amountWidget->setFixedWidth(97);
#else
    amountWidget->setFixedWidth(100);
#endif
    amountWidget->setValidator(new QDoubleValidator(0, 1e20, 8, this));
    hlayout->addWidget(amountWidget);

    QVBoxLayout *vlayout = new QVBoxLayout(this);
    vlayout->setContentsMargins(0,0,0,0);
    vlayout->setSpacing(0);

    QTableView *view = new QTableView(this);
    vlayout->addLayout(hlayout);
    vlayout->addWidget(createDateRangeWidget());
    vlayout->addWidget(view);
    
    QTableView *viewT2 = new QTableView(this);
    viewT2->setFixedHeight(60);
		vlayout->addWidget(viewT2);
    QTableView *viewT = new QTableView(this);
    viewT->setFixedHeight(30);
		vlayout->addWidget(viewT);		
		
    vlayout->setSpacing(0);    
    int width = view->verticalScrollBar()->sizeHint().width();
     //Cover scroll bar width with spacing
#ifdef Q_OS_MAC
    hlayout->addSpacing(width+2);
#else
    hlayout->addSpacing(width);
#endif
    // Always show scroll bar
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    view->setTabKeyNavigation(false);
    view->setContextMenuPolicy(Qt::CustomContextMenu);
    reportView = view;
    reportViewT = viewT;
    reportViewT2 = viewT2;

    // Actions
    QAction *showTotalAction = new QAction(tr("Show transaction total"), this);

    contextMenu = new QMenu();
    contextMenu->addAction(showTotalAction);
    
    // Connect actions
    connect(dateWidget, SIGNAL(activated(int)), this, SLOT(chooseDate(int)));
    connect(typeWidget, SIGNAL(activated(int)), this, SLOT(chooseType(int)));
    connect(addressWidget, SIGNAL(textChanged(QString)), this, SLOT(changedPrefix(QString)));
    connect(amountWidget, SIGNAL(textChanged(QString)), this, SLOT(changedAmount(QString)));

    connect(showTotalAction, SIGNAL(triggered()), this, SLOT(showTotal()));
}

void ReportView::setModel(WalletModel *model)
{
    this->model = model;
    if(model)
    {
        transactionProxyModel = new TransactionFilterProxy(this);
        transactionProxyModel->setSourceModel(model->getTransactionTableModel());
        transactionProxyModel->setDynamicSortFilter(true);
        transactionProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
        transactionProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
        transactionProxyModel->setSortRole(Qt::EditRole);
        
        reportModel = new QStandardItemModel(this);
        reportModel->setSortRole(Qt::EditRole);
        reportView->setModel(reportModel);
        reportView->setAlternatingRowColors(true);
        reportView->setSelectionBehavior(QAbstractItemView::SelectRows);
        reportView->setSelectionMode(QAbstractItemView::ExtendedSelection);
        reportView->setSortingEnabled(true);
        reportView->sortByColumn(0, Qt::DescendingOrder);
        reportView->verticalHeader()->hide();
        reportView->setShowGrid(false);        
        	
        reportModelT = new QStandardItemModel(this);
        reportViewT->setModel(reportModelT);
        reportViewT->setAlternatingRowColors(true);
        reportViewT->setSelectionBehavior(QAbstractItemView::SelectRows);
        reportViewT->setSelectionMode(QAbstractItemView::ExtendedSelection);
        reportViewT->verticalHeader()->hide();
        reportViewT->setShowGrid(false); 
        
        reportModelT2 = new QStandardItemModel(this);
        reportViewT2->setModel(reportModelT2);
        reportViewT2->setAlternatingRowColors(true);
        reportViewT2->setSelectionBehavior(QAbstractItemView::SelectRows);
        reportViewT2->setSelectionMode(QAbstractItemView::ExtendedSelection);
        reportViewT2->verticalHeader()->hide();
        reportViewT2->setShowGrid(false); 
        
        connect(reportView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), this, SLOT(showTotal()));         
        showTotal();
    }
}

void ReportView::chooseDate(int idx)
{
    if(!transactionProxyModel)
        return;
    QDate current = QDate::currentDate();
    dateRangeWidget->setVisible(false);
    switch(dateWidget->itemData(idx).toInt())
    {
    case All:
        transactionProxyModel->setDateRange(
                TransactionFilterProxy::MIN_DATE,
                TransactionFilterProxy::MAX_DATE);
        break;
    case Today:
        transactionProxyModel->setDateRange(
                QDateTime(current),
                TransactionFilterProxy::MAX_DATE);
        break;
    case Yesterday:{
    	  QDate startOfDay = current.addDays(-1);
        transactionProxyModel->setDateRange(
                QDateTime(startOfDay),
                QDateTime(current));
        } break;
    case ThisWeek: {
        // Find last Monday
        QDate startOfWeek = current.addDays(-(current.dayOfWeek()-1));
        transactionProxyModel->setDateRange(
                QDateTime(startOfWeek),
                TransactionFilterProxy::MAX_DATE);
        } break;
    case LastWeek: {
    	  //from Monday to Sunday
        QDate startOfWeek = current.addDays(-(current.dayOfWeek()+6));
        QDate endOfWeek = current.addDays(-(current.dayOfWeek()-1));
        transactionProxyModel->setDateRange(
                QDateTime(startOfWeek),
                QDateTime(endOfWeek));
        } break;        
    case ThisMonth:
        transactionProxyModel->setDateRange(
                QDateTime(QDate(current.year(), current.month(), 1)),
                TransactionFilterProxy::MAX_DATE);
        break;
    case LastMonth:
        transactionProxyModel->setDateRange(
                QDateTime(QDate(current.year(), current.month()-1, 1)),
                QDateTime(QDate(current.year(), current.month(), 1)));
        break;
    case ThisYear:
        transactionProxyModel->setDateRange(
                QDateTime(QDate(current.year(), 1, 1)),
                TransactionFilterProxy::MAX_DATE);
        break;
    case Range:
        dateRangeWidget->setVisible(true);
        dateRangeChanged();
        break;
    }
    showTotal();
}

void ReportView::chooseType(int idx)
{
    if(!transactionProxyModel)
        return;
    transactionProxyModel->setTypeFilter(
        typeWidget->itemData(idx).toInt());
    showTotal();
}

void ReportView::changedPrefix(const QString &prefix)
{
    if(!transactionProxyModel)
        return;
    transactionProxyModel->setAddressPrefix(prefix);
    showTotal();
}

void ReportView::changedAmount(const QString &amount)
{
    if(!transactionProxyModel)
        return;
    int64_t amount_parsed = 0;
    if(BitcoinUnits::parse(model->getOptionsModel()->getDisplayUnit(), amount, &amount_parsed))
    {
        transactionProxyModel->setMinAmount(amount_parsed);
    }
    else
    {
        transactionProxyModel->setMinAmount(0);
    }
    showTotal();
}

void ReportView::exportClicked()
{
    // CSV is currently the only supported format
    QString filename = GUIUtil::getSaveFileName(this,
        tr("Export Transaction Report"), QString(),
        tr("Comma separated file (*.csv)"), NULL);

    if (filename.isNull()) return;

    CSVModelWriter writer(filename);

    // name, column, role
    writer.setModel(reportModel);
    writer.addColumn(tr("AddressLabel"), 0, Qt::DisplayRole);
    writer.addColumn(tr("DateRange"), 1, Qt::DisplayRole);
    writer.addColumn(tr("Type"), 2, Qt::EditRole);
    writer.addColumn(tr("Payment amount"), 3, Qt::EditRole);
    writer.addColumn(tr("Number of payments"), 4, Qt::EditRole);

    if(!writer.write()) {
        Q_EMIT message(tr("Exporting Failed"), tr("There was an error trying to save the transaction history to %1.").arg(filename),
            CClientUIInterface::MSG_ERROR);
    }
    else {
        Q_EMIT message(tr("Exporting Successful"), tr("The transaction history was successfully saved to %1.").arg(filename),
            CClientUIInterface::MSG_INFORMATION);
    }
}

void ReportView::showTotal()
{
		if(!model)
			 return;
			 
		int unit = model->getOptionsModel()->getDisplayUnit();
	  qint64 fTotal=0;
	  qint64 iTotal=0;
	  int i=0;
	  QString addressname = "";
	  QString typelistname = "";
	  QVector<QString> addresslist;
	  QVector<QString> typelist;
	  QVector<qint64> totallist;
	  QVector<double> timelist;
	  QVector<qint64> totallist2;
	  QVector<double> timelist2;
	  
	  //accountReport from every address
	  for (i=0;i<=transactionProxyModel->rowCount()-1;i++)
	  {
	  	//ColumnIndex
	  	fTotal+=transactionProxyModel->data(transactionProxyModel->index(i,5),TransactionTableModel::AmountRole).toLongLong();	
	  	iTotal=transactionProxyModel->data(transactionProxyModel->index(i,5),TransactionTableModel::AmountRole).toLongLong();	  	
	  	addressname=transactionProxyModel->data(transactionProxyModel->index(i,4)).toString();
	  	typelistname=transactionProxyModel->data(transactionProxyModel->index(i,3)).toString();
	  	
	  	int t = addresslist.indexOf(addressname);
	  	if (t != -1)
	  		{
	  			totallist[t]=totallist.at(t)+iTotal;
	  			timelist[t]=timelist.at(t)+1;
	  		}
	  	else
	  		{
						addresslist.append(addressname);
						totallist.append(iTotal);
						timelist.append(1);  		   
	  		}
	  		
	  	int t2 = typelist.indexOf(typelistname);
	  	if (t2 != -1)
	  		{
	  			totallist2[t2]=totallist2.at(t2)+iTotal;
	  			timelist2[t2]=timelist2.at(t2)+1;
	  		}
	  	else
	  		{
						typelist.append(typelistname);
						totallist2.append(iTotal);
						timelist2.append(1);
	  		}
	  }
	  int iTimes=i;
	  
	  //Total Address
	  reportModel->clear();
	  reportModel->setRowCount(addresslist.size());
	  reportModel->setSortRole(Qt::EditRole);
    reportModel->setHorizontalHeaderItem(0, new QStandardItem(tr("AddressLabel")));
    reportModel->setHorizontalHeaderItem(1, new QStandardItem(tr("DateRange")));
   	reportModel->setHorizontalHeaderItem(2, new QStandardItem(tr("Type")));
    reportModel->setHorizontalHeaderItem(3, new QStandardItem(tr("Payment amount")));
    reportModel->setHorizontalHeaderItem(4, new QStandardItem(tr("Number of payments")));
    reportView->horizontalHeader()->resizeSection(0, 270);
    reportView->horizontalHeader()->resizeSection(1, 120);
    reportView->horizontalHeader()->resizeSection(2, 120);
    reportView->horizontalHeader()->resizeSection(3, 140);
    reportView->horizontalHeader()->resizeSection(4, 140);
    for (i=0;i!=addresslist.size();++i)
    { 
    	reportModel->setItem(i,0,new QStandardItem(addresslist.at(i)));
    	reportModel->setItem(i,1,new QStandardItem(dateWidget->currentText()));
    	reportModel->setItem(i,2,new QStandardItem(typeWidget->currentText()));
    	
    	QString amountText = BitcoinUnits::format(unit, totallist.at(i), false, BitcoinUnits::separatorAlways);
    	reportModel->setItem(i,3,new QStandardItem(amountText));
    	
      reportModel->setItem(i,4,new QStandardItem(QObject::tr("%1").arg(timelist.at(i))));
    }
    
    //Total Type
    reportModelT2->clear();
	  reportModelT2->setRowCount(typelist.size());
    reportModelT2->setHorizontalHeaderItem(0, new QStandardItem(tr("AddressLabel")));
    reportModelT2->setHorizontalHeaderItem(1, new QStandardItem(tr("DateRange")));
   	reportModelT2->setHorizontalHeaderItem(2, new QStandardItem(tr("Type")));
    reportModelT2->setHorizontalHeaderItem(3, new QStandardItem(tr("Payment amount")));
    reportModelT2->setHorizontalHeaderItem(4, new QStandardItem(tr("Number of payments")));
    reportViewT2->horizontalHeader()->resizeSection(0, 270);
    reportViewT2->horizontalHeader()->resizeSection(1, 120);
    reportViewT2->horizontalHeader()->resizeSection(2, 120);
    reportViewT2->horizontalHeader()->resizeSection(3, 140);
    reportViewT2->horizontalHeader()->resizeSection(4, 140);
    for (i=0;i!=typelist.size();++i)
    { 
    	reportModelT2->setItem(i,0,new QStandardItem(typelist.at(i)));
    	reportModelT2->setItem(i,1,new QStandardItem(dateWidget->currentText()));
    	reportModelT2->setItem(i,2,new QStandardItem(typeWidget->currentText()));
    	    	
    	QString amountText = BitcoinUnits::format(unit, totallist2.at(i), false, BitcoinUnits::separatorAlways);
    	reportModelT2->setItem(i,3,new QStandardItem(amountText));
    	
      reportModelT2->setItem(i,4,new QStandardItem(QObject::tr("%1").arg(timelist2.at(i))));
      	
      reportModelT2->item(i,0)->setTextAlignment(Qt::AlignCenter);
      reportModelT2->item(i,1)->setTextAlignment(Qt::AlignCenter);
      reportModelT2->item(i,2)->setTextAlignment(Qt::AlignCenter);
      reportModelT2->item(i,3)->setTextAlignment(Qt::AlignCenter);
      reportModelT2->item(i,4)->setTextAlignment(Qt::AlignCenter);
    }
    reportViewT2->horizontalHeader()->hide();
    
    
    //Total Line
    reportModelT->clear();
    reportModelT->setRowCount(1); 
    reportModelT->setHorizontalHeaderItem(0, new QStandardItem(tr("AddressLabel")));
    reportModelT->setHorizontalHeaderItem(1, new QStandardItem(tr("DateRange")));
   	reportModelT->setHorizontalHeaderItem(2, new QStandardItem(tr("Type")));
    reportModelT->setHorizontalHeaderItem(3, new QStandardItem(tr("Payment amount")));
    reportModelT->setHorizontalHeaderItem(4, new QStandardItem(tr("Number of payments")));
    reportViewT->horizontalHeader()->resizeSection(0, 270);
    reportViewT->horizontalHeader()->resizeSection(1, 120);
    reportViewT->horizontalHeader()->resizeSection(2, 120);
    reportViewT->horizontalHeader()->resizeSection(3, 140);
    reportViewT->horizontalHeader()->resizeSection(4, 140);
  	reportModelT->setItem(0,0,new QStandardItem(tr("Total")));
  	reportModelT->setItem(0,1,new QStandardItem(dateWidget->currentText()));
  	reportModelT->setItem(0,2,new QStandardItem(typeWidget->currentText()));  	
  	QString amountText = BitcoinUnits::format(unit, fTotal, false, BitcoinUnits::separatorAlways);
  	reportModelT->setItem(0,3,new QStandardItem(amountText));
    reportModelT->setItem(0,4,new QStandardItem(QObject::tr("%1").arg(iTimes)));    
    reportModelT->item(0,0)->setTextAlignment(Qt::AlignCenter);
    reportModelT->item(0,1)->setTextAlignment(Qt::AlignCenter);
    reportModelT->item(0,2)->setTextAlignment(Qt::AlignCenter);
    reportModelT->item(0,3)->setTextAlignment(Qt::AlignCenter);
    reportModelT->item(0,4)->setTextAlignment(Qt::AlignCenter);
    reportModelT->item(0,0)->setFont(QFont("Times", 10, QFont::Black));
    reportModelT->item(0,1)->setFont(QFont("Times", 10, QFont::Black));
    reportModelT->item(0,2)->setFont(QFont("Times", 10, QFont::Black));
    reportModelT->item(0,3)->setFont(QFont("Times", 10, QFont::Black));
    reportModelT->item(0,4)->setFont(QFont("Times", 10, QFont::Black));
    reportViewT->horizontalHeader()->hide();
}

QWidget *ReportView::createDateRangeWidget()
{
    dateRangeWidget = new QFrame();
    dateRangeWidget->setFrameStyle(QFrame::Panel | QFrame::Raised);
    dateRangeWidget->setContentsMargins(1,1,1,1);
    QHBoxLayout *layout = new QHBoxLayout(dateRangeWidget);
    layout->setContentsMargins(0,0,0,0);
    layout->addSpacing(23);
    layout->addWidget(new QLabel(tr("Range:")));

    dateFrom = new QDateTimeEdit(this);
    dateFrom->setDisplayFormat("dd/MM/yy");
    dateFrom->setCalendarPopup(true);
    dateFrom->setMinimumWidth(100);
    dateFrom->setDate(QDate::currentDate().addDays(-7));
    layout->addWidget(dateFrom);
    layout->addWidget(new QLabel(tr("to")));

    dateTo = new QDateTimeEdit(this);
    dateTo->setDisplayFormat("dd/MM/yy");
    dateTo->setCalendarPopup(true);
    dateTo->setMinimumWidth(100);
    dateTo->setDate(QDate::currentDate());
    layout->addWidget(dateTo);
    layout->addStretch();

    // Hide by default
    dateRangeWidget->setVisible(false);

    // Notify on change
    connect(dateFrom, SIGNAL(dateChanged(QDate)), this, SLOT(dateRangeChanged()));
    connect(dateTo, SIGNAL(dateChanged(QDate)), this, SLOT(dateRangeChanged()));

    return dateRangeWidget;
}

void ReportView::dateRangeChanged()
{
    if(!transactionProxyModel)
        return;
    transactionProxyModel->setDateRange(
            QDateTime(dateFrom->date()),
            QDateTime(dateTo->date()).addDays(1));
    showTotal();
}

void ReportView::focusTransaction(const QModelIndex &idx)
{
    if(!transactionProxyModel)
        return;
    QModelIndex targetIdx = transactionProxyModel->mapFromSource(idx);
    reportView->scrollTo(targetIdx);
    reportView->setCurrentIndex(targetIdx);
    reportView->setFocus();
}
