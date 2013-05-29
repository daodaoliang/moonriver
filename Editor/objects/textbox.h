/* Copyright (C) 2012, 2013 Carlos Pais 
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TEXT_OBJECT_H
#define TEXT_OBJECT_H

#include <QObject>
#include "scene.h"
#include "object.h"
#include "textbox_editor_widget.h"
#include "textimage.h"
#include <QVector>
#include <QFontMetrics>
class TextPropertiesWidget;
class ObjectEditorWidget;

class TextBox : public Object
{
    Q_OBJECT

    QString mText;
    QColor mTextColor;
    QRect mTextRect;
    QString mPlaceholderText;
    Qt::Alignment mTextAlignment;
    QFont mFont;
    TextImage *mTextImageResource;
    QPoint mImagePoint;
    Object *mTextImage;
    QVector< QVector < int > > mRectPoint;
    int mWordWidth;     //每个字符的宽度
    int mWordHeight;    //每个字符的高度
    int mRowCount;      //行数
    int mColCount;      //列数
    QString mTargetString;      //目标字符串
public:
    explicit TextBox(QObject *parent = 0, const QString& name="TextBox");
    TextBox(const QString&, QObject* parent=0, const QString& name="TextBox");
    TextBox(const QVariantMap&, QObject*);
    virtual ~TextBox();
    QString text();
    virtual void setText(const QString&);
    void setTextColor(const QColor&);
    QColor textColor();
    QRect textRect();
    QString currentText();
    int fontSize();
    void setFontSize(int);

    QString fontFamily();
    void setFontFamily(const QString&);

    QString placeholderText() const;
    void setPlaceholderText(const QString&);

    void setTextRect(const QRect&);
    virtual void move(int, int);
    void init(const QString&);
    static TextPropertiesWidget* textEditorWidget();
    static void setTextEditorWidget(TextPropertiesWidget *);
    virtual ObjectEditorWidget* editorWidget();
    virtual void paint(QPainter &);
    virtual QVariantMap toJsonObject();

    void setX(int);
    void setY(int);
    virtual void setWidth(int, bool percent=false);
    virtual void setHeight(int, bool percent=false);

    Qt::Alignment textAlignment();
    QString textAlignmentAsString();
    void setTextAlignment(Qt::Alignment);

    void alignText();
    //设置图片
    void setTextImage(const QString &textImage);
    //设置不规则图形点
    void fillImageRect(int rowBegin, int rowEnd, int colBegin, int colEnd);
    //获取图像在某行某列是否不是透明
    int hasColor(int row, int col);
private:
    void printVector();
signals:

public slots:
    void textImageDeleted(Object *obj);
};

#endif // TEXT_OBJECT_H
