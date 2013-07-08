#ifndef IMAGEPROPERTIESWIDGET_H
#define IMAGEPROPERTIESWIDGET_H

#include <QWidget>
#include <QTextEdit>
#include <QSpinBox>
#include "color_pushbutton.h"
#include "choosefilebutton.h"
#include "textbox.h"
#include "object.h"
#include "object_editor_widget.h"
#include "choosefontwidget.h"

class ImagePropertiesWidget : public ObjectEditorWidget
{
    Q_OBJECT
public:
    explicit ImagePropertiesWidget(QWidget *parent = 0);
    void updateData(Object *obj);
signals:
    
public slots:
    void onRotateChanged(const QString &rotate);
private:
    QLineEdit *mRotate;
    Image *mCurrentObject;
};

#endif // IMAGEPROPERTIESWIDGET_H
