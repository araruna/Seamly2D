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
 **  @file   vtoolheight.cpp
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   November 15, 2013
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

#include "vtoolheight.h"

#include <QSharedPointer>
#include <QStaticStringData>
#include <QStringData>
#include <QStringDataPtr>
#include <new>

#include "../../../../../dialogs/tools/dialogheight.h"
#include "../../../../../dialogs/tools/dialogtool.h"
#include "../../../../../visualization/visualization.h"
#include "../../../../../visualization/line/vistoolheight.h"
#include "../ifc/exception/vexception.h"
#include "../ifc/ifcdef.h"
#include "../vgeometry/vgobject.h"
#include "../vgeometry/vpointf.h"
#include "../vpatterndb/vcontainer.h"
#include "../vwidgets/vmaingraphicsscene.h"
#include "../../../../vabstracttool.h"
#include "../../../vdrawtool.h"
#include "vtoollinepoint.h"

template <class T> class QSharedPointer;

const QString VToolHeight::ToolType = QStringLiteral("height");

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief VToolHeight constructor.
 * @param doc dom document container.
 * @param data container with variables.
 * @param id object id in container.
 * @param lineType line type.
 * @param lineWeight line weight.
 * @param lineColor line color.
 * @param basePointId id base point of projection.
 * @param p1LineId id first point of line.
 * @param p2LineId id second point of line.
 * @param typeCreation way we create this tool.
 * @param parent parent object.
 */
VToolHeight::VToolHeight(VAbstractPattern *doc, VContainer *data, const quint32 &id, const QString &lineType,
                         const QString &lineWeight, const QString &lineColor, const quint32 &basePointId,
                         const quint32 &p1LineId, const quint32 &p2LineId,
                         const Source &typeCreation, QGraphicsItem * parent)
    : VToolLinePoint(doc, data, id, lineType, lineWeight, lineColor, QString()
    , basePointId, 0, parent)
    , p1LineId(p1LineId)
    , p2LineId(p2LineId)
{
    ToolCreation(typeCreation);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief setDialog set dialog when user want change tool option.
 */
void VToolHeight::setDialog()
{
    SCASSERT(not m_dialog.isNull())
    QSharedPointer<DialogHeight> dialogTool = m_dialog.objectCast<DialogHeight>();
    SCASSERT(not dialogTool.isNull())
    const QSharedPointer<VPointF> p = VAbstractTool::data.GeometricObject<VPointF>(m_id);
    dialogTool->setLineType(m_lineType);
    dialogTool->setLineWeight(m_lineWeight);
    dialogTool->setLineColor(lineColor);
    dialogTool->SetBasePointId(basePointId);
    dialogTool->SetP1LineId(p1LineId);
    dialogTool->SetP2LineId(p2LineId);
    dialogTool->SetPointName(p->name());
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Create help create tool from GUI.
 * @param dialog dialog.
 * @param scene pointer to scene.
 * @param doc dom document container.
 * @param data container with variables.
 * @return the created tool
 */
VToolHeight* VToolHeight::Create(QSharedPointer<DialogTool> dialog, VMainGraphicsScene *scene, VAbstractPattern *doc,
                                 VContainer *data)
{
    SCASSERT(not dialog.isNull())
    QSharedPointer<DialogHeight> dialogTool = dialog.objectCast<DialogHeight>();
    SCASSERT(not dialogTool.isNull())
    const QString pointName   = dialogTool->getPointName();
    const QString lineType    = dialogTool->getLineType();
    const QString lineWeight  = dialogTool->getLineWeight();
    const QString lineColor   = dialogTool->getLineColor();
    const quint32 basePointId = dialogTool->GetBasePointId();
    const quint32 p1LineId    = dialogTool->GetP1LineId();
    const quint32 p2LineId    = dialogTool->GetP2LineId();

    VToolHeight *point = Create(0, pointName, lineType, lineWeight, lineColor, basePointId, p1LineId, p2LineId, 5, 10, true,
                                scene, doc, data, Document::FullParse, Source::FromGui);
    if (point != nullptr)
    {
        point->m_dialog = dialogTool;
    }
    return point;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Create help create tool
 * @param _id tool id, 0 if tool doesn't exist yet.
 * @param pointName point name.
 * @param lineType line type.
 * @param lineWeight line weight.
 * @param lineColor line color.
 * @param basePointId id base point of projection.
 * @param p1LineId id first point of line.
 * @param p2LineId id second point of line.
 * @param mx label bias x axis.
 * @param my label bias y axis.
 * @param showPointName show/hide point name text
 * @param scene pointer to scene.
 * @param doc dom document container.
 * @param data container with variables.
 * @param parse parser file mode.
 * @param typeCreation way we create this tool.
 * @return the created tool
 */
VToolHeight* VToolHeight::Create(const quint32 _id, const QString &pointName, const QString &lineType,
                                 const QString &lineWeight, const QString &lineColor, quint32 basePointId,
                                 quint32 p1LineId, quint32 p2LineId,
                                 qreal mx, qreal my, bool showPointName, VMainGraphicsScene *scene,
                                 VAbstractPattern *doc, VContainer *data, const Document &parse,
                                 const Source &typeCreation)
{
    const QSharedPointer<VPointF> basePoint = data->GeometricObject<VPointF>(basePointId);
    const QSharedPointer<VPointF> p1Line = data->GeometricObject<VPointF>(p1LineId);
    const QSharedPointer<VPointF> p2Line = data->GeometricObject<VPointF>(p2LineId);

    QPointF pHeight = FindPoint(QLineF(static_cast<QPointF>(*p1Line), static_cast<QPointF>(*p2Line)),
                                static_cast<QPointF>(*basePoint));
    quint32 id = _id;
    VPointF *p = new VPointF(pHeight, pointName, mx, my);
    p->setShowPointName(showPointName);

    if (typeCreation == Source::FromGui)
    {
        id = data->AddGObject(p);
        data->AddLine(basePointId, id);
        data->AddLine(p1LineId, id);
        data->AddLine(p2LineId, id);
    }
    else
    {
        data->UpdateGObject(id, p);
        data->AddLine(basePointId, id);
        data->AddLine(p1LineId, id);
        data->AddLine(p2LineId, id);
        if (parse != Document::FullParse)
        {
            doc->UpdateToolData(id, data);
        }
    }

    if (parse == Document::FullParse)
    {
        VDrawTool::AddRecord(id, Tool::Height, doc);
        VToolHeight *point = new VToolHeight(doc, data, id, lineType, lineWeight, lineColor, basePointId, p1LineId, p2LineId,
                                             typeCreation);
        scene->addItem(point);
        InitToolConnections(scene, point);
        VAbstractPattern::AddTool(id, point);
        doc->IncrementReferens(basePoint->getIdTool());
        doc->IncrementReferens(p1Line->getIdTool());
        doc->IncrementReferens(p2Line->getIdTool());
        return point;
    }
    return nullptr;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief FindPoint find projection base point onto line.
 * @param line line
 * @param point base point.
 * @return point onto line.
 */
QPointF VToolHeight::FindPoint(const QLineF &line, const QPointF &point)
{
    return VGObject::ClosestPoint(line, point);
}

//---------------------------------------------------------------------------------------------------------------------
QString VToolHeight::FirstLinePointName() const
{
    return VAbstractTool::data.GetGObject(p1LineId)->name();
}

//---------------------------------------------------------------------------------------------------------------------
QString VToolHeight::SecondLinePointName() const
{
    return VAbstractTool::data.GetGObject(p2LineId)->name();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief contextMenuEvent handle context menu events.
 * @param event context menu event.
 */
void VToolHeight::showContextMenu(QGraphicsSceneContextMenuEvent *event, quint32 id)
{
    try
    {
        ContextMenu<DialogHeight>(event, id);
    }
    catch(const VExceptionToolWasDeleted &e)
    {
        Q_UNUSED(e)
        return;//Leave this method immediately!!!
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief SaveDialog save options into file after change in dialog.
 */
void VToolHeight::SaveDialog(QDomElement &domElement)
{
    SCASSERT(not m_dialog.isNull())
    QSharedPointer<DialogHeight> dialogTool = m_dialog.objectCast<DialogHeight>();
    SCASSERT(not dialogTool.isNull())
    doc->SetAttribute(domElement, AttrName,       dialogTool->getPointName());
    doc->SetAttribute(domElement, AttrLineType,   dialogTool->getLineType());
    doc->SetAttribute(domElement, AttrLineWeight, dialogTool->getLineWeight());
    doc->SetAttribute(domElement, AttrLineColor,  dialogTool->getLineColor());
    doc->SetAttribute(domElement, AttrBasePoint,  QString().setNum(dialogTool->GetBasePointId()));
    doc->SetAttribute(domElement, AttrP1Line,     QString().setNum(dialogTool->GetP1LineId()));
    doc->SetAttribute(domElement, AttrP2Line,     QString().setNum(dialogTool->GetP2LineId()));
}

//---------------------------------------------------------------------------------------------------------------------
void VToolHeight::SaveOptions(QDomElement &tag, QSharedPointer<VGObject> &obj)
{
    VToolLinePoint::SaveOptions(tag, obj);

    doc->SetAttribute(tag, AttrType,      ToolType);
    doc->SetAttribute(tag, AttrBasePoint, basePointId);
    doc->SetAttribute(tag, AttrP1Line,    p1LineId);
    doc->SetAttribute(tag, AttrP2Line,    p2LineId);
}

//---------------------------------------------------------------------------------------------------------------------
void VToolHeight::ReadToolAttributes(const QDomElement &domElement)
{
    m_lineType   = doc->GetParametrString(domElement, AttrLineType,   LineTypeSolidLine);
    m_lineWeight = doc->GetParametrString(domElement, AttrLineWeight, "0.35");
    lineColor    = doc->GetParametrString(domElement, AttrLineColor,  ColorBlack);
    basePointId  = doc->GetParametrUInt(domElement,   AttrBasePoint,  NULL_ID_STR);
    p1LineId     = doc->GetParametrUInt(domElement,   AttrP1Line,     NULL_ID_STR);
    p2LineId     = doc->GetParametrUInt(domElement,   AttrP2Line,     NULL_ID_STR);
}

//---------------------------------------------------------------------------------------------------------------------
void VToolHeight::SetVisualization()
{
    if (not vis.isNull())
    {
        VisToolHeight *visual = qobject_cast<VisToolHeight *>(vis);
        SCASSERT(visual != nullptr)

        visual->setObject1Id(basePointId);
        visual->setLineP1Id(p1LineId);
        visual->setLineP2Id(p2LineId);
        visual->setLineStyle(lineTypeToPenStyle(m_lineType));
        visual->setLineWeight(m_lineWeight);
        visual->RefreshGeometry();
    }
}

//---------------------------------------------------------------------------------------------------------------------
QString VToolHeight::makeToolTip() const
{
    const QSharedPointer<VPointF> basePoint = VAbstractTool::data.GeometricObject<VPointF>(basePointId);
    const QSharedPointer<VPointF> p1Line = VAbstractTool::data.GeometricObject<VPointF>(p1LineId);
    const QSharedPointer<VPointF> p2Line = VAbstractTool::data.GeometricObject<VPointF>(p2LineId);
    const QSharedPointer<VPointF> current = VAbstractTool::data.GeometricObject<VPointF>(m_id);

    const QLineF curLine(static_cast<QPointF>(*basePoint), static_cast<QPointF>(*current));
    const QLineF p1ToCur(static_cast<QPointF>(*p1Line), static_cast<QPointF>(*current));
    const QLineF p2ToCur(static_cast<QPointF>(*p2Line), static_cast<QPointF>(*current));

    const QString toolTip = QString("<table>"
                                    "<tr> <td><b>  %10:</b> %11</td> </tr>"
                                    "<tr> <td><b>%1:</b> %2 %3</td> </tr>"
                                    "<tr> <td><b>  %4:</b> %5°</td> </tr>"
                                    "<tr> <td><b>%6:</b> %7 %3</td> </tr>"
                                    "<tr> <td><b>%8:</b> %9 %3</td> </tr>"
                                    "</table>")
            .arg(tr("Length"))
            .arg(qApp->fromPixel(curLine.length()))
            .arg(UnitsToStr(qApp->patternUnit(), true))
            .arg(tr("Angle"))
            .arg(curLine.angle())
            .arg(QString("%1->%2").arg(p1Line->name(), current->name()))
            .arg(qApp->fromPixel(p1ToCur.length()))
            .arg(QString("%1->%2").arg(p2Line->name(), current->name()))
            .arg(qApp->fromPixel(p2ToCur.length()))
            .arg(tr("Name"))
            .arg(current->name());

    return toolTip;
}

//---------------------------------------------------------------------------------------------------------------------
quint32 VToolHeight::GetP2LineId() const
{
    return p2LineId;
}

//---------------------------------------------------------------------------------------------------------------------
void VToolHeight::SetP2LineId(const quint32 &value)
{
    if (value != NULL_ID)
    {
        p2LineId = value;

        QSharedPointer<VGObject> obj = VAbstractTool::data.GetGObject(m_id);
        SaveOption(obj);
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VToolHeight::ShowVisualization(bool show)
{
    ShowToolVisualization<VisToolHeight>(show);
}

//---------------------------------------------------------------------------------------------------------------------
quint32 VToolHeight::GetP1LineId() const
{
    return p1LineId;
}

//---------------------------------------------------------------------------------------------------------------------
void VToolHeight::SetP1LineId(const quint32 &value)
{
    if (value != NULL_ID)
    {
        p1LineId = value;

        QSharedPointer<VGObject> obj = VAbstractTool::data.GetGObject(m_id);
        SaveOption(obj);
    }
}
