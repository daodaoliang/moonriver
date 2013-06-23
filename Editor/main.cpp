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

#include <QtGui/QApplication>

#include <QTimer>
#include <QDebug>
#include <QChar>
#include <QTextCodec>
#include "belle.h"
#include <QFile>
#include <QMessageBox>
int main(int argc, char ** argv)
{
    QApplication app(argc, argv);
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF8"));
    Belle belle;
    if(!QFile::exists(QApplication::applicationDirPath()+"/skin.qss"))
    {
        QMessageBox::warning(&belle,QObject::tr("警告"),QObject::tr("亲~~,皮肤文件没有找到,请查看该路径下的文件，存在否o(╯□╰)o\n%1")
                             .arg(QApplication::applicationDirPath()+"/skin.qss"));
        return 0;
    }
    QFile tempFile(QApplication::applicationDirPath()+"/skin.qss");
    tempFile.open(QIODevice::ReadOnly);
    belle.setStyleSheet(tempFile.readAll());
    belle.show();
    tempFile.close();
    return app.exec();
}
