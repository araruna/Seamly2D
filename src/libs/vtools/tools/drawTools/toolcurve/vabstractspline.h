/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2017  Seamly, LLC                                       *
 *                                                                         *
 *   https://github.com/fashionfreedom/seamly2d                            *
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
 **  @file   vabstractspline.h
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   4 3, 2014
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

#ifndef VABSTRACTSPLINE_H
#define VABSTRACTSPLINE_H

#include <qcompilerdetection.h>
#include <QDomElement>
#include <QGraphicsItem>
#include <QGraphicsPathItem>
#include <QMetaObject>
#include <QObject>
#include <QPainterPath>
#include <QPointF>
#include <QString>
#include <QVariant>
#include <QVector>
#include <QtGlobal>

#include "../../../visualization/line/visline.h"
#include "../vdrawtool.h"
#include "../vgeometry/vabstractcurve.h"
#include "../vgeometry/vgeometrydef.h"
#include "../vmisc/vabstractapplication.h"
#include "../vmisc/def.h"
#include "../vwidgets/vmaingraphicsscene.h"
#include "../vwidgets/vmaingraphicsview.h"

class VControlPointSpline;
template <class T> class QSharedPointer;

class VAbstractSpline:public VDrawTool, public QGraphicsPathItem
{
    Q_OBJECT
public:
                        VAbstractSpline(VAbstractPattern *doc, VContainer *data, quint32 id,
                                        QGraphicsItem * parent = nullptr);
    virtual            ~VAbstractSpline() Q_DECL_EQ_DEFAULT;

    virtual QPainterPath shape() const Q_DECL_OVERRIDE;
    virtual void         paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;
    virtual int          type() const  Q_DECL_OVERRIDE {return Type;}

    enum { Type = UserType + static_cast<int>(Tool::AbstractSpline)};

    virtual QString      getTagName() const Q_DECL_OVERRIDE;
    void                 ShowHandles(bool show);

    QString              getLineColor() const;
    void                 setLineColor(const QString &value);

    QString              GetPenStyle() const;
    void                 SetPenStyle(const QString &value);

    QString              getLineWeight() const;
    void                 setLineWeight(const QString &value);

    QString              name() const;

    virtual void         GroupVisibility(quint32 object, bool visible) Q_DECL_OVERRIDE;

public slots:
    virtual void         FullUpdateFromFile () Q_DECL_OVERRIDE;
    virtual void         Disable(bool disable, const QString &draftBlockName) Q_DECL_OVERRIDE;
    virtual void         piecesMode(bool mode) Q_DECL_OVERRIDE;
    virtual void         AllowHover(bool enabled) Q_DECL_OVERRIDE;
    virtual void         AllowSelecting(bool enabled) Q_DECL_OVERRIDE;
    virtual QString      makeToolTip() const Q_DECL_OVERRIDE;

signals:
    /**
     * @brief setEnabledPoint disable control points.
     * @param enable enable or diasable points.
     */
    void                 setEnabledPoint(bool enable);

protected:
    /**
     * @brief controlPoints list pointers of control points.
     */
    QVector<VControlPointSpline *> controlPoints;

    SceneObject          sceneType;
    bool                 m_isHovered;
    bool                 m_piecesMode;

    /**
     * @brief RefreshGeometry  refresh item on scene.
     */
    virtual void         RefreshGeometry();

    virtual void         ShowTool(quint32 id, bool enable) Q_DECL_OVERRIDE;
    virtual void         hoverEnterEvent ( QGraphicsSceneHoverEvent * event ) Q_DECL_OVERRIDE;
    virtual void         hoverLeaveEvent ( QGraphicsSceneHoverEvent * event ) Q_DECL_OVERRIDE;
    virtual QVariant     itemChange ( GraphicsItemChange change, const QVariant &value ) Q_DECL_OVERRIDE;
    virtual void         keyReleaseEvent(QKeyEvent * event) Q_DECL_OVERRIDE;
    virtual void         mousePressEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
    virtual void         mouseReleaseEvent ( QGraphicsSceneMouseEvent * event ) Q_DECL_OVERRIDE;
    virtual void         ReadToolAttributes(const QDomElement &domElement) Q_DECL_OVERRIDE;
    virtual void         SaveOptions(QDomElement &tag, QSharedPointer<VGObject> &obj) Q_DECL_OVERRIDE;
    virtual void         refreshCtrlPoints();
    virtual void         contextMenuEvent (QGraphicsSceneContextMenuEvent * event ) Q_DECL_OVERRIDE;

    VSpline              CorrectedSpline(const VSpline &spline, const SplinePointPosition &position,
                                         const QPointF &pos) const;

    template <typename T>
    void        ShowToolVisualization(bool show);

    template <typename T>
    static void InitSplineToolConnections(VMainGraphicsScene *scene, T *tool);

    template <typename T>
    static void InitSplinePathToolConnections(VMainGraphicsScene *scene, T *tool);

    template <typename T>
    static void InitArcToolConnections(VMainGraphicsScene *scene, T *tool);

    template <typename T>
    static void InitElArcToolConnections(VMainGraphicsScene *scene, T *tool);

private:
    Q_DISABLE_COPY(VAbstractSpline)

    void InitDefShape();
};

//---------------------------------------------------------------------------------------------------------------------
template <typename T>
inline void VAbstractSpline::ShowToolVisualization(bool show)
{
    if (show)
    {
        if (vis.isNull())
        {
            AddVisualization<T>();
            SetVisualization();
        }
        else
        {
            if (T *visual = qobject_cast<T *>(vis))
            {
                visual->show();
            }
        }
    }
    else
    {
        delete vis;
    }

    if (m_piecesMode)
    {
        ShowHandles(m_piecesMode);
    }
    else
    {
        ShowHandles(show);
    }

    if (QGraphicsScene *sc = scene())
    { // Showing/hiding control points require recalculation scene size.
        VMainGraphicsView::NewSceneRect(sc, qApp->getSceneView(), this);
    }
}

//---------------------------------------------------------------------------------------------------------------------
template <typename T>
void VAbstractSpline::InitSplineToolConnections(VMainGraphicsScene *scene, T *tool)
{
    SCASSERT(scene != nullptr)
    SCASSERT(tool != nullptr)

    InitDrawToolConnections(scene, tool);
    QObject::connect(scene, &VMainGraphicsScene::EnableSplineItemHover, tool, &T::AllowHover);
    QObject::connect(scene, &VMainGraphicsScene::EnableSplineItemSelection, tool, &T::AllowSelecting);
}

//---------------------------------------------------------------------------------------------------------------------
template <typename T>
void VAbstractSpline::InitSplinePathToolConnections(VMainGraphicsScene *scene, T *tool)
{
    SCASSERT(scene != nullptr)
    SCASSERT(tool != nullptr)

    InitDrawToolConnections(scene, tool);
    QObject::connect(scene, &VMainGraphicsScene::EnableSplinePathItemHover, tool, &T::AllowHover);
    QObject::connect(scene, &VMainGraphicsScene::EnableSplinePathItemSelection, tool, &T::AllowSelecting);
}

//---------------------------------------------------------------------------------------------------------------------
template <typename T>
void VAbstractSpline::InitArcToolConnections(VMainGraphicsScene *scene, T *tool)
{
    SCASSERT(scene != nullptr)
    SCASSERT(tool != nullptr)

    InitDrawToolConnections(scene, tool);
    QObject::connect(scene, &VMainGraphicsScene::EnableArcItemHover, tool, &T::AllowHover);
    QObject::connect(scene, &VMainGraphicsScene::EnableArcItemSelection, tool, &T::AllowSelecting);
}

//---------------------------------------------------------------------------------------------------------------------
template <typename T>
void VAbstractSpline::InitElArcToolConnections(VMainGraphicsScene *scene, T *tool)
{
    SCASSERT(scene != nullptr)
    SCASSERT(tool != nullptr)

    InitDrawToolConnections(scene, tool);
    QObject::connect(scene, &VMainGraphicsScene::EnableElArcItemHover, tool, &T::AllowHover);
    QObject::connect(scene, &VMainGraphicsScene::EnableElArcItemSelection, tool, &T::AllowSelecting);
}

#endif // VABSTRACTSPLINE_H
