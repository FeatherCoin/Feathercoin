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
    ArrayRef<char> mymatrix(width*height);
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++) {
            mymatrix[y*width+x] = qGray(image.pixel(x, y));
        }
    }
    return mymatrix;
}

