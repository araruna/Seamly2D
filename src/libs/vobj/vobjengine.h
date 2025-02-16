/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2017  Seamly, LLC                                       *
 *                                                                         *
 *   https://github.com/fashionfreedom/seamly2d                             *
 *                                                                         *
 ***************************************************************************
 **
 **  Seamly2D is free software: you can redistribute it and/or modify
 **  it under the terms of the GNU General Public License as published by
 **  the Free Software Foundation, either version 3 of the License, or
 **  (at your option) any later version.
 **
 **  Seamly2D is distributed in the hope that it will be useful,
 **  but WITHOUT ANY WARRANTY; without even the implied warranty of
 **  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 **  GNU General Public License for more details.
 **
 **  You should have received a copy of the GNU General Public License
 **  along with Seamly2D.  If not, see <http://www.gnu.org/licenses/>.
 **
 **************************************************************************

 ************************************************************************
 **
 **  @file   vobjengine.h
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   12 12, 2014
 **
 **  @brief
 **  @copyright
 **  This source code is part of the Valentine project, a pattern making
 **  program, whose allow create and modeling patterns of clothing.
 **  Copyright (C) 2013-2015 Seamly2D project
 **  <https://github.com/fashionfreedom/seamly2d> All Rights Reserved.
 **
 **  Seamly2D is free software: you can redistribute it and/or modify
 **  it under the terms of the GNU General Public License as published by
 **  the Free Software Foundation, either version 3 of the License, or
 **  (at your option) any later version.
 **
 **  Seamly2D is distributed in the hope that it will be useful,
 **  but WITHOUT ANY WARRANTY; without even the implied warranty of
 **  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 **  GNU General Public License for more details.
 **
 **  You should have received a copy of the GNU General Public License
 **  along with Seamly2D.  If not, see <http://www.gnu.org/licenses/>.
 **
 *************************************************************************/

#ifndef VOBJENGINE_H
#define VOBJENGINE_H

#include <qcompilerdetection.h>
#include <QTransform>
#include <QPaintEngine>
#include <QPolygonF>
#include <QRectF>
#include <QSharedPointer>
#include <QSize>
#include <QtGlobal>

#include "delaunay.h"

class QTextStream;

#define MAX_POINTS      512

class VObjEngine : public QPaintEngine
{
public:
    VObjEngine();
    virtual ~VObjEngine() Q_DECL_OVERRIDE;

    virtual bool begin(QPaintDevice *pdev) Q_DECL_OVERRIDE;
    virtual bool end() Q_DECL_OVERRIDE;
    virtual void updateState(const QPaintEngineState &state) Q_DECL_OVERRIDE;
    virtual void drawPath(const QPainterPath &path) Q_DECL_OVERRIDE;
    virtual Type type() const Q_DECL_OVERRIDE;
    virtual void drawPoints(const QPointF *points, int pointCount) Q_DECL_OVERRIDE;
    virtual void drawPoints(const QPoint *points, int pointCount) Q_DECL_OVERRIDE;
    virtual void drawPixmap(const QRectF &r, const QPixmap &pm, const QRectF &sr) Q_DECL_OVERRIDE;
    virtual void drawPolygon(const QPointF *points, int pointCount, PolygonDrawMode mode) Q_DECL_OVERRIDE;
    virtual void drawPolygon(const QPoint *points, int pointCount, PolygonDrawMode mode) Q_DECL_OVERRIDE;

    QSize getSize() const;
    void setSize(const QSize &value);

    QIODevice *getOutputDevice() const;
    void setOutputDevice(QIODevice *value);

    int getResolution() const;
    void setResolution(int value);

private:
    Q_DISABLE_COPY(VObjEngine)
    QSharedPointer<QTextStream> stream;
    quint32     globalPointsCount;
    QSharedPointer<QIODevice> outputDevice;
    del_point2d_t    points[MAX_POINTS];
    quint32          planeCount;
    QSize            size;
    int              resolution;
    QTransform       transform;

    QPolygonF  MakePointsUnique(const QPolygonF &polygon)const;
    qint64     Square(const QPolygonF &poly)const;
};

#endif // VOBJENGINE_H
