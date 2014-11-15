// Copyright (c) 2011-2013 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#if defined(HAVE_CONFIG_H)
#include "bitcoin-config.h"
#endif

#include "shiftdialog.h"
#include "ui_shiftdialog.h"

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

ShiftDialog::ShiftDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShiftDialog),
    model(0),
    mapper(0),
    fProxyIpValid(true)
{
    ui->setupUi(this);
    GUIUtil::restoreWindowGeometry("nShiftDialogWindow", this->size(), this);
    
    QStringList strPair;
    strPair << "ftc_btc" << "ftc_doge" << "ftc_ltc";
    ui->comboBox->addItems(strPair);
    ui->fixComboBox->addItems(strPair);
    
    ui->label_5->installEventFilter(this);
    ui->label_7->installEventFilter(this);
    ui->label_9->installEventFilter(this);
    ui->label_8->installEventFilter(this);

		manager = new QNetworkAccessManager(this); 
		connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(replyFinished(QNetworkReply*)));
		manager->get(QNetworkRequest(QUrl("http://shapeshift.io/rate/ftc_btc")));
		manager->get(QNetworkRequest(QUrl("http://shapeshift.io/rate/ftc_doge")));
		manager->get(QNetworkRequest(QUrl("http://shapeshift.io/limit/ftc_btc")));
		manager->get(QNetworkRequest(QUrl("http://shapeshift.io/limit/ftc_doge")));
		

    /* Widget-to-option mapper */
    mapper = new MonitoredDataMapper(this);
    mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    mapper->setOrientation(Qt::Vertical);
}

ShiftDialog::~ShiftDialog()
{
    GUIUtil::saveWindowGeometry("nShiftDialogWindow", this);
    delete ui;
}

void ShiftDialog::replyFinished(QNetworkReply *reply)
{   
    int sw=0;
    QByteArray data = reply->readAll();
    QString result(data);
    ui->textReplyEdit->setText(result);
    
    QJsonParseError err;  
    QJsonDocument json = QJsonDocument::fromJson(data, &err);
    if(json.isObject())  
    {
    		/* Rate=1,Deposit Limit=2,status=3,deposit=4,Fixed Amount Transaction=5 */
		    QJsonObject obj = json.object();
				if(obj.contains("pair"))  
				{
					QJsonValue name_value = obj.take("pair");
					if(name_value.toString()=="ftc_btc")
			    {
			        sw=1;
			    }
					if(name_value.toString()=="ftc_doge")
			    {
			        sw=2;
			    }			    
				}
				
				if(obj.contains("rate"))  
				{
					QJsonValue name_value = obj.take("rate");
					if(sw==1)
			    {
			        ui->label_5->setText(name_value.toString());
			    }
					if(sw==2)
			    {
			        ui->label_7->setText(name_value.toString());
			    }
				}
				if(obj.contains("limit"))  
				{
					QJsonValue name_value = obj.take("limit");
					if(sw==1)
			    {
			        ui->label_9->setText(name_value.toString());
			    }
					if(sw==2)
			    {
			        ui->label_8->setText(name_value.toString());
			    }
				}
				
				if(obj.contains("status"))  
				{
					QJsonValue name_value = obj.take("status");
	        sw=3;
	        ui->label_11->setText(tr("status: ")+name_value.toString());	    
				}	
				if(obj.contains("address"))  
				{
					QJsonValue name_value = obj.take("address");
					if(sw==3)
			    {
			        ui->label_10->setText(tr("address: ")+name_value.toString());
			    }
				}
				if(obj.contains("incomingCoin"))  
				{
					if(sw==3)
			    {					
						QJsonValue name_value = obj.take("incomingCoin");
				    ui->label_13->setText(tr("amount deposited ,incomingCoin: ")+QString::number(name_value.toVariant().toDouble()));
				  }
				}
				if(obj.contains("incomingType"))  
				{
					if(sw==3)
			    {								
						QJsonValue name_value = obj.take("incomingType");
				    ui->label_14->setText(tr("coin type of deposit,incomingType: ")+name_value.toString());
			    }
				}
				if(obj.contains("withdraw"))  
				{
					if(sw==3)
			    {								
						QJsonValue name_value = obj.take("withdraw");
				    ui->label_12->setText(tr("withdrawal address: ")+name_value.toString());
			    }
				}
				if(obj.contains("outgoingCoin"))  
				{
					if(sw==3)
			    {								
						QJsonValue name_value = obj.take("outgoingCoin");
				    ui->label_15->setText(tr("amount sent to withdrawal address,outgoingCoin: ")+QString::number(name_value.toVariant().toDouble()));
			    }
				}
				if(obj.contains("outgoingType"))  
				{
					if(sw==3)
			    {								
						QJsonValue name_value = obj.take("outgoingType");
				    ui->label_17->setText(tr("coin type of withdrawal,outgoingType: ")+name_value.toString());
			    }
				}
				if(obj.contains("transaction"))  
				{
					if(sw==3)
			    {								
						QJsonValue name_value = obj.take("transaction");
				    ui->label_16->setText(tr("transaction id: ")+name_value.toString());
			    }
				}
				
				
				if(obj.contains("deposit"))  
				{
					QJsonValue name_value = obj.take("deposit");
	        sw=4;
	        ui->lineEdit_3->setText(name_value.toString());
				}
				if(obj.contains("depositType"))  
				{
					QJsonValue name_value = obj.take("depositType");
	        if(sw==4)
	        {
	        	ui->label_19->setText(tr("Deposit Type: ")+name_value.toString());	    
	      	}
				}
				if(obj.contains("withdrawal"))  
				{
					QJsonValue name_value = obj.take("withdrawal");
	        if(sw==4)
	        {
	        	ui->label_20->setText(tr("Withdrawal Address: ")+name_value.toString());	    
	      	}
				}
				if(obj.contains("withdrawalType"))  
				{
					QJsonValue name_value = obj.take("withdrawalType");
	        if(sw==4)
	        {
	        	ui->label_21->setText(tr("Withdrawal Type: ")+name_value.toString());	    
	      	}
				}
				if(obj.contains("returnAddress"))  
				{
					QJsonValue name_value = obj.take("returnAddress");
	        if(sw==4)
	        {
	        	ui->label_23->setText(tr("return Address: ")+name_value.toString());	    
	      	}
				}
				if(obj.contains("returnAddressType"))  
				{
					QJsonValue name_value = obj.take("returnAddressType");
	        if(sw==4)
	        {
	        	ui->label_24->setText(tr("return Address Type: ")+name_value.toString());	    
	      	}
				}
				

				if(obj.contains("success"))  
				{
					QJsonValue name_value = obj.take("success");
	        sw=5;
	        ui->label_27->setText(tr("Post status: success"));
	        
	        QJsonObject obj2 = name_value.toObject();
        	if(obj2.contains("pair"))
        	{
						QJsonValue name_value = obj2.take("pair");
		        if(sw==5)
		        {
		        	ui->label_35->setText(tr("Pair: ")+name_value.toString());	    
		      	}
	      	}
        	if(obj2.contains("withdrawal"))
        	{
						QJsonValue name_value = obj2.take("withdrawal");
		        if(sw==5)
		        {
		        	ui->label_28->setText(tr("Withdrawal Address: ")+name_value.toString());	    
		      	}
	      	}
        	if(obj2.contains("withdrawalAmount"))
        	{
						QJsonValue name_value = obj2.take("withdrawalAmount");
		        if(sw==5)
		        {
		        	ui->label_29->setText(tr("Withdrawal Amount: ")+QString::number(name_value.toVariant().toDouble()));	    
		      	}
	      	}
        	if(obj2.contains("deposit"))
        	{
						QJsonValue name_value = obj2.take("deposit");
		        if(sw==5)
		        {
		        	ui->lineEdit->setText(name_value.toString());	
		      	}
	      	}
        	if(obj2.contains("depositAmount"))
        	{
						QJsonValue name_value = obj2.take("depositAmount");
		        if(sw==5)
		        {
		        	ui->lineEdit_2->setText(name_value.toString());	
		      	}
	      	}
        	if(obj2.contains("expiration"))
        	{
						QJsonValue name_value = obj2.take("expiration");
		        if(sw==5)
		        {
		        	QDateTime dt;
		        	dt.setTimeSpec(Qt::UTC);
		        	dt=QDateTime::currentDateTime();		        		        	
		        	uint st2=dt.toTime_t();		        	
		        	double st1=name_value.toVariant().toDouble();
		        	int st=(st1/1000-st2);
		        	ui->label_33->setText(tr("Expiration: ")+QString::number(st)+"sec");  
		      	}
	      	}
        	if(obj2.contains("quotedRate"))
        	{
						QJsonValue name_value = obj2.take("quotedRate");
		        if(sw==5)
		        {
		        	ui->label_32->setText(tr("Rate: ")+name_value.toString());	    
		      	}
	      	}
        	if(obj2.contains("returnAddress"))
        	{
						QJsonValue name_value = obj2.take("returnAddress");
		        if(sw==5)
		        {
		        	ui->label_34->setText(tr("Return Address: ")+name_value.toString());	    
		      	}
	      	}
				}
				
		}
    reply->deleteLater();
}

void ShiftDialog::setModel(OptionsModel *model)
{
    this->model = model;

    if(model)
    {


        mapper->setModel(model);
        setMapper();
        mapper->toFirst();
    }

}

void ShiftDialog::setMapper()
{

}

void ShiftDialog::enableOkButton()
{

}

void ShiftDialog::disableOkButton()
{

}

void ShiftDialog::setOkButtonState(bool fState)
{

}

void ShiftDialog::on_resetButton_clicked()
{

}

void ShiftDialog::on_txStatButton_clicked()
{
	QString strURL="http://shapeshift.io/txStat/"+ui->txAddress->text();
	manager->get(QNetworkRequest(QUrl(strURL)));
}

void ShiftDialog::on_postTransButton_clicked()
{
	QByteArray post_data;
	QString jsonData="{\"withdrawal\":\""+ui->withdrawalAddr->text()+"\",\"pair\":\""+ui->comboBox->currentText()+"\",\"returnAddress\":\""+ui->ftcAddr->text()+"\"}";
	post_data.append(jsonData);
	//post_data.append("{\"withdrawal\":\"17GZr6RaDfUt2HXRVkmJgVhXKdkej1VMb9\",\"pair\":\"ftc_btc\",\"returnAddress\":\"71whQbi6pq2aCSvMvcTKCcTZDfAbUvf2Se\"}");
	
	QNetworkRequest network_request;
	network_request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json;charset=utf-8");
	network_request.setHeader(QNetworkRequest::ContentLengthHeader, post_data.length());
	network_request.setUrl(QUrl("http://shapeshift.io/shift"));
	
	manager->post(network_request, post_data);
	
}

void ShiftDialog::on_postFixButton_clicked()
{
	QByteArray post_data;
	QString jsonData="{\"amount\":"+ui->fixAmountEdit->text()+",\"withdrawal\":\""+ui->fixWithdrawalAddrEdit->text()+"\",\"pair\":\""+ui->fixComboBox->currentText()+"\",\"returnAddress\":\""+ui->returnAddr->text()+"\"}";
	post_data.append(jsonData);
	//post_data.append("{\"amount\":0.0001,\"withdrawal\":\"17GZr6RaDfUt2HXRVkmJgVhXKdkej1VMb9\",\"pair\":\"ftc_btc\",\"returnAddress\":\"71whQbi6pq2aCSvMvcTKCcTZDfAbUvf2Se\"}");
	
	QNetworkRequest network_request;
	network_request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json;charset=utf-8");
	network_request.setHeader(QNetworkRequest::ContentLengthHeader, post_data.length());
	network_request.setUrl(QUrl("http://shapeshift.io/sendamount"));
	
	manager->post(network_request, post_data);
}

void ShiftDialog::on_okButton_clicked()
{
    /* Refresh API Button */
		manager->get(QNetworkRequest(QUrl("http://shapeshift.io/rate/ftc_btc")));
		manager->get(QNetworkRequest(QUrl("http://shapeshift.io/rate/ftc_doge")));
		manager->get(QNetworkRequest(QUrl("http://shapeshift.io/limit/ftc_btc")));
		manager->get(QNetworkRequest(QUrl("http://shapeshift.io/limit/ftc_doge")));   
}

void ShiftDialog::on_cancelButton_clicked()
{
    /* Exit  Button */
    reject();
}

void ShiftDialog::showRestartWarning(bool fPersistent)
{

}

void ShiftDialog::clearStatusLabel()
{

}

void ShiftDialog::updateDisplayUnit()
{

}

void ShiftDialog::doProxyIpChecks(QValidatedLineEdit *pUiProxyIp, int nProxyPort)
{

}

bool ShiftDialog::eventFilter(QObject *object, QEvent *event)
{
    if(event->type() == QEvent::MouseButtonPress)
    {	   		
		    if (object == ui->label_5)
		    {
						manager->get(QNetworkRequest(QUrl("http://shapeshift.io/rate/ftc_btc")));
		    }
		    if (object == ui->label_7)
		    {
						manager->get(QNetworkRequest(QUrl("http://shapeshift.io/rate/ftc_doge")));
		    }
		    if (object == ui->label_9)
		    {
						manager->get(QNetworkRequest(QUrl("http://shapeshift.io/limit/ftc_btc")));
		    }
		    if (object == ui->label_8)
		    {
						manager->get(QNetworkRequest(QUrl("http://shapeshift.io/limit/ftc_doge")));   
		    }
  	}
    return QDialog::eventFilter(object, event);
}
