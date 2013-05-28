#include "textimage.h"

TextImage::TextImage(const QString &path, QObject *obj) :
    Image(path, obj)
{
}

void TextImage::move(int x, int y)
{
    x = x > mMaxX ? mMaxX : x;
    x = x < mMinX ? mMinX : x;
    y = y > mMaxY ? mMaxY : y;
    y = y < mMinX ? mMinY : y;
    Image::move(x, y);
}
