#ifndef CREATEMULTISIGADDRESSDIALOG_H
#define CREATEMULTISIGADDRESSDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

namespace Ui {
    class CreateMultiSigAddrDialog;
}

/** Dialog for editing an address and associated information.
 */
class CreateMultiSigAddrDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateMultiSigAddrDialog(QWidget *parent = 0);
    ~CreateMultiSigAddrDialog();

public slots:
    void create();
    void cancel();
    void handleSelectionChanged(int idx);
    void onTextChanged0(const QString & text);
    void onTextChanged1(const QString & text);
    void onTextChanged2(const QString & text);
    void importAddress();
    void updatePromptText();

private:
    Ui::CreateMultiSigAddrDialog *ui;
    int currentPubkeyNum;
    void onTextChanged(QLabel* label, const QString & text);
};

#endif // CREATEMULTISIGADDRESSDIALOG_H
