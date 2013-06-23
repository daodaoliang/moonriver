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

#include "textbox.h"

#include <QDebug>
#include <QTextCodec>

#include "utils.h"
#include "scene_manager.h"
static TextPropertiesWidget* mEditorWidget = 0;

TextBox::TextBox(QObject *parent, const QString& name) :
    Object(parent, name)
{
    mSceneRect.setY(Scene::height()-(Scene::height()/3));
    mSceneRect.setHeight(Scene::height()/3);
    mSceneRect.setWidth(Scene::width());
    init("");
}

TextBox::TextBox(const QString& text, QObject* parent, const QString& name) :
    Object(parent, name)
{
    mSceneRect.setY(Scene::height()-(Scene::height()/3));
    mSceneRect.setHeight(Scene::height()/3);
    mSceneRect.setWidth(Scene::width());
    init(text);
}

TextBox::TextBox(const QVariantMap & data, QObject * parent):
    Object(data, parent)
{
    init("");

    if (data.contains("text") && data.value("text").type() == QVariant::String) {
        QTextCodec *codec = QTextCodec::codecForName("UTF-8");

        if (codec)
            mText = codec->toUnicode(data.value("text").toByteArray());
        else
            mText = data.value("text").toString();
    }

    if (data.contains("textColor") && data.value("textColor").type() == QVariant::List)
        mTextColor = Utils::listToColor(data.value("textColor").toList());

    if (data.contains("textAlignment") && data.value("textAlignment").type() == QVariant::String) {
        QString align = data.value("textAlignment").toString();
        Qt::Alignment alignment;

        if (align.contains("HCenter", Qt::CaseInsensitive))
            alignment |= Qt::AlignHCenter;
        else if (align.contains("Left", Qt::CaseInsensitive))
            alignment |= Qt::AlignLeft;
        else if (align.contains("Right", Qt::CaseInsensitive))
            alignment |= Qt::AlignRight;

        if (align.contains("VCenter", Qt::CaseInsensitive))
            alignment |= Qt::AlignVCenter;
        else if (align.contains("Top", Qt::CaseInsensitive))
            alignment |= Qt::AlignTop;
        else if (align.contains("Bottom", Qt::CaseInsensitive))
            alignment |= Qt::AlignBottom;

        mTextAlignment = alignment;
    }

    if (data.contains("font") && data.value("font").type() == QVariant::String) {
        QString font = data.value("font").toString();
        mFont.setFamily(Utils::fontFamily(font));
        mFont.setPixelSize(Utils::fontSize(font));
    }
}

void TextBox::init(const QString& text)
{
    QString newText(text);
    if (text.isNull() || text.isEmpty())
        newText = tr("The text goes here");

    mText = newText;
    mTextColor = QColor(Qt::black);
    mTextRect = sceneRect();
    mTextImage = NULL;
    mPlaceholderText = "";
    mTextAlignment = Qt::AlignLeft | Qt::AlignTop;
    setType("TextBox");
    mFont.setFamily(Object::defaultFontFamily());
    mFont.setPixelSize(Object::defaultFontSize());
    connect(SceneManager::currentScene(), SIGNAL(removeSceneObject(Object*)),
            this, SLOT(textImageDeleted(Object*)));
}

TextBox::~TextBox()
{
}

QRect TextBox::textRect()
{
    return mTextRect;
}

void TextBox::setTextRect(const QRect & rect)
{
    mTextRect = rect;
}

QString TextBox::text()
{
    return mText;
}

void TextBox::setText(const QString & text)
{
    mText = text;
    emit dataChanged();
}


TextPropertiesWidget* TextBox::textEditorWidget()
{
    return mEditorWidget;
}

ObjectEditorWidget* TextBox::editorWidget()
{
    return mEditorWidget;
}

void TextBox::setTextEditorWidget(TextPropertiesWidget* widget)
{
    if (! widget)
        return;

    if (mEditorWidget)
        mEditorWidget->deleteLater();

    mEditorWidget = widget;
}

QColor TextBox::textColor()
{
    return mTextColor;
}

void TextBox::setTextColor(const QColor& color)
{
    mTextColor = color;
    emit dataChanged();
}

void TextBox::move(int x, int y)
{
    mTextRect.moveTo(x, y);
    Object::move(x, y);
    alignText();
}

void TextBox::setX(int x)
{
    Object::setX(x);
}

void TextBox::setY(int y)
{
    Object::setY(y);
}


void TextBox::setWidth(int w, bool percent)
{
    Object::setWidth(w, percent);
    mTextRect.setWidth(width());
    emit dataChanged();
}

void TextBox::setHeight(int h, bool percent)
{
    Object::setHeight(h, percent);
    mTextRect.setHeight(h);
    emit dataChanged();

}

void TextBox::alignText()
{
    /*QRect rect = sceneRect();
    QFontMetrics metrics(mFont);
    int textWidth = metrics.width(currentText());
    int textHeight = metrics.height();

    if (mTextAlignment == Qt::AlignCenter) {
        qDebug() << "center";
        int hw = rect.width() / 2;
        int hh = rect.height() / 2;
        int htw = textWidth / 2;
        int hth = textHeight / 2;

        mTextRect.setLeft(rect.x() + hw - htw);
        mTextRect.setTop(rect.y() + hh - hth);
        mTextRect.setWidth(textWidth);
        mTextRect.setHeight(textHeight);
    }


    qDebug() << mTextRect << textWidth << textHeight;*/

}

void TextBox::setTextImage(const QString &image)
{
    TextImage *textImageResource = new TextImage(image, ResourceManager::instance());
    ResourceManager::instance()->addResource(textImageResource);

    Scene* scene = SceneManager::currentScene();
    if (! scene)
        return;

    Object* resource = textImageResource;
    if (resource) {
        QVariantMap data(resource->toJsonObject());
        mTextImage = ResourceManager::instance()->createResource(data, false);
        if (mTextImage) {
            mTextImage->setResource(resource);
            scene->appendObject(mTextImage, true);
            mTextImage->move(x(), y());
        }
    }

    emit dataChanged();
}

void TextBox::fillImageRect()
{
    //获取图像起始点
    //每行长度
    int rowLength = qMin(mTextImage->x() - x() + mTextImage->width(), mWordWidth * mColCount);
    //每列宽度
    int colLength = qMin(mTextImage->y() - y() + mTextImage->height(), mWordHeight * mRowCount);
    int colBegin = qMax((mTextImage->x() - x()) / mWordWidth, 0) ;
    int colEnd = qMax(rowLength, 0) / mWordWidth;
    if(qMax(rowLength, 0) % mWordWidth != 0)
    {
        colEnd++;
    }
    int rowBegin = qMax(mTextImage->y() - y(), 0) / mWordHeight;
    int rowEnd = qMax(colLength, 0) / mWordHeight;
    if(qMax(colLength, 0) % mWordHeight != 0)
    {
        rowEnd++;
    }

    Image *image = static_cast<Image *>(mTextImage);

    QPixmap tmpImage = image->image()->pixmap()->scaled(image->sceneRect().size());
    for(int i = 0; i < rowEnd - rowBegin; i++)
    {
        for(int j = 0; j < colEnd - colBegin; j++)
        {
            mRectPoint[rowBegin + i][colBegin + j] = hasColor(rowBegin + i, colBegin + j, tmpImage);
        }
    }
}

int TextBox::hasColor(int row, int col, QPixmap &tmpImage)
{
    int beginRowPx = qMax(row * mWordHeight + y() - mTextImage->y(), 0);
    int endRowPx = (row + 1) * mWordHeight + y() - mTextImage->y();
    int beginColPx = qMax(col * mWordWidth + x() - mTextImage->x(), 0);
    int endColPx = (col + 1) * mWordWidth + x() - mTextImage->x();
    //qDebug() << "--------------------------";
    //qDebug() << "(" << beginRowPx << "," << beginColPx << ")";
    //qDebug() << "(" << endRowPx << "," << endColPx << ")";


    for(int i = beginRowPx; i < qMin(endRowPx, tmpImage.height()); i++)
    {
        for(int j = beginColPx; j < qMin(endColPx, tmpImage.width()); j++)
        {
            if(tmpImage.toImage().pixel(j, i))
            {
                //qDebug() << "row" << row << "col" << col << "color"
                //         << image->image()->pixmap()->toImage().pixel(j, i);
                //qDebug() << "x" << x() << "textimage" << mTextImage->x();
                return 1;
            }
        }
    }
    return 0;
}

void TextBox::updatePaintParam()
{
    //获取字体高宽
    QFontMetrics metrics(mFont);
    mWordWidth = metrics.width(" ");
    mWordHeight = metrics.lineSpacing();
    //qDebug() << mWordWidth << mWordHeight;
    //获取行数与列数
    mColCount = contentWidth() / mWordWidth;
    mRowCount = contentHeight() / mWordHeight;
    //清空数组
    mRectPoint.clear();
    for(int i = 0; i < mRowCount; i++)
    {
        QVector<int> tmpVector;
        for(int j = 0; j < mColCount; j++)
        {
            tmpVector.append(0);
        }
        mRectPoint.append(tmpVector);
    }
}

void TextBox::printVector()
{
    QString printString;
    for(int i = 0; i < mRectPoint.count(); i++)
    {
        for(int j = 0; j < mRectPoint[i].count(); j++)
        {
            printString.append(QString::number(mRectPoint[i][j]));
        }
        qDebug() << printString;
        printString.clear();
    }
}

void TextBox::textImageDeleted(Object *obj)
{
    if(mTextImage == obj)
    {
        mTextImage = NULL;
    }
}

QString TextBox::placeholderText() const {
    return mPlaceholderText;
}

void TextBox::setPlaceholderText(const QString& text)
{
    mPlaceholderText = text;
    emit dataChanged();
}

void TextBox::paint(QPainter & painter)
{
    Object::paint(painter);
    mTargetString.clear();
    if(mTextImage)
    {
        //更新参数
        updatePaintParam();

        //将图像区域赋值为1
        fillImageRect();

        //打印一下数组，用于调试
        printVector();

        //画文字
        int tmpRow = 0;
        int tmpCol = 0;
        int count = 0;
        QPen pen(mTextColor);
        painter.save();
        painter.setFont(mFont);
        painter.setPen(pen);
        QString textDrawn;
        QString mTextStore = mText;
        qDebug() << mText.count();
        while(!mText.isEmpty() && count < mRowCount * mColCount)
        {
            tmpRow = count / mColCount;
            tmpCol = count % mColCount;
            if(mRectPoint[tmpRow][tmpCol])
            {
                mTargetString.append("#");
            }
            else
            {
                textDrawn = mText.left(1);
                //如果是汉字则判断两位
                if(textDrawn.contains(QRegExp("[\\x4e00-\\x9fa5]+")))
                {
                    count++;
                    if(count == mRowCount * mColCount)
                    {
                        break;
                    }
                    if(count % mColCount == 0)
                    {
                        continue;
                    }
                    if(mRectPoint[count / mColCount][count % mColCount])
                    {
                        mTargetString.append("#");
                    }
                    else
                    {
                        mTargetString.append(textDrawn);
                        painter.drawText(x() + mWordWidth * (tmpCol), y() + mWordHeight * (tmpRow + 1), textDrawn);
                        mText.remove(0, 1);
                    }
                }
                else
                {
                    mTargetString.append(textDrawn);
                    if(textDrawn.compare("\n") == 0)
                    {
                        //换行符处理
                        count = (count / mColCount + 1) * mColCount - 1;
                    }
                    else
                    {
                        painter.drawText(x() + mWordWidth * (tmpCol), y() + mWordHeight * (tmpRow + 1), textDrawn);
                    }
                    mText.remove(0, 1);
                }
            }
            count++;
        }
        painter.restore();
        qDebug() << mText;
        mText = mTextStore;
        qDebug() << mTargetString;
    }
    else
    {
        QRect rect(sceneRect());
        rect.setWidth(contentWidth());
        rect.setHeight(contentHeight());

        QPen pen(mTextColor);
        painter.save();
        painter.setFont(mFont);
        painter.setPen(pen);
        painter.drawText(rect, mTextAlignment | Qt::TextWordWrap, currentText());
        painter.restore();
    }
}

QVariantMap TextBox::toJsonObject()
{
    QVariantMap object = Object::toJsonObject();
    QVariantList color;
    color << mTextColor.red() << mTextColor.green() << mTextColor.blue()
          << mTextColor.alpha();

    object.insert("textColor", color);
    object.insert("text", mText);
    object.insert("textAlignment", textAlignmentAsString());
    if (mFont != Object::defaultFont())
        object.insert("font", Utils::font(mFont.pixelSize(), mFont.family()));

    filterResourceData(object);

    return object;
}


QString TextBox::currentText()
{
    if (! mPlaceholderText.isEmpty())
        return mPlaceholderText;
    return mText;
}

Qt::Alignment TextBox::textAlignment()
{
    return mTextAlignment;
}

QString TextBox::textAlignmentAsString()
{
    QString textAlignment("");
    if (mTextAlignment.testFlag(Qt::AlignHCenter))
        textAlignment += "|HCenter";
    else if (mTextAlignment.testFlag(Qt::AlignLeft))
        textAlignment += "|Left";
    else if (mTextAlignment.testFlag(Qt::AlignRight))
        textAlignment += "|Right";

    if (mTextAlignment.testFlag(Qt::AlignVCenter))
        textAlignment += "|VCenter";
    else if (mTextAlignment.testFlag(Qt::AlignTop))
        textAlignment += "|Top";
    else if (mTextAlignment.testFlag(Qt::AlignBottom))
        textAlignment += "|Bottom";

    if (textAlignment.startsWith("|"))
        textAlignment = textAlignment.mid(1);

    return textAlignment;
}

void TextBox::setTextAlignment(Qt::Alignment alignment)
{
    mTextAlignment = alignment;
    emit dataChanged();
}

int TextBox::fontSize()
{
    return mFont.pixelSize();
}

void TextBox::setFontSize(int size)
{
    mFont.setPixelSize(size);
    emit dataChanged();
}

QString TextBox::fontFamily()
{
    return mFont.family();
}

void TextBox::setFontFamily(const QString& family)
{
    mFont.setFamily(family);
    emit dataChanged();
}



