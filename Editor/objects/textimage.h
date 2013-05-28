#ifndef TEXTIMAGE_H
#define TEXTIMAGE_H
#include "image.h"
class TextImage : public Image
{
public:
    TextImage(const QString &path, QObject *obj);
    virtual void move(int x, int y);
    void setMaxX(int maxX)
    {
        mMaxX = maxX;
    }

    void setMaxY(int maxY)
    {
        mMaxY = maxY;
    }

    void setMinX(int minX)
    {
        mMinX = minX;
    }

    void setMinY(int minY)
    {
        mMinY = minY;
    }

private:
    int mMaxX;
    int mMaxY;
    int mMinX;
    int mMinY;
};

#endif // TEXTIMAGE_H
