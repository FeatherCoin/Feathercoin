#include "qimagesource.h"
#include <QColor>

QImageLuminanceSource::QImageLuminanceSource(QImage& _image) : LuminanceSource(_image.width(), _image.height())
{
    image = _image.copy();
}


ArrayRef<char> QImageLuminanceSource::getRow(int y, ArrayRef<char> _row) const
{
    for (int x = 0; x < this->getWidth(); x++)
        _row[x] = qGray(image.pixel(x, y));
    return _row;
}

ArrayRef<char> QImageLuminanceSource::getMatrix() const
{
    ArrayRef<char> mymatrix(this->getWidth()*this->getHeight());
    for (int y = 0; y < this->getHeight(); y++)
    {
        for (int x = 0; x < this->getWidth(); x++) {
            mymatrix[y*this->getWidth()+x] = qGray(image.pixel(x, y));
        }
    }
    return mymatrix;
}

