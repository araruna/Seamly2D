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
 **  @file
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   3 11, 2016
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

#ifndef VABSTRACTPIECE_H
#define VABSTRACTPIECE_H

#include <QtGlobal>
#include <QSharedDataPointer>
#include <QPointF>
#include <QDebug>

#include "../vmisc/diagnostic.h"
#include "../vmisc/def.h"
#include "../vgeometry/vgobject.h"

template <class T> class QVector;

class VAbstractPieceData;

QT_WARNING_PUSH
QT_WARNING_DISABLE_GCC("-Weffc++")
QT_WARNING_DISABLE_GCC("-Wnon-virtual-dtor")

/**
 * @brief The VSAPoint class seam allowance point
 */
class VSAPoint : public QPointF
{
public:
    Q_DECL_CONSTEXPR VSAPoint();
    Q_DECL_CONSTEXPR VSAPoint(qreal xpos, qreal ypos);
    Q_DECL_CONSTEXPR explicit VSAPoint(const QPointF &p);

    Q_DECL_CONSTEXPR qreal GetSABefore() const;
                     qreal GetSABefore(qreal width) const;
                     void  SetSABefore(qreal value);

    Q_DECL_CONSTEXPR qreal GetSAAfter() const;
                     qreal GetSAAfter(qreal width) const;
                     void  SetSAAfter(qreal value);

    Q_DECL_CONSTEXPR PieceNodeAngle GetAngleType() const;
                     void           SetAngleType(PieceNodeAngle value);

private:
    qreal          m_before;
    qreal          m_after;
    PieceNodeAngle m_angle;
};

Q_DECLARE_METATYPE(VSAPoint)
Q_DECLARE_TYPEINFO(VSAPoint, Q_MOVABLE_TYPE);

//---------------------------------------------------------------------------------------------------------------------
Q_DECL_CONSTEXPR inline VSAPoint::VSAPoint()
    : QPointF(),
      m_before(-1),
      m_after(-1),
      m_angle(PieceNodeAngle::ByLength)
{}

//---------------------------------------------------------------------------------------------------------------------
Q_DECL_CONSTEXPR inline VSAPoint::VSAPoint(qreal xpos, qreal ypos)
    : QPointF(xpos, ypos),
      m_before(-1),
      m_after(-1),
      m_angle(PieceNodeAngle::ByLength)
{}

//---------------------------------------------------------------------------------------------------------------------
Q_DECL_CONSTEXPR inline VSAPoint::VSAPoint(const QPointF &p)
    : QPointF(p),
      m_before(-1),
      m_after(-1),
      m_angle(PieceNodeAngle::ByLength)
{}

//---------------------------------------------------------------------------------------------------------------------
Q_DECL_CONSTEXPR inline qreal VSAPoint::GetSABefore() const
{
    return m_before;
}

//---------------------------------------------------------------------------------------------------------------------
inline void VSAPoint::SetSABefore(qreal value)
{
    value < 0 ? m_before = -1 : m_before = value;
}

//---------------------------------------------------------------------------------------------------------------------
Q_DECL_CONSTEXPR inline qreal VSAPoint::GetSAAfter() const
{
    return m_after;
}

//---------------------------------------------------------------------------------------------------------------------
inline void VSAPoint::SetSAAfter(qreal value)
{
    value < 0 ? m_after = -1 : m_after = value;
}

//---------------------------------------------------------------------------------------------------------------------
Q_DECL_CONSTEXPR inline PieceNodeAngle VSAPoint::GetAngleType() const
{
    return m_angle;
}

//---------------------------------------------------------------------------------------------------------------------
inline void VSAPoint::SetAngleType(PieceNodeAngle value)
{
    m_angle = value;
}

QT_WARNING_POP

class VAbstractPiece
{
public:
    VAbstractPiece();
    VAbstractPiece(const VAbstractPiece &piece);
    virtual ~VAbstractPiece();

    VAbstractPiece &operator=(const VAbstractPiece &piece);
#ifdef Q_COMPILER_RVALUE_REFS
	VAbstractPiece &operator=(VAbstractPiece &&piece) Q_DECL_NOTHROW;
#endif

	void Swap(VAbstractPiece &piece) Q_DECL_NOTHROW;

    QString GetName() const;
    void    SetName(const QString &value);

    QString getColor() const;
    void    setColor(const QString &value);

    QString getFill() const;
    void    setFill(const QString &value);

    bool    getLock() const;
    void    setLock(bool value);

    bool IsForbidFlipping() const;
    void SetForbidFlipping(bool value);

    bool IsSeamAllowance() const;
    void SetSeamAllowance(bool value);

    bool IsSeamAllowanceBuiltIn() const;
    void SetSeamAllowanceBuiltIn(bool value);

    bool isHideSeamLine() const;
    void setHideSeamLine(bool value);

    qreal GetSAWidth() const;
    void  SetSAWidth(qreal value);

    qreal GetMx() const;
    void  SetMx(qreal value);

    qreal GetMy() const;
    void  SetMy(qreal value);

    static QVector<QPointF> Equidistant(const QVector<VSAPoint> &points, qreal width);
    static qreal            sumTrapezoids(const QVector<QPointF> &points);
    static bool             isClockwise(const QVector<QPointF> &points);
    static QVector<QPointF> CheckLoops(const QVector<QPointF> &points);
    static QVector<QPointF> EkvPoint(const VSAPoint &p1Line1, const VSAPoint &p2Line1,
                                     const VSAPoint &p1Line2, const VSAPoint &p2Line2, qreal width);
    static QLineF           createParallelLine(const VSAPoint &p1, const VSAPoint &p2, qreal width);
    static QLineF           createParallelLine(const QPointF &p1, const QPointF &p2, qreal width);

    template <class T>
    static QVector<T> CorrectEquidistantPoints(const QVector<T> &points, bool removeFirstAndLast = true);

protected:
    template <class T>
    static QVector<T> RemoveDublicates(const QVector<T> &points, bool removeFirstAndLast = true);
    static qreal      MaxLocalSA(const VSAPoint &p, qreal width);
    static bool       IsEkvPointOnLine(const QPointF &iPoint, const QPointF &prevPoint, const QPointF &nextPoint);
    static bool       IsEkvPointOnLine(const VSAPoint &iPoint, const VSAPoint &prevPoint, const VSAPoint &nextPoint);

private:
    QSharedDataPointer<VAbstractPieceData> d;

    static bool             CheckIntersection(const QVector<QPointF> &points, int i, int iNext, int j, int jNext,
                                              const QPointF &crossPoint);
    static bool             ParallelCrossPoint(const QLineF &line1, const QLineF &line2, QPointF &point);
    static bool             Crossing(const QVector<QPointF> &sub1, const QVector<QPointF> &sub2);
    static QVector<QPointF> SubPath(const QVector<QPointF> &path, int startIndex, int endIndex);
    static Q_DECL_CONSTEXPR qreal PointPosition(const QPointF &p, const QLineF &line);
    static QVector<QPointF> AngleByLength(const QPointF &p2, const QPointF &sp1, const QPointF &sp2, const QPointF &sp3,
                                          qreal width);
    static QVector<QPointF> AngleByIntersection(const QPointF &p1, const QPointF &p2, const QPointF &p3,
                                                const QPointF &sp1, const QPointF &sp2, const QPointF &sp3,
                                                qreal width);
    static QVector<QPointF> AngleByFirstSymmetry(const QPointF &p1, const QPointF &p2,
                                                 const QPointF &sp1, const QPointF &sp2, const QPointF &sp3,
                                                 qreal width);
    static QVector<QPointF> AngleBySecondSymmetry(const QPointF &p2, const QPointF &p3,
                                                  const QPointF &sp1, const QPointF &sp2, const QPointF &sp3,
                                                  qreal width);
    static QVector<QPointF> AngleByFirstRightAngle(const QPointF &p1, const QPointF &p2,
                                                   const QPointF &sp1, const QPointF &sp2, const QPointF &sp3,
                                                   qreal width);
    static QVector<QPointF> AngleBySecondRightAngle(const QPointF &p2, const QPointF &p3,
                                                    const QPointF &sp1, const QPointF &sp2, const QPointF &sp3,
                                                    qreal width);

    static QPointF          SingleParallelPoint(const QPointF &p1, const QPointF &p2, qreal angle, qreal width);
    static QLineF           BisectorLine(const QPointF &p1, const QPointF &p2, const QPointF &p3);
    static qreal            AngleBetweenBisectors(const QLineF &b1, const QLineF &b2);
};

Q_DECLARE_TYPEINFO(VAbstractPiece, Q_MOVABLE_TYPE);

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief CorrectEquidistantPoints clear equivalent points and remove point on line from equdistant.
 * @param points list of points equdistant.
 * @return corrected list.
 */
template <class T>
QVector<T> VAbstractPiece::CorrectEquidistantPoints(const QVector<T> &points, bool removeFirstAndLast)
{
    if (points.size()<4)//Better don't check if only three points. We can destroy equidistant.
    {
        qDebug()<<"Only three points.";
        return points;
    }

    //Clear equivalent points
    QVector<T> buf1 = RemoveDublicates(points, removeFirstAndLast);

    if (buf1.size()<3)
    {
        return buf1;
    }

    int prev = -1;

    QVector<T> buf2;
    //Remove point on line
    for (qint32 i = 0; i < buf1.size(); ++i)
    {// In this case we alwayse will have bounded intersection, so all is need is to check if point i is on line.
     // Unfortunatelly QLineF::intersect can't be used in this case because of the floating-point accuraccy problem.
        if (prev == -1)
        {
            i == 0 ? prev = buf1.size() - 1 : prev = i-1;
        }

        int next = i+1;
        if (i == buf1.size() - 1)
        {
            next = 0;
        }

        const T &iPoint = buf1.at(i);
        const T &prevPoint = buf1.at(prev);
        const T &nextPoint = buf1.at(next);

        if (not (IsEkvPointOnLine(iPoint, prevPoint, nextPoint) && prevPoint == nextPoint/*not zigzag*/)
                // If RemoveDublicates does not remove these points it is a valid case.
                // Case where last point equal first point
                || ((i == 0 || i == buf1.size() - 1) && (iPoint == prevPoint || iPoint == nextPoint)))
        {
            buf2.append(iPoint);
            prev = -1;
        }
    }

    buf2 = RemoveDublicates(buf2, false);

    return buf2;
}

//---------------------------------------------------------------------------------------------------------------------
template <class T>
QVector<T> VAbstractPiece::RemoveDublicates(const QVector<T> &points, bool removeFirstAndLast)
{
    QVector<T> p = points;

    if (removeFirstAndLast)
    {
        if (not p.isEmpty() && p.size() > 1)
        {
            // Path can't be closed
            // See issue #686
            if ((qAbs(p.first().x() - p.last().x()) < VGObject::accuracyPointOnLine)
                 && (qAbs(p.first().y() - p.last().y()) < VGObject::accuracyPointOnLine))
            {
                p.removeLast();
            }
        }
    }

    for (int i = 0; i < p.size()-1; ++i)
    {
        if ((qAbs(p.at(i).x() - p.at(i+1).x()) < VGObject::accuracyPointOnLine)
             && (qAbs(p.at(i).y() - p.at(i+1).y()) < VGObject::accuracyPointOnLine))
        {
            if (not removeFirstAndLast && (i == p.size()-1))
            {
                continue;
            }

            p.erase(p.begin() + i + 1);
            --i;
            continue;
        }
    }

    return p;
}

#endif // VABSTRACTPIECE_H
