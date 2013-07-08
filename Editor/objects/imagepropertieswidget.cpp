#include "imagepropertieswidget.h"

ImagePropertiesWidget::ImagePropertiesWidget(QWidget *parent) :
    ObjectEditorWidget(parent)
{
    mRotate = new QLineEdit(this);
    mRotate->setObjectName("rotateEditer");
    connect(mRotate, SIGNAL(textChanged(QString)), this, SLOT(onRotateChanged(QString)));
    beginGroup("旋转", "rotate");
    appendRow("角度", mRotate);
    endGroup();
    mCurrentObject = 0;
}

void ImagePropertiesWidget::updateData(Object *obj)
{
    if (obj == mCurrentObject)
        return;

    ObjectEditorWidget::updateData(obj);

    mCurrentObject = qobject_cast<Image*>(obj);
    if (! mCurrentObject)
        return;
}

void ImagePropertiesWidget::onRotateChanged(const QString &rotate)
{
    if(mCurrentObject)
    {
        Image *image = static_cast<Image *>(mCurrentObject);
        image->rotate(rotate.toDouble());
    }
}
