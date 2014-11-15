// Copyright (c) 2011-2013 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef SHIFTDIALOG_H
#define SHIFTDIALOG_H

#include <QDialog>
#include <QtNetwork>

class MonitoredDataMapper;
class OptionsModel;
class QValidatedLineEdit;

namespace Ui {
class ShiftDialog;
}

/** Preferences dialog. */
class ShiftDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ShiftDialog(QWidget *parent);
    ~ShiftDialog();

    void setModel(OptionsModel *model);
    void setMapper();

protected:
    bool eventFilter(QObject *object, QEvent *event);

private slots:
    /* enable OK button */
    void enableOkButton();
    /* disable OK button */
    void disableOkButton();
    /* set OK button state (enabled / disabled) */
    void setOkButtonState(bool fState);
    void on_resetButton_clicked();
    void on_okButton_clicked();
    void on_cancelButton_clicked();
    void on_txStatButton_clicked();
    void on_postTransButton_clicked();
    void on_postFixButton_clicked();

    void showRestartWarning(bool fPersistent = false);
    void clearStatusLabel();
    void updateDisplayUnit();
    void doProxyIpChecks(QValidatedLineEdit *pUiProxyIp, int nProxyPort);
    void replyFinished(QNetworkReply *);

signals:
    void proxyIpChecks(QValidatedLineEdit *pUiProxyIp, int nProxyPort);

private:
    Ui::ShiftDialog *ui;
    OptionsModel *model;
    MonitoredDataMapper *mapper;
    bool fProxyIpValid;
    QNetworkAccessManager *manager;    
};

#endif // SHIFTDIALOG_H
