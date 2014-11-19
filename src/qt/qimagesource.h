#ifndef QIMAGELUMINANCESOURCE_H
#define QIMAGELUMINANCESOURCE_H

#include <QImage>
#include <QString>
#include <zxing/LuminanceSource.h>

using namespace zxing;

class QImageLuminanceSource : public LuminanceSource
{
public:
    QImageLuminanceSource(QImage& image);
    ArrayRef<char> getMatrix() const;
    ArrayRef<char> getRow(int y, ArrayRef<char> row) const;
   
private:
    QImage image;
};

#endif //QIMAGELUMINANCESOURCE_H
