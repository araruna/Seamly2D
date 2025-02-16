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
 **  @file   vtoolpointofintersectioncurves.cpp
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   22 1, 2016
 **
 **  @brief
 **  @copyright
 **  This source code is part of the Valentine project, a pattern making
 **  program, whose allow create and modeling patterns of clothing.
 **  Copyright (C) 2016 Seamly2D project
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

#include "vtoolpointofintersectioncurves.h"

#include "vtoolsinglepoint.h"
#include "../ifc/ifcdef.h"
#include "../ifc/exception/vexception.h"
#include "../vgeometry/vabstractcurve.h"
#include "../vgeometry/vgobject.h"
#include "../vgeometry/vpointf.h"
#include "../vmisc/vabstractapplication.h"
#include "../vpatterndb/vcontainer.h"
#include "../vwidgets/vmaingraphicsscene.h"
#include "../../vdrawtool.h"
#include "../../../vabstracttool.h"
#include "../../../../dialogs/tools/dialogtool.h"
#include "../../../../dialogs/tools/dialogpointofintersectioncurves.h"
#include "../../../../visualization/visualization.h"
#include "../../../../visualization/path/vistoolpointofintersectioncurves.h"

#include <QLineF>
#include <QMessageBox>
#include <QSharedPointer>
#include <QStaticStringData>
#include <QStringData>
#include <QStringDataPtr>
#include <new>

template <class T> class QSharedPointer;

const QString VToolPointOfIntersectionCurves::ToolType = QStringLiteral("pointOfIntersectionCurves");

//---------------------------------------------------------------------------------------------------------------------
VToolPointOfIntersectionCurves::VToolPointOfIntersectionCurves(VAbstractPattern *doc, VContainer *data,
                                                               const quint32 &id, const quint32 firstCurveId,
                                                               quint32 secondCurveId, VCrossCurvesPoint vCrossPoint,
                                                               HCrossCurvesPoint hCrossPoint, const Source &typeCreation,
                                                               QGraphicsItem *parent)
    :VToolSinglePoint(doc, data, id, QColor(qApp->Settings()->getPointNameColor()), parent),
      firstCurveId(firstCurveId),
      secondCurveId(secondCurveId),
      vCrossPoint(vCrossPoint),
      hCrossPoint(hCrossPoint)
{
    ToolCreation(typeCreation);
}

//---------------------------------------------------------------------------------------------------------------------
void VToolPointOfIntersectionCurves::setDialog()
{
    SCASSERT(not m_dialog.isNull())
    auto dialogTool = qobject_cast<DialogPointOfIntersectionCurves*>(m_dialog);
    SCASSERT(dialogTool != nullptr)
    auto p = VAbstractTool::data.GeometricObject<VPointF>(m_id);
    dialogTool->SetFirstCurveId(firstCurveId);
    dialogTool->SetSecondCurveId(secondCurveId);
    dialogTool->SetVCrossPoint(vCrossPoint);
    dialogTool->SetHCrossPoint(hCrossPoint);
    dialogTool->SetPointName(p->name());
}

//---------------------------------------------------------------------------------------------------------------------
VToolPointOfIntersectionCurves *VToolPointOfIntersectionCurves::Create(QSharedPointer<DialogTool> dialog,
                                                                       VMainGraphicsScene *scene,
                                                                       VAbstractPattern *doc, VContainer *data)
{
    SCASSERT(not dialog.isNull())
    QSharedPointer<DialogPointOfIntersectionCurves> dialogTool = dialog.objectCast<DialogPointOfIntersectionCurves>();
    SCASSERT(not dialogTool.isNull())
    const quint32 firstCurveId = dialogTool->GetFirstCurveId();
    const quint32 secondCurveId = dialogTool->GetSecondCurveId();
    const VCrossCurvesPoint vCrossPoint = dialogTool->GetVCrossPoint();
    const HCrossCurvesPoint hCrossPoint = dialogTool->GetHCrossPoint();
    const QString pointName = dialogTool->getPointName();
    VToolPointOfIntersectionCurves *point = Create(0, pointName, firstCurveId, secondCurveId, vCrossPoint, hCrossPoint,
                                                   5, 10, true, scene, doc, data, Document::FullParse, Source::FromGui);
    if (point != nullptr)
    {
        point->m_dialog = dialogTool;
    }
    return point;
}

//---------------------------------------------------------------------------------------------------------------------
VToolPointOfIntersectionCurves *VToolPointOfIntersectionCurves::Create(const quint32 _id, const QString &pointName,
                                                                       quint32 firstCurveId,
                                                                       quint32 secondCurveId,
                                                                       VCrossCurvesPoint vCrossPoint,
                                                                       HCrossCurvesPoint hCrossPoint,
                                                                       qreal mx, qreal my,
                                                                       bool showPointName,
                                                                       VMainGraphicsScene *scene,
                                                                       VAbstractPattern *doc, VContainer *data,
                                                                       const Document &parse,
                                                                       const Source &typeCreation)
{
    auto curve1 = data->GeometricObject<VAbstractCurve>(firstCurveId);
    auto curve2 = data->GeometricObject<VAbstractCurve>(secondCurveId);

    const QPointF point = VToolPointOfIntersectionCurves::FindPoint(curve1->getPoints(), curve2->getPoints(),
                                                                    vCrossPoint, hCrossPoint);

    if (point == QPointF())
    {
        const QString msg = tr("<b><big>Can't find intersection point %1 of Curves</big></b><br>"
                               "Using origin point as a place holder until pattern is corrected.")
                               .arg(pointName);

        QMessageBox msgBox(qApp->getMainWindow());
        msgBox.setWindowTitle(tr("Point Intersect Curves"));
        msgBox.setWindowFlags(msgBox.windowFlags() & ~Qt::WindowContextHelpButtonHint);
        msgBox.setWindowIcon(QIcon(":/toolicon/32x32/intersection_curves.png"));
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText(msg);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
    }
    quint32 id = _id;

    VPointF *p = new VPointF(point, pointName, mx, my);
    p->setShowPointName(showPointName);

    if (typeCreation == Source::FromGui)
    {
        id = data->AddGObject(p);
    }
    else
    {
        data->UpdateGObject(id, p);
        if (parse != Document::FullParse)
        {
            doc->UpdateToolData(id, data);
        }
    }

    if (parse == Document::FullParse)
    {
        VDrawTool::AddRecord(id, Tool::PointOfIntersectionCurves, doc);
        auto point = new VToolPointOfIntersectionCurves(doc, data, id, firstCurveId, secondCurveId, vCrossPoint,
                                                        hCrossPoint, typeCreation);
        scene->addItem(point);
        InitToolConnections(scene, point);
        VAbstractPattern::AddTool(id, point);
        doc->IncrementReferens(curve1->getIdTool());
        doc->IncrementReferens(curve2->getIdTool());
        return point;
    }
    return nullptr;
}

//---------------------------------------------------------------------------------------------------------------------
QPointF VToolPointOfIntersectionCurves::FindPoint(const QVector<QPointF> &curve1Points,
                                                  const QVector<QPointF> &curve2Points,
                                                  VCrossCurvesPoint vCrossPoint, HCrossCurvesPoint hCrossPoint)
{
    if (curve1Points.isEmpty() || curve2Points.isEmpty())
    {
        return QPointF();
    }

    QVector<QPointF> intersections;
    for ( auto i = 0; i < curve1Points.count()-1; ++i )
    {
        intersections << VAbstractCurve::CurveIntersectLine(curve2Points,
                                                            QLineF(curve1Points.at(i), curve1Points.at(i+1)));
    }

    if (intersections.isEmpty())
    {
        return QPointF();
    }

    if (intersections.size() == 1)
    {
        return intersections.at(0);
    }

    QVector<QPointF> vIntersections;
    if (vCrossPoint == VCrossCurvesPoint::HighestPoint)
    {
        qreal minY = intersections.at(0).y();
        vIntersections.append(intersections.at(0));

        for ( auto i = 1; i < intersections.count(); ++i )
        {
            const QPointF p = intersections.at(i);
            if (p.y() > minY)
            {
                continue;
            }
            else if (p.y() < minY)
            {
                minY = p.y();
                vIntersections.clear();
                vIntersections.append(p);
            }
            else
            {
                vIntersections.append(p);
            }
        }
    }
    else
    {
        qreal maxY = intersections.at(0).y();
        vIntersections.append(intersections.at(0));

        for ( auto i = 1; i < intersections.count(); ++i )
        {
            const QPointF p = intersections.at(i);
            if (p.y() > maxY)
            {
                maxY = p.y();
                vIntersections.clear();
                vIntersections.append(p);
            }
            else if (p.y() < maxY)
            {
                continue;
            }
            else
            {
                vIntersections.append(p);
            }
        }
    }

    if (vIntersections.isEmpty())
    {
        return QPointF();
    }

    if (vIntersections.size() == 1)
    {
        return vIntersections.at(0);
    }

    QPointF crossPoint = vIntersections.at(0);

    if (hCrossPoint == HCrossCurvesPoint::RightmostPoint)
    {
        qreal maxX = vIntersections.at(0).x();

        for ( auto i = 1; i < vIntersections.count(); ++i )
        {
            const QPointF p = vIntersections.at(i);
            if (p.x() > maxX)
            {
                maxX = p.x();
                crossPoint = p;
            }
        }
    }
    else
    {
        qreal minX = vIntersections.at(0).x();

        for ( auto i = 1; i < vIntersections.count(); ++i )
        {
            const QPointF p = vIntersections.at(i);
            if (p.x() < minX)
            {
                minX = p.x();
                crossPoint = p;
            }
        }
    }

    return crossPoint;
}

//---------------------------------------------------------------------------------------------------------------------
QString VToolPointOfIntersectionCurves::FirstCurveName() const
{
    return VAbstractTool::data.GetGObject(firstCurveId)->name();
}

//---------------------------------------------------------------------------------------------------------------------
QString VToolPointOfIntersectionCurves::SecondCurveName() const
{
    return VAbstractTool::data.GetGObject(secondCurveId)->name();
}

//---------------------------------------------------------------------------------------------------------------------
quint32 VToolPointOfIntersectionCurves::GetFirstCurveId() const
{
    return firstCurveId;
}

//---------------------------------------------------------------------------------------------------------------------
void VToolPointOfIntersectionCurves::SetFirstCurveId(const quint32 &value)
{
    if (value != NULL_ID)
    {
        firstCurveId = value;

        auto obj = VAbstractTool::data.GetGObject(m_id);
        SaveOption(obj);
    }
}

//---------------------------------------------------------------------------------------------------------------------
quint32 VToolPointOfIntersectionCurves::GetSecondCurveId() const
{
    return secondCurveId;
}

//---------------------------------------------------------------------------------------------------------------------
void VToolPointOfIntersectionCurves::SetSecondCurveId(const quint32 &value)
{
    if (value != NULL_ID)
    {
        secondCurveId = value;

        auto obj = VAbstractTool::data.GetGObject(m_id);
        SaveOption(obj);
    }
}

//---------------------------------------------------------------------------------------------------------------------
VCrossCurvesPoint VToolPointOfIntersectionCurves::GetVCrossPoint() const
{
    return vCrossPoint;
}

//---------------------------------------------------------------------------------------------------------------------
void VToolPointOfIntersectionCurves::SetVCrossPoint(const VCrossCurvesPoint &value)
{
    vCrossPoint = value;

    auto obj = VAbstractTool::data.GetGObject(m_id);
    SaveOption(obj);
}

//---------------------------------------------------------------------------------------------------------------------
HCrossCurvesPoint VToolPointOfIntersectionCurves::GetHCrossPoint() const
{
    return hCrossPoint;
}

//---------------------------------------------------------------------------------------------------------------------
void VToolPointOfIntersectionCurves::SetHCrossPoint(const HCrossCurvesPoint &value)
{
    hCrossPoint = value;

    auto obj = VAbstractTool::data.GetGObject(m_id);
    SaveOption(obj);
}

//---------------------------------------------------------------------------------------------------------------------
void VToolPointOfIntersectionCurves::ShowVisualization(bool show)
{
    ShowToolVisualization<VisToolPointOfIntersectionCurves>(show);
}

//---------------------------------------------------------------------------------------------------------------------
void VToolPointOfIntersectionCurves::RemoveReferens()
{
    const auto curve1 = VAbstractTool::data.GetGObject(firstCurveId);
    const auto curve2 = VAbstractTool::data.GetGObject(secondCurveId);

    doc->DecrementReferens(curve1->getIdTool());
    doc->DecrementReferens(curve2->getIdTool());
}

//---------------------------------------------------------------------------------------------------------------------
void VToolPointOfIntersectionCurves::showContextMenu(QGraphicsSceneContextMenuEvent *event, quint32 id)
{
    try
    {
        ContextMenu<DialogPointOfIntersectionCurves>(event, id);
    }
    catch(const VExceptionToolWasDeleted &e)
    {
        Q_UNUSED(e)
        return;//Leave this method immediately!!!
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VToolPointOfIntersectionCurves::SaveDialog(QDomElement &domElement)
{
    SCASSERT(not m_dialog.isNull())
    auto dialogTool = qobject_cast<DialogPointOfIntersectionCurves*>(m_dialog);
    SCASSERT(dialogTool != nullptr)
    doc->SetAttribute(domElement, AttrName, dialogTool->getPointName());
    doc->SetAttribute(domElement, AttrCurve1, QString().setNum(dialogTool->GetFirstCurveId()));
    doc->SetAttribute(domElement, AttrCurve2, QString().setNum(dialogTool->GetSecondCurveId()));
    doc->SetAttribute(domElement, AttrVCrossPoint, QString().setNum(static_cast<int>(dialogTool->GetVCrossPoint())));
    doc->SetAttribute(domElement, AttrHCrossPoint, QString().setNum(static_cast<int>(dialogTool->GetHCrossPoint())));
}

//---------------------------------------------------------------------------------------------------------------------
void VToolPointOfIntersectionCurves::SaveOptions(QDomElement &tag, QSharedPointer<VGObject> &obj)
{
    VToolSinglePoint::SaveOptions(tag, obj);

    doc->SetAttribute(tag, AttrType, ToolType);
    doc->SetAttribute(tag, AttrCurve1, firstCurveId);
    doc->SetAttribute(tag, AttrCurve2, secondCurveId);
    doc->SetAttribute(tag, AttrVCrossPoint, static_cast<int>(vCrossPoint));
    doc->SetAttribute(tag, AttrHCrossPoint, static_cast<int>(hCrossPoint));
}

//---------------------------------------------------------------------------------------------------------------------
void VToolPointOfIntersectionCurves::ReadToolAttributes(const QDomElement &domElement)
{
    firstCurveId = doc->GetParametrUInt(domElement, AttrCurve1, NULL_ID_STR);
    secondCurveId = doc->GetParametrUInt(domElement, AttrCurve2, NULL_ID_STR);
    vCrossPoint = static_cast<VCrossCurvesPoint>(doc->GetParametrUInt(domElement, AttrVCrossPoint, "1"));
    hCrossPoint = static_cast<HCrossCurvesPoint>(doc->GetParametrUInt(domElement, AttrHCrossPoint, "1"));
}

//---------------------------------------------------------------------------------------------------------------------
void VToolPointOfIntersectionCurves::SetVisualization()
{
    if (not vis.isNull())
    {
        auto visual = qobject_cast<VisToolPointOfIntersectionCurves *>(vis);
        SCASSERT(visual != nullptr)

        visual->setObject1Id(firstCurveId);
        visual->setObject2Id(secondCurveId);
        visual->setVCrossPoint(vCrossPoint);
        visual->setHCrossPoint(hCrossPoint);
        visual->RefreshGeometry();
    }
}
