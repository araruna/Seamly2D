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
 **  @file   vstandardtablecell.h
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

#ifndef VMULTISIZETABLEROW_H
#define VMULTISIZETABLEROW_H

#include <qcompilerdetection.h>
#include <QMap>
#include <QSharedDataPointer>
#include <QString>
#include <QStringList>
#include <QTypeInfo>
#include <QtGlobal>

#include "../vmisc/def.h"
#include "../ifc/ifcdef.h"
#include "vvariable.h"

class VContainer;
class VMeasurementData;

/**
 * @brief The VMeasurement class keep data row of multisize table
 */
class VMeasurement :public VVariable
{
public:
    VMeasurement(quint32 index, const QString &name, qreal baseSize, qreal baseHeight, const qreal &base,
                 const qreal &ksize, const qreal &kheight, const QString &gui_text = QString(),
                 const QString &description = QString(), const QString &tagName = QString());
    VMeasurement(VContainer *data, quint32 index, const QString &name, const qreal &base, const QString &formula,
                 bool ok, const QString &gui_text = QString(), const QString &description = QString(),
                 const QString &tagName = QString());
    VMeasurement(const VMeasurement &m);

    virtual ~VMeasurement() Q_DECL_OVERRIDE;

    VMeasurement &operator=(const VMeasurement &m);
#ifdef Q_COMPILER_RVALUE_REFS
	VMeasurement &operator=(VMeasurement &&m) Q_DECL_NOTHROW;
#endif

	void Swap(VMeasurement &m) Q_DECL_NOTHROW;

    QString getGuiText() const;

    QString TagName() const;
    void    setTagName(const QString &tagName);

    QString GetFormula() const;

    bool    isCustom() const;

    int     Index() const;
    bool    IsFormulaOk() const;

    virtual bool isNotUsed() const Q_DECL_OVERRIDE;

    virtual qreal  GetValue() const Q_DECL_OVERRIDE;
    virtual qreal* GetValue() Q_DECL_OVERRIDE;

    VContainer *GetData();

    void SetSize(qreal *size);
    void SetHeight(qreal *height);
    void SetUnit(const Unit *unit);

    qreal   GetBase() const;
    void    SetBase(const qreal &value);

    qreal   GetKsize() const;
    void    SetKsize(const qreal &value);

    qreal   GetKheight() const;
    void    SetKheight(const qreal &value);

    static QStringList ListHeights(QMap<GHeights, bool> heights, Unit patternUnit);
    static QStringList ListSizes(QMap<GSizes, bool> sizes, Unit patternUnit);
    static QStringList WholeListHeights(Unit patternUnit);
    static QStringList WholeListSizes(Unit patternUnit);
    static bool IsGradationSizeValid(const QString &size);
    static bool IsGradationHeightValid(const QString &height);
private:
    QSharedDataPointer<VMeasurementData> d;

    qreal CalcValue() const;

    static void        ListValue(QStringList &list, qreal value, Unit patternUnit);
};

Q_DECLARE_TYPEINFO(VMeasurement, Q_MOVABLE_TYPE);

#endif // VMULTISIZETABLEROW_H
