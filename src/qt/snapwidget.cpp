
#include "snapwidget.h"
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#include <QDialog>
#else
#include <QDesktopWidget>
#endif
#include <zxing/common/GlobalHistogramBinarizer.h>
#include <zxing/Binarizer.h>
#include <zxing/BinaryBitmap.h>
#include <zxing/MultiFormatReader.h>
#include "qimagesource.h"


//////////////////  SnapWidget Class
SnapWidget::SnapWidget(QWidget* _parent)
#ifdef Q_OS_MAC
   : QDialog( _parent, Qt::Widget | Qt::WindowStaysOnTopHint|  Qt::WindowCloseButtonHint ){
#else
   : QDialog(_parent, Qt::Widget | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowStaysOnTopHint) {
#endif

    setupUi(this);
    this->setMinimumWidth(50);
    prepareMask();
    show();
}

SnapWidget::~SnapWidget()
{

}

void SnapWidget::on_snapButton_clicked() 
{
    int _x, _y, _w, _h;
    _x = geometry().x();
    _y = geometry().y() + cancelButton->height();
    _w = geometry().width();
    _h = geometry().height() - cancelButton->height();
    QPixmap p;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen) {   
        p = screen->grabWindow(0, _x, _y, _w, _h);
        }
#else
    p = QPixmap::grabWindow(QApplication::desktop()->winId(), _x, _y, _w, _h);
#endif
    
    if (!p.isNull()) {
        QImage image = p.toImage();
        Ref<Result> r;
        MultiFormatReader* qrDecoder = new MultiFormatReader();
        QImageLuminanceSource* lumImage = new QImageLuminanceSource(image);
        Ref<LuminanceSource> imageRef(lumImage);
        GlobalHistogramBinarizer* binarizer = new GlobalHistogramBinarizer(imageRef);
        Ref<Binarizer> binarizerRef(binarizer);
        BinaryBitmap* binaryBitmap = new BinaryBitmap(binarizerRef);
        Ref<BinaryBitmap> binaryBitmapRef(binaryBitmap);
        try {
            r = qrDecoder->decode(binaryBitmapRef, DecodeHints::QR_CODE_HINT);
        }
        catch (Exception e) {
            delete qrDecoder;
            close();
            return;
        }
        decodedString = QString(r->getText()->getText().c_str());
        delete qrDecoder;
    }
    this->close();
}

void SnapWidget::on_cancelButton_clicked() 
{
    this->close();
}

void SnapWidget::closeEvent(QCloseEvent *event)
{
    emit finished(decodedString);
}

void SnapWidget::prepareMask() 
{
    int fh = frameSize().height();
    int fw = frameSize().width();
    QStyleOptionTitleBar option;
    option.initFrom(this);
    
    QRegion r;  
#ifdef Q_OS_MAC
    //this looks acceptable on Mac
    QRegion rgn(0, 0, fw, fh);
    QRegion rgn2(15, cancelButton->height() + 10, fw - 30, fh - 80);
#else
    //this looks ok on windows and ubuntu
    int captionHeight = style()->pixelMetric(QStyle::PM_TitleBarHeight, &option, this);
    QRegion rgn(-7, -captionHeight, fw, fh);
    QRegion rgn2(5, cancelButton->height() + 3, fw - 28, fh - 75);
#endif
    r = rgn.subtracted(rgn2);
    setMask(r);
    update();
}


void SnapWidget::resizeEvent(QResizeEvent*) 
{
    this->prepareMask();
}
