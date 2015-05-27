// Copyright (c) 2011-2013 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#if defined(HAVE_CONFIG_H)
#include "bitcoin-config.h"
#endif

#include "coinnectordialog.h"
#include "ui_coinnectordialog.h"

#include "bitcoinunits.h"
#include "guiutil.h"
#include "monitoreddatamapper.h"
#include "optionsmodel.h"

#include "main.h" // for CTransaction::nMinTxFee and MAX_SCRIPTCHECK_THREADS
#include "netbase.h"
#include "txdb.h" // for -dbcache defaults

#include <QDir>
#include <QIntValidator>
#include <QLocale>
#include <QMessageBox>
#include <QTimer>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QByteArray>
#include <QDateTime>
#include <QStandardItemModel>
#include <QDesktopServices>
#include <QUrl>

CoinnectorDialog::CoinnectorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CoinnectorDialog),
    model(0),
    mapper(0),
    fProxyIpValid(true)
{
    ui->setupUi(this);
    GUIUtil::restoreWindowGeometry("nCoinnectorDialogWindow", this->size(), this);
       
    ui->label_6->installEventFilter(this);
    ui->tableView->installEventFilter(this);

		manager = new QNetworkAccessManager(this); 
		connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(replyFinished(QNetworkReply*)));
		manager->get(QNetworkRequest(QUrl("https://coinnector.com/API/ticker/")));

    /* Widget-to-option mapper */
    mapper = new MonitoredDataMapper(this);
    mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    mapper->setOrientation(Qt::Vertical);
}

CoinnectorDialog::~CoinnectorDialog()
{
    GUIUtil::saveWindowGeometry("nCoinnectorDialogWindow", this);
    delete ui;
}

void CoinnectorDialog::replyFinished(QNetworkReply *reply)
{   
    int sw=0;
    QByteArray data = reply->readAll();
    QString result(data);
    ui->textReplyEdit->setText(result);
    //{"result":"success","timestamp":1432562645,"BTC_DASH":["81.2366199952",1],"DASH_BTC":["0.0116620098",-1],"BTC_DOGE":["1633333.333333333",0],"DOGE_BTC":["0.0000005782",0],"BTC_FTC":["50943.9640074796",0],"FTC_BTC":["0.0000164383",0],"BTC_LTC":["128.6659808144",-1],"LTC_BTC":["0.0073686355",1],"BTC_NMC":["623.9861305917",1],"NMC_BTC":["0.0013302033",-1],"BTC_PPC":["696.1033450117",-1],"PPC_BTC":["0.0013102915",1],"DASH_DOGE":["19047.9493465333",0],"DOGE_DASH":["0.000046971",0],"DASH_FTC":["594.1090077099",0],"FTC_DASH":["0.0013353946",0],"DASH_LTC":["1.5005039297",-1],"LTC_DASH":["0.5986030454",1],"DASH_NMC":["7.2769323725",1],"NMC_DASH":["0.1080612229",-1],"DASH_PPC":["8.1179640341",-1],"PPC_DASH":["0.1064436502",1],"DOGE_FTC":["0.0294558",0],"FTC_DOGE":["26.8492772333",0],"DOGE_LTC":["0.0000743947",-1],"LTC_DOGE":["12035.4380519333",1],"DOGE_NMC":["0.0003607888",1],"NMC_DOGE":["2172.6654488",-1],"DOGE_PPC":["0.000402487",-1],"PPC_DOGE":["2140.1427343333",1],"FTC_LTC":["0.0021150542",-1],"LTC_FTC":["375.3875038359",1],"FTC_NMC":["0.0102572918",-1],"NMC_FTC":["67.7658308718",1],"FTC_PPC":["0.0114427786",-1],"PPC_FTC":["66.751441487",1],"LTC_NMC":["4.5979263796",1],"NMC_LTC":["0.1711519169",-1],"LTC_PPC":["5.129331849",1],"PPC_LTC":["0.1685899371",-1],"NMC_PPC":["0.9259589917",-1],"PPC_NMC":["0.8176037043",1]}
    
		//listModel->setRowCount(6);
		    
    QJsonParseError err;  
    QJsonDocument json = QJsonDocument::fromJson(data, &err);
    if(json.isObject())  
    {
				QJsonObject obj = json.object();
				if(obj.contains("FTC_BTC"))
				{
					QJsonValue name_value = obj.take("FTC_BTC");
					listModel->setItem(0,0,new QStandardItem("FTC_BTC"));
					listModel->setItem(0,1,new QStandardItem(name_value.toArray()[0].toString()));
				}
				if(obj.contains("FTC_DOGE"))
				{
					QJsonValue name_value = obj.take("FTC_DOGE");
					listModel->setItem(1,0,new QStandardItem("FTC_DOGE"));
					listModel->setItem(1,1,new QStandardItem(name_value.toArray()[0].toString()));
				}
				if(obj.contains("FTC_LTC"))
				{
					QJsonValue name_value = obj.take("FTC_LTC");
					listModel->setItem(2,0,new QStandardItem("FTC_LTC"));
					listModel->setItem(2,1,new QStandardItem(name_value.toArray()[0].toString()));
				}
				if(obj.contains("FTC_PPC"))
				{
					QJsonValue name_value = obj.take("FTC_PPC");
					listModel->setItem(3,0,new QStandardItem("FTC_PPC"));
					listModel->setItem(3,1,new QStandardItem(name_value.toArray()[0].toString()));
				}
				if(obj.contains("FTC_NMC"))
				{
					QJsonValue name_value = obj.take("FTC_NMC");
					listModel->setItem(4,0,new QStandardItem("FTC_NMC"));
					listModel->setItem(4,1,new QStandardItem(name_value.toArray()[0].toString()));
				}
				if(obj.contains("FTC_DASH"))
				{
					QJsonValue name_value = obj.take("FTC_DASH");
					listModel->setItem(5,0,new QStandardItem("FTC_DASH"));
					listModel->setItem(5,1,new QStandardItem(name_value.toArray()[0].toString()));
				}
				
				
				if(obj.contains("BTC_FTC"))
				{
					QJsonValue name_value = obj.take("BTC_FTC");
					listModel->setItem(0,2,new QStandardItem("BTC_FTC"));
					listModel->setItem(0,3,new QStandardItem(name_value.toArray()[0].toString()));
				}
				if(obj.contains("DOGE_FTC"))
				{
					QJsonValue name_value = obj.take("DOGE_FTC");
					listModel->setItem(1,2,new QStandardItem("DOGE_FTC"));
					listModel->setItem(1,3,new QStandardItem(name_value.toArray()[0].toString()));
				}
				if(obj.contains("LTC_FTC"))
				{
					QJsonValue name_value = obj.take("LTC_FTC");
					listModel->setItem(2,2,new QStandardItem("LTC_FTC"));
					listModel->setItem(2,3,new QStandardItem(name_value.toArray()[0].toString()));
				}
				if(obj.contains("PPC_FTC"))
				{
					QJsonValue name_value = obj.take("PPC_FTC");
					listModel->setItem(3,2,new QStandardItem("PPC_FTC"));
					listModel->setItem(3,3,new QStandardItem(name_value.toArray()[0].toString()));
				}
				if(obj.contains("NMC_FTC"))
				{
					QJsonValue name_value = obj.take("NMC_FTC");
					listModel->setItem(4,2,new QStandardItem("NMC_FTC"));
					listModel->setItem(4,3,new QStandardItem(name_value.toArray()[0].toString()));
				}
				if(obj.contains("DASH_FTC"))
				{
					QJsonValue name_value = obj.take("DASH_FTC");
					listModel->setItem(5,2,new QStandardItem("DASH_FTC"));
					listModel->setItem(5,3,new QStandardItem(name_value.toArray()[0].toString()));
				}
		}
    reply->deleteLater();
}

void CoinnectorDialog::setModel(OptionsModel *model)
{
    this->model = model;

    if(model)
    {
        mapper->setModel(model);
        setMapper();
        mapper->toFirst();

        listModel = new QStandardItemModel(6, 4);
        listModel->setHeaderData(0, Qt::Horizontal, QObject::tr("Markets"));
        listModel->setHeaderData(1, Qt::Horizontal, QObject::tr("Price"));
        listModel->setHeaderData(2, Qt::Horizontal, QObject::tr("Markets"));
        listModel->setHeaderData(3, Qt::Horizontal, QObject::tr("Price"));
        ui->tableView->setModel(listModel);
        ui->tableView->setAlternatingRowColors(true);
        ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
        ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->tableView->setColumnWidth(0,100);
        ui->tableView->setColumnWidth(1,150);
        ui->tableView->setColumnWidth(2,100);
        ui->tableView->setColumnWidth(3,150);
        ui->tableView->setShowGrid(true);
        ui->tableView->setAlternatingRowColors(true);
        ui->tableView->setStyleSheet("alternate-background-color: rgb(246, 246, 246);");
    }
}

void CoinnectorDialog::setMapper()
{

}

void CoinnectorDialog::enableOkButton()
{

}

void CoinnectorDialog::disableOkButton()
{

}

void CoinnectorDialog::setOkButtonState(bool fState)
{

}

void CoinnectorDialog::on_resetButton_clicked()
{

}

void CoinnectorDialog::on_txStatButton_clicked()
{

}

void CoinnectorDialog::on_postTransButton_clicked()
{

}

void CoinnectorDialog::on_postFixButton_clicked()
{

}

void CoinnectorDialog::on_okButton_clicked()
{

}

void CoinnectorDialog::on_pushButton_clicked()
{
    /* Refresh API Button */
		manager->get(QNetworkRequest(QUrl("https://coinnector.com/API/ticker/")));
}

void CoinnectorDialog::on_cancelButton_clicked()
{
    /* Exit  Button */
    reject();
}

void CoinnectorDialog::showRestartWarning(bool fPersistent)
{

}

void CoinnectorDialog::clearStatusLabel()
{

}

void CoinnectorDialog::updateDisplayUnit()
{

}

void CoinnectorDialog::doProxyIpChecks(QValidatedLineEdit *pUiProxyIp, int nProxyPort)
{

}

bool CoinnectorDialog::eventFilter(QObject *object, QEvent *event)
{
    if(event->type() == QEvent::MouseButtonPress)
    {	   		
		    if (object == ui->label_6)
		    {
						//manager->get(QNetworkRequest(QUrl("https://coinnector.com/API/ticker/")));
						QDesktopServices::openUrl(QUrl("https://coinnector.com"));
		    }
  	}
  	
    if(event->type() == QEvent::MouseButtonDblClick)
    {	   		
		    QDesktopServices::openUrl(QUrl("https://coinnector.com"));
  	}
  	
    return QDialog::eventFilter(object, event);
}
