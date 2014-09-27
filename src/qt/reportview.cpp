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
    qint64 amount_parsed = 0;
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
        emit message(tr("Exporting Failed"), tr("There was an error trying to save the transaction history to %1.").arg(filename),
            CClientUIInterface::MSG_ERROR);
    }
    else {
        emit message(tr("Exporting Successful"), tr("The transaction history was successfully saved to %1.").arg(filename),
            CClientUIInterface::MSG_INFORMATION);
    }
}

void ReportView::showTotal()
{
	  float fTotal=0;
	  float iTotal=0;
	  int i=0;
	  QString addressname = "";
	  QVector<QString> addresslist;
	  QVector<float> totallist;
	  QVector<float> inlist;
	  QVector<float> outlist;
	  QVector<float> timelist;
	  //accountReport from every address
	  for (i=0;i<=transactionProxyModel->rowCount()-1;i++)
	  {
	  	fTotal+=transactionProxyModel->data(transactionProxyModel->index(i,4)).toFloat();	
	  	iTotal=transactionProxyModel->data(transactionProxyModel->index(i,4)).toFloat();	
	  	addressname=transactionProxyModel->data(transactionProxyModel->index(i,3)).toString();
	  	
	  	int t = addresslist.indexOf(addressname);
	  	if (t != -1)
	  		{
	  			totallist[t]=totallist.at(t)+iTotal;
	  			timelist[t]=timelist.at(t)+1;
	  			if (iTotal>0){
	  				inlist[t]=inlist.at(t)+iTotal;
	  			}
	  			else{
	  				outlist[t]=outlist.at(t)+iTotal;
	  			}
	  		}
	  	else
	  		{
						addresslist.append(addressname);
						totallist.append(iTotal);
						timelist.append(1);
						if (iTotal>0){
							inlist.append(iTotal);
							outlist.append(0);
						}
						else{
							outlist.append(iTotal);
							inlist.append(0);
						}	  		   
	  		}	  		
	  }
	  int iTimes=i;
	  
	  reportModel->clear();
	  reportModel->setRowCount(addresslist.size()+1); 
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
        
		QString account = "";
    for (i=0;i!=addresslist.size();++i)
    { 
    	reportModel->setItem(i,0,new QStandardItem(addresslist.at(i)));
    	reportModel->setItem(i,1,new QStandardItem(dateWidget->currentText()));
    	reportModel->setItem(i,2,new QStandardItem(typeWidget->currentText()));
    	reportModel->setItem(i,3,new QStandardItem(QObject::tr("%1").arg(totallist.at(i))));
      reportModel->setItem(i,4,new QStandardItem(QObject::tr("%1").arg(timelist.at(i))));
    }
    //Total    
  	reportModel->setItem(i,0,new QStandardItem(tr("Total")));
  	reportModel->setItem(i,1,new QStandardItem(dateWidget->currentText()));
  	reportModel->setItem(i,2,new QStandardItem(typeWidget->currentText()));
  	reportModel->setItem(i,3,new QStandardItem(QObject::tr("%1").arg(QObject::tr("%1").arg(fTotal))));
    reportModel->setItem(i,4,new QStandardItem(QObject::tr("%1").arg(iTimes)));
    reportModel->item(i,0)->setTextAlignment(Qt::AlignCenter);
    reportModel->item(i,1)->setTextAlignment(Qt::AlignCenter);
    reportModel->item(i,2)->setTextAlignment(Qt::AlignCenter);
    reportModel->item(i,3)->setTextAlignment(Qt::AlignCenter);
    reportModel->item(i,4)->setTextAlignment(Qt::AlignCenter);
    reportModel->item(i,0)->setFont(QFont("Times", 10, QFont::Black));
    reportModel->item(i,1)->setFont(QFont("Times", 10, QFont::Black));
    reportModel->item(i,2)->setFont(QFont("Times", 10, QFont::Black));
    reportModel->item(i,3)->setFont(QFont("Times", 10, QFont::Black));
    reportModel->item(i,4)->setFont(QFont("Times", 10, QFont::Black));
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
