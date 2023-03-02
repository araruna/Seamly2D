/***************************************************************************
 **  @file   groups_widget.cpp
 **  @author Douglas S Caskey
 **  @date   Mar 1, 2023
 **
 **  @copyright
 **  Copyright (C) 2017 - 2023 Seamly, LLC
 **  https://github.com/fashionfreedom/seamly2d
 **
 **  @brief
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
 **  along with Seamly2D. If not, see <http://www.gnu.org/licenses/>.
 **************************************************************************/

 /************************************************************************
  **
  **  @file   vwidgetgroups.cpp
  **  @author Roman Telezhynskyi <dismine(at)gmail.com>
  **  @date   6 4, 2016
  **
  **  @brief
  **  @copyright
  **  This source code is part of the Valentina project, a pattern making
  **  program, whose allow create and modeling patterns of clothing.
  **  Copyright (C) 2016 Valentina project
  ** <https://bitbucket.org/dismine/valentina> All Rights Reserved.
  **
  **  Valentina is free software: you can redistribute it and/or modify
  **  it under the terms of the GNU General Public License as published by
  **  the Free Software Foundation, either version 3 of the License, or
  **  (at your option) any later version.
  **
  **  Valentina is distributed in the hope that it will be useful,
  **  but WITHOUT ANY WARRANTY; without even the implied warranty of
  **  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  **  GNU General Public License for more details.
  **
  **  You should have received a copy of the GNU General Public License
  **  along with Valentina.  If not, see <http://www.gnu.org/licenses/>.
  **
  *************************************************************************/

#include "groups_widget.h"
#include "ui_groups_widget.h"
#include "../core/vapplication.h"
#include "../vtools/tools/vabstracttool.h"
#include "../vtools/dialogs/tools/dialogtool.h"
#include "../vtools/dialogs/tools/editgroup_dialog.h"
#include "../vtools/undocommands/delgroup.h"
#include "../vtools/undocommands/addgroup.h"
#include "../vtools/undocommands/delete_groupitem.h"
#include "../vtools/tools/vabstracttool.h"
#include "../vtools/tools/drawTools/toolpoint/toolsinglepoint/toolcut/vtoolcutspline.h"
#include "../vtools/tools/drawTools/toolpoint/toolsinglepoint/toolcut/vtoolcutsplinepath.h"
#include "../vtools/tools/drawTools/toolpoint/toolsinglepoint/toolcut/vtoolcutarc.h"
#include "../vgeometry/vabstractarc.h"
#include "../vgeometry/varc.h"
#include "../vgeometry/vellipticalarc.h"
#include "../vgeometry/vcubicbezier.h"
#include "../vgeometry/vsplinepath.h"
#include "../vgeometry/vcubicbezierpath.h"
#include "../vgeometry/vpointf.h"
#include "../vpatterndb/vcontainer.h"
#include "../vwidgets/vabstractmainwindow.h"
#include "../vmisc/vabstractapplication.h"
#include "../vmisc/vsettings.h"
#include "../vmisc/vcommonsettings.h"
#include "../vmisc/logging.h"
#include "../ifc/ifcdef.h"
#include "../ifc/xml/vabstractpattern.h"

#include <Qt>
#include <QApplication>
#include <QMenu>
#include <QObject>
#include <QMetaObject>
#include <QPointer>
#include <QtDebug>
#include <QTableWidget>
#include <QListWidget>
#include <QListWidgetItem>
#include <QColor>
#include <QMessageBox>
#include <QIcon>
#include <QScopedPointer>


Q_LOGGING_CATEGORY(WidgetGroups, "vwidgetgroups")

//---------------------------------------------------------------------------------------------------------------------
GroupsWidget::GroupsWidget(VContainer *data, VAbstractPattern *doc, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::GroupsWidget)
    , m_doc(doc)
    , m_data(data)
    , m_patternHasGroups(false)
    , m_currentGroupId(0)

{
    ui->setupUi(this);

    fillTable(m_doc->getGroups());

    connect(m_doc, &VAbstractPattern::patternHasGroups, this, &GroupsWidget::draftBlockHasGroups);

    connect(ui->showAllGroups_ToolButton,   &QToolButton::clicked, this,  &GroupsWidget::showAllGroups);
    connect(ui->hideAllGroups_ToolButton,   &QToolButton::clicked, this,  &GroupsWidget::hideAllGroups);
    connect(ui->lockAllGroups_ToolButton,   &QToolButton::clicked, this,  &GroupsWidget::lockAllGroups);
    connect(ui->unlockAllGroups_ToolButton, &QToolButton::clicked, this,  &GroupsWidget::unlockAllGroups);
    connect(ui->addGroup_ToolButton,        &QToolButton::clicked, this,  &GroupsWidget::addGroupToList);
    connect(ui->deleteGroup_ToolButton,     &QToolButton::clicked, this,  &GroupsWidget::deleteGroupFromList);
    connect(ui->editGroup_ToolButton,       &QToolButton::clicked, this,  &GroupsWidget::editGroup);

    connect(ui->groups_TableWidget, &QTableWidget::cellClicked, this, &GroupsWidget::groupVisibilityChanged);
    connect(ui->groups_TableWidget, &QTableWidget::cellClicked, this, &GroupsWidget::groupLockChanged);
    connect(ui->groups_TableWidget, &QTableWidget::cellChanged, this, &GroupsWidget::renameGroup);
    connect(ui->groups_TableWidget, &QTableWidget::cellClicked, this, &GroupsWidget::fillGroupItemList);

    ui->groups_TableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->groups_TableWidget, &QTableWidget::customContextMenuRequested, this, &GroupsWidget::groupContextMenu);

    ui->groupItems_ListWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->groupItems_ListWidget, &QListWidget::customContextMenuRequested, this, &GroupsWidget::groupItemContextMenu);
}

//---------------------------------------------------------------------------------------------------------------------
GroupsWidget::~GroupsWidget()
{
    delete ui;
}

//---------------------------------------------------------------------------------------------------------------------
void GroupsWidget::groupVisibilityChanged(int row, int column)
{
    if (column != 0) return;

    QTableWidgetItem *item = ui->groups_TableWidget->item(row, column);
    const quint32 groupId = item->data(Qt::UserRole).toUInt();
    const bool locked = m_doc->getGroupLock(groupId);
    if (locked == false)
    {
        const bool visible = not m_doc->getGroupVisivility(groupId);
        m_doc->setGroupVisivility(groupId, visible);
        if (visible)
        {
            item->setIcon(QIcon("://icon/32x32/visible_on.png"));
        }
        else
        {
            item->setIcon(QIcon("://icon/32x32/visible_off.png"));
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
void GroupsWidget::groupLockChanged(int row, int column)
{
    if (column != 1)
    {
        return;
    }

    QTableWidgetItem *item = ui->groups_TableWidget->item(row, column);
    if (not item) return;
    const quint32 groupId = item->data(Qt::UserRole).toUInt();
    const bool locked = not m_doc->getGroupLock(groupId);
    m_doc->setGroupLock(groupId, locked);
    if (locked)
    {
        item->setIcon(QIcon("://icon/32x32/lock_on.png"));
    }
    else
    {
        item->setIcon(QIcon("://icon/32x32/lock_off.png"));
    }
}

//---------------------------------------------------------------------------------------------------------------------
void GroupsWidget::renameGroup(int row, int column)
{
    if (column != 2)
    {
        return;
    }

    const quint32 groupId = ui->groups_TableWidget->item(row, 0)->data(Qt::UserRole).toUInt();
    const bool locked = m_doc->getGroupLock(groupId);
    if (locked == false)
    {
        m_doc->setGroupName(groupId, ui->groups_TableWidget->item(row, column)->text());
        updateGroups();
    }
}


void GroupsWidget::showAllGroups()
{
     qCDebug(WidgetGroups, "Show All Groups");
     quint32 groupId;
     bool locked;
     for (int i = 0; i < ui->groups_TableWidget->rowCount(); ++i)
     {
         QTableWidgetItem *item = ui->groups_TableWidget->item(i, 0);
         if (not item)
         {
             return;
         }
         groupId = item->data(Qt::UserRole).toUInt();
         locked = m_doc->getGroupLock(groupId);
         if (item && locked == false)
         {
             m_doc->setGroupVisivility(groupId, true);
             item->setIcon(QIcon("://icon/32x32/visible_on.png"));
         }
     }
}

void GroupsWidget::hideAllGroups()
{
    qCDebug(WidgetGroups, "Hide All Groups");
    quint32 groupId;
    bool locked;
    for (int i = 0; i < ui->groups_TableWidget->rowCount(); ++i)
    {
        QTableWidgetItem *item = ui->groups_TableWidget->item(i, 0);
        if (not item)
        {
            return;
        }
        groupId = item->data(Qt::UserRole).toUInt();
        locked = m_doc->getGroupLock(groupId);
        if (item && locked == false)
        {
            m_doc->setGroupVisivility(groupId, false);
            item->setIcon(QIcon("://icon/32x32/visible_off.png"));
        }
    }
}

void GroupsWidget::lockAllGroups()
{
    qCDebug(WidgetGroups, "Lock All Groups");
    for (int i = 0; i < ui->groups_TableWidget->rowCount(); ++i)
    {
        QTableWidgetItem *item = ui->groups_TableWidget->item(i, 1);
        if (not item)
        {
            return;
        }
        const quint32 groupId = item->data(Qt::UserRole).toUInt();
        m_doc->setGroupLock(groupId, true);
        item->setIcon(QIcon("://icon/32x32/lock_on.png"));
    }
}
void GroupsWidget::unlockAllGroups()
{
    qCDebug(WidgetGroups, "Unlock All Groups");
    for (int i = 0; i < ui->groups_TableWidget->rowCount(); ++i)
    {
        QTableWidgetItem *item = ui->groups_TableWidget->item(i, 1);
        if (not item)
        {
            return;
        }
        const quint32 groupId = item->data(Qt::UserRole).toUInt();
        m_doc->setGroupLock(groupId, false);
        item->setIcon(QIcon("://icon/32x32/lock_off.png"));
    }
}

void GroupsWidget::addGroupToList()
{
    QScopedPointer<EditGroupDialog> dialog(new EditGroupDialog(new VContainer(qApp->TrVars(),
                                                                  qApp->patternUnitP()), NULL_ID, this));
    SCASSERT(dialog != nullptr)

    QString groupName;
    while (1)
    {
        const bool result = dialog->exec();
        groupName = dialog->getName();
        if (result == false || groupName.isEmpty())
        {
            return;
        }
        bool exists = m_doc->groupNameExists(groupName);
        if (exists == false)
        {
            break;
        }

        QMessageBox messageBox;
        messageBox.setWindowTitle(tr("Name Exists"));
        messageBox.setIcon(QMessageBox::Warning);
        messageBox.setStandardButtons(QMessageBox::Retry | QMessageBox::Cancel);
        messageBox.setDefaultButton(QMessageBox::Retry);
        messageBox.setText(tr("The action can't be completed because the group name already exists."));
        int boxResult = messageBox.exec();

        switch (boxResult)
        {
            case QMessageBox::Retry:
                break;    // Repeat Add Group Dialog
            case QMessageBox::Cancel:
                return;  // Exit Add Group Dialog
            default:
                break;   // should never be reached
        }
    }

    const quint32 nextId = VContainer::getNextId();
    qCDebug(WidgetGroups, "Group Name = %s", qUtf8Printable(groupName));
    qCDebug(WidgetGroups, "Next Id = %d", nextId);

    const QDomElement group = m_doc->createGroup(nextId, groupName, dialog->getColor(), dialog->getLineType(),
                                                  dialog->getLineWeight(), dialog->getGroupData());

    if (not group.isNull())
    {
        qCDebug(WidgetGroups, "Add a Group to List");
        AddGroup *command = new AddGroup(group, m_doc);
        connect(command, &AddGroup::updateGroups, this, &GroupsWidget::updateGroups);
        qApp->getUndoStack()->push(command);
    }
}

void GroupsWidget::deleteGroupFromList()
{
    const quint32 groupId = getGroupId();
    qCDebug(WidgetGroups, "Remove Group %d from List", groupId);
    const bool locked = m_doc->getGroupLock(groupId);
    QTableWidgetItem *item = ui->groups_TableWidget->currentItem();
    if (not item)
    {
        return;
    }

    if (item && (locked == false))
    {
        DelGroup *command = new DelGroup(m_doc, groupId);
        connect(command, &DelGroup::updateGroups, this, &GroupsWidget::updateGroups);
        qApp->getUndoStack()->push(command);
    }
}

void GroupsWidget::editGroup()
{
    ui->groups_TableWidget->blockSignals(true);
    qCDebug(WidgetGroups, "Edit Group List");
    const int row = ui->groups_TableWidget->currentRow();
    if (ui->groups_TableWidget->rowCount() == 0 || row == -1 || row >= ui->groups_TableWidget->rowCount())
    {
        ui->groups_TableWidget->blockSignals(false);
        return;
    }
    QTableWidgetItem *item = ui->groups_TableWidget->item(row,1);
    const quint32 groupId = ui->groups_TableWidget->item(row, 0)->data(Qt::UserRole).toUInt();
    const bool locked = m_doc->getGroupLock(groupId);
    QString oldGroupName = m_doc->getGroupName(groupId);
    if (locked == false)
    {
        qCDebug(WidgetGroups, "Row = %d", row);

        QScopedPointer<EditGroupDialog> dialog(new EditGroupDialog(new VContainer(qApp->TrVars(),
                                                                   qApp->patternUnitP()), NULL_ID, this));
        dialog->setName(oldGroupName);
        dialog->setColor(m_doc->getGroupColor(groupId));
        dialog->setLineType(m_doc->getGroupLineType(groupId));
        dialog->setLineWeight(m_doc->getGroupLineWeight(groupId));
        dialog->setWindowTitle(tr("Edit Group"));

        QString groupName;
        while (1)
        {
            const bool result = dialog->exec();
            groupName = dialog->getName();
            if (result == false || groupName.isEmpty())
            {
                ui->groups_TableWidget->blockSignals(false);
                return;
            }
            bool exists = m_doc->groupNameExists(groupName);
            if (exists == false || groupName == oldGroupName)
            {
                break;
            }

            QMessageBox messageBox;
            messageBox.setWindowTitle(tr("Name Exists"));
            messageBox.setIcon(QMessageBox::Warning);
            messageBox.setStandardButtons(QMessageBox::Retry | QMessageBox::Cancel);
            messageBox.setDefaultButton(QMessageBox::Retry);
            messageBox.setText(tr("The action can't be completed because the group name already exists."));
            int boxResult = messageBox.exec();

            switch (boxResult)
            {
                case QMessageBox::Retry:
                    break;    // Repeat Add Group Dialog
                case QMessageBox::Cancel:
                    return;  // Exit Add Group Dialog
                default:
                    break;   // should never be reached
            }
        }

        const QString groupColor = dialog->getColor();
        QPixmap pixmap = VAbstractTool::createColorIcon(32, 12, groupColor);

        const QString groupLineType = dialog->getLineType();
        const QString groupLineWeight = dialog->getLineWeight();

        item = ui->groups_TableWidget->item(row,3);
        item->setIcon(QIcon(pixmap));
        item->setText(groupName);
        m_doc->setGroupName(groupId, groupName);
        m_doc->setGroupColor(groupId, groupColor);
        m_doc->setGroupLineType(groupId, groupLineType);
        m_doc->setGroupLineWeight(groupId, groupLineWeight);

        updateGroups();
        ui->groups_TableWidget->blockSignals(false);
    }
}

//---------------------------------------------------------------------------------------------------------------------
void GroupsWidget::groupContextMenu(const QPoint &pos)
{
    QTableWidgetItem *item = ui->groups_TableWidget->itemAt(pos);
    if (not item)
    {
        return;
    }

    const int row = item->row();
    item = ui->groups_TableWidget->item(row, 0);
    const quint32 groupId = item->data(Qt::UserRole).toUInt();

    const bool locked = m_doc->getGroupLock(groupId);
    if (locked)
    {
        return;
    }

    QScopedPointer<QMenu> menu(new QMenu());
    QAction *actionRename = menu->addAction(QIcon("://icon/32x32/rename_32"), tr("Rename"));
    QAction *actionDelete = menu->addAction(QIcon::fromTheme("edit-delete"), tr("Delete"));
    QAction *selectedAction = menu->exec(ui->groups_TableWidget->viewport()->mapToGlobal(pos));
    if(selectedAction == nullptr)
    {
        return;
    }
    else if (selectedAction == actionRename)
    {
      QString oldGroupName = m_doc->getGroupName(groupId);
      QScopedPointer<EditGroupDialog> dialog(new EditGroupDialog(new VContainer(qApp->TrVars(),
                                                                    qApp->patternUnitP()), NULL_ID, this));
      dialog->setName(m_doc->getGroupName(groupId));
      dialog->setColor(m_doc->getGroupColor(groupId));
      dialog->setLineType(m_doc->getGroupLineType(groupId));
      dialog->setLineWeight(m_doc->getGroupLineWeight(groupId));
      dialog->setWindowTitle(tr("Edit Group"));

      QString groupName;
      while (1)
      {
          const bool result = dialog->exec();
          groupName = dialog->getName();
          if (result == false || groupName.isEmpty())
          {
              return;
          }
          bool exists = m_doc->groupNameExists(groupName);
          if (exists == false || groupName == oldGroupName)
          {
              break;
          }

          QMessageBox messageBox;
          messageBox.setWindowTitle(tr("Name Exists"));
          messageBox.setIcon(QMessageBox::Warning);
          messageBox.setStandardButtons(QMessageBox::Retry | QMessageBox::Cancel);
          messageBox.setDefaultButton(QMessageBox::Retry);
          messageBox.setText(tr("The action can't be completed because the group name already exists."));
          int boxResult = messageBox.exec();

          switch (boxResult)
          {
              case QMessageBox::Retry:
                  break;    // Repeat Add Group Dialog
              case QMessageBox::Cancel:
                  return;  // Exit Add Group Dialog
              default:
                  break;   // should never be reached
          }
      }

      const QString groupColor = dialog->getColor();
      const QString groupLineType = dialog->getLineType();
       const QString groupLineWeight = dialog->getLineWeight();
      QPixmap pixmap = VAbstractTool::createColorIcon(32, 12, groupColor);
      item = ui->groups_TableWidget->item(row, 3);
      item->setIcon(QIcon(pixmap));
      item->setText(groupName);
      m_doc->setGroupName(groupId, groupName);
      m_doc->setGroupColor(groupId, groupColor);
      m_doc->setGroupLineType(groupId, groupLineType);
      m_doc->setGroupLineWeight(groupId, groupLineWeight);
    }
    else if (selectedAction == actionDelete)
    {
        DelGroup *command = new DelGroup(m_doc, groupId);
        connect(command, &DelGroup::updateGroups, this, &GroupsWidget::updateGroups);
        qApp->getUndoStack()->push(command);
    }
}

//---------------------------------------------------------------------------------------------------------------------
void GroupsWidget::updateGroups()
{
    int row = ui->groups_TableWidget->currentRow();
    fillTable(m_doc->getGroups());
    if (ui->groups_TableWidget->rowCount() != 0 || row != -1 || row <= ui->groups_TableWidget->rowCount())
    {
        ui->groups_TableWidget->selectRow(row);
    }
    fillGroupItemList();
}

//---------------------------------------------------------------------------------------------------------------------
void GroupsWidget::fillTable(const QMap<quint32, GroupAttributes> &groups)
{
    ui->groups_TableWidget->blockSignals(true);
    ui->groups_TableWidget->clear();
    ui->groups_TableWidget->setColumnCount(4);
    ui->groups_TableWidget->setRowCount(groups.size());
    qint32 currentRow = -1;
    auto i = groups.constBegin();
    while (i != groups.constEnd())
    {
        ++currentRow;
        const GroupAttributes data = i.value();

        // Add visibility item
        QTableWidgetItem *item = new QTableWidgetItem();
        item->setTextAlignment(Qt::AlignHCenter);

        if (data.visible)
        {
            item->setIcon(QIcon("://icon/32x32/visible_on.png"));
        }
        else
        {
            item->setIcon(QIcon("://icon/32x32/visible_off.png"));
        }

        item->setData(Qt::UserRole, i.key());
        item->setFlags(item->flags() &= ~(Qt::ItemIsEditable));  // set the item non-editable (view only), and non-selectable
        ui->groups_TableWidget->setItem(currentRow, 0, item);

        // Add locked item
        item = new QTableWidgetItem();
        item->setTextAlignment(Qt::AlignHCenter);

        if (data.locked)
        {
            item->setIcon(QIcon("://icon/32x32/lock_on.png"));
        }
        else
        {
            item->setIcon(QIcon("://icon/32x32/lock_off.png"));
        }

        item->setData(Qt::UserRole, i.key());
        item->setFlags(item->flags() &= ~(Qt::ItemIsEditable));  // set the item non-editable (view only), and non-selectable
        ui->groups_TableWidget->setItem(currentRow, 1, item);

        // Add Edit Item
        if (!m_doc->isGroupEmpty(i.key()))
        {
            item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignHCenter);
            item->setIcon(QIcon("://icon/32x32/history.png"));
            item->setData(Qt::UserRole, i.key());
            item->setFlags(item->flags() &= ~(Qt::ItemIsEditable));  // set the item non-editable (view only), and non-selectable
            ui->groups_TableWidget->setItem(currentRow, 2, item);
        }

        // Add group name item
        QPixmap pixmap = VAbstractTool::createColorIcon(32,12,data.color);

        item = new QTableWidgetItem(data.name);
        item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        item->setIcon(QIcon(pixmap));
        //Qt::ItemFlags flags = item->flags();
        //flags &= ~(Qt::ItemIsEditable); // reset/clear the flag
        //item->setFlags(flags);
        ui->groups_TableWidget->setItem(currentRow, 3, item);

        ++i;
    }
    ui->groups_TableWidget->sortItems(1, Qt::AscendingOrder);
    ui->groups_TableWidget->resizeColumnsToContents();
    ui->groups_TableWidget->resizeRowsToContents();
    ui->groups_TableWidget->blockSignals(false);
}

void GroupsWidget::draftBlockHasGroups(bool value)
{
    m_patternHasGroups = value;

    ui->showAllGroups_ToolButton->setEnabled(value);
    ui->hideAllGroups_ToolButton->setEnabled(value);
    ui->lockAllGroups_ToolButton->setEnabled(value);
    ui->unlockAllGroups_ToolButton->setEnabled(value);
    ui->deleteGroup_ToolButton->setEnabled(value);
    ui->editGroup_ToolButton->setEnabled(value);
    qCDebug(WidgetGroups, "Draft Block Has Groups = %d", value);
}

void GroupsWidget::setAddGroupEnabled(bool value)
{
    ui->addGroup_ToolButton->setEnabled(value);
}

quint32 GroupsWidget::getGroupId()
{
    QTableWidgetItem *item = ui->groups_TableWidget->currentItem();
    if (not item)
    {
        return 0;
    }
    int row = ui->groups_TableWidget->row(item);
    qCDebug(WidgetGroups, "Row = %d\n", row);
    const quint32 groupId = ui->groups_TableWidget->item(row, 0)->data(Qt::UserRole).toUInt();
    return groupId;
}

QString GroupsWidget::getCurrentGroupName()
{
    QTableWidgetItem *item = ui->groups_TableWidget->currentItem();
    if (not item)
    {
        return QString();
    }
    int row = ui->groups_TableWidget->row(item);
    qCDebug(WidgetGroups, "Row = %d\n", row);
    const QString groupName = ui->groups_TableWidget->item(row, 3)->text();
    return groupName;
}

void GroupsWidget::fillGroupItemList()
{
    ui->groupItems_ListWidget->blockSignals(true);
    ui->groupItems_ListWidget->clear();
    QString groupName = getCurrentGroupName();
    QDomElement groupDomElement = m_doc->getGroupByName(groupName);
    if (groupDomElement.isNull())
    {
        return;
    }
    QMap<quint32, Tool> history = m_doc->getGroupObjHistory();
    QPair<bool, QMap<quint32, quint32> > group = m_doc->parseItemElement(groupDomElement);
    const QMap<quint32, quint32> groupData = group.second;
    if (!groupData.isEmpty())
    {
        auto i = groupData.constBegin();
        while (i != groupData.constEnd())
        {
           Tool tooltype = history.value(i.key());
           addGroupItem(i.key(), tooltype);
           ++i;
        }
    }
    ui->groupItems_ListWidget->sortItems(Qt::AscendingOrder);
    ui->groupItems_ListWidget->blockSignals(false);
}

//---------------------------------------------------------------------------------------------------------------------
QT_WARNING_PUSH
QT_WARNING_DISABLE_GCC("-Wswitch-default")
/**
 * @brief Add description for group item in listwidget
 * @param tool Tooltype
 */
void GroupsWidget::addGroupItem(const quint32 &toolId, const Tool &tooltype)
{
    // This check helps to find missing tools in the switch
    Q_STATIC_ASSERT_X(static_cast<int>(Tool::LAST_ONE_DO_NOT_USE) == 53, "Not all tools were used in history.");

    QString iconFileName = "";
    QString objName = "Unknown";
    const QDomElement domElem = m_doc->elementById(toolId);
    if (domElem.isElement() == false)
    {
        qDebug()<<"Can't find element by id"<<Q_FUNC_INFO;
        return;
    }
        try
        {
            switch ( tooltype )
            {
                case Tool::Arrow:
                case Tool::SinglePoint:
                case Tool::DoublePoint:
                case Tool::LinePoint:
                case Tool::AbstractSpline:
                case Tool::Cut:
                case Tool::Midpoint:// Same as Tool::AlongLine, but tool will never have such type
                case Tool::ArcIntersectAxis:// Same as Tool::CurveIntersectAxis, but tool will never have such type
                case Tool::LAST_ONE_DO_NOT_USE:
                    Q_UNREACHABLE(); //-V501
                    break;

                case Tool::BasePoint:
                    iconFileName = ":/toolicon/32x32/segment.png";
                    objName = tr("%1 - Base point").arg(getPointName(toolId));
                    break;

                case Tool::EndLine:
                    iconFileName = ":/toolicon/32x32/segment.png";
                    objName = tr("Point - Length and Angle of Line %1_%2")
                             .arg(getPointName(attrUInt(domElem, AttrBasePoint)))
                             .arg(getPointName(toolId));
                    break;

                case Tool::Line:
                    iconFileName = ":/toolicon/32x32/line.png";
                    objName = tr("Line %1_%2")
                             .arg(getPointName(attrUInt(domElem, AttrFirstPoint)))
                             .arg(getPointName(attrUInt(domElem, AttrSecondPoint)));
                    break;

                case Tool::AlongLine:
                    iconFileName = ":/toolicon/32x32/along_line.png";
                    objName = tr("%3 - Point - On Line %1_%2")
                             .arg(getPointName(attrUInt(domElem, AttrFirstPoint)))
                             .arg(getPointName(attrUInt(domElem, AttrSecondPoint)))
                             .arg(getPointName(toolId));
                    break;

                case Tool::ShoulderPoint:
                    iconFileName = ":/toolicon/32x32/shoulder.png";
                    objName = tr("%1 - Point - Length to Line").arg(getPointName(toolId));
                    break;

                case Tool::Normal:
                    iconFileName = ":/toolicon/32x32/normal.png";
                    objName = tr("%3 - Point - On Perpendicular to Line %1_%2")
                             .arg(getPointName(attrUInt(domElem, AttrFirstPoint)))
                             .arg(getPointName(attrUInt(domElem, AttrSecondPoint)))
                             .arg(getPointName(toolId));
                    break;

                case Tool::Bisector:
                    iconFileName = ":/toolicon/32x32/bisector.png";
                    objName = tr("%4 - Point - On Bisector of Angle %1_%2_%3")
                             .arg(getPointName(attrUInt(domElem, AttrFirstPoint)))
                             .arg(getPointName(attrUInt(domElem, AttrSecondPoint)))
                             .arg(getPointName(attrUInt(domElem, AttrThirdPoint)))
                             .arg(getPointName(toolId));
                    break;

                case Tool::LineIntersect:
                    iconFileName = ":/toolicon/32x32/intersect.png";
                    objName = tr("%5 - Point - Intersect Lines %1_%2 & %3_%4")
                             .arg(getPointName(attrUInt(domElem, AttrP1Line1)))
                             .arg(getPointName(attrUInt(domElem, AttrP2Line1)))
                             .arg(getPointName(attrUInt(domElem, AttrP1Line2)))
                             .arg(getPointName(attrUInt(domElem, AttrP2Line2)))
                             .arg(getPointName(toolId));
                    break;

               case Tool::Spline:
                {
                    const QSharedPointer<VSpline> spl = m_data->GeometricObject<VSpline>(toolId);
                    SCASSERT(not spl.isNull())
                    iconFileName = ":/toolicon/32x32/spline.png";
                    objName = spl->NameForHistory(tr("Curve - Interactive"));
                    break;

                }
                case Tool::CubicBezier:
                {
                    const QSharedPointer<VCubicBezier> spl = m_data->GeometricObject<VCubicBezier>(toolId);
                    SCASSERT(not spl.isNull())
                    iconFileName = ":/toolicon/32x32/cubic_bezier.png";
                    objName = spl->NameForHistory(tr("Curve - Fixed"));
                    break;

                }
                case Tool::Arc:
                {
                    const QSharedPointer<VArc> arc = m_data->GeometricObject<VArc>(toolId);
                    SCASSERT(not arc.isNull())
                    iconFileName = ":/toolicon/32x32/arc.png";
                    objName = tr("%1").arg(arc->NameForHistory(tr("Arc")));
                    break;

                }
                case Tool::ArcWithLength:
                {
                    const QSharedPointer<VArc> arc = m_data->GeometricObject<VArc>(toolId);
                    SCASSERT(not arc.isNull())
                    iconFileName = ":/toolicon/32x32/arc_with_length.png";
                    objName = tr("%1 - Arc - Length = %2 %3")
                                 .arg(arc->NameForHistory(tr("Arc")))
                                 .arg(qApp->fromPixel(arc->GetLength()))
                                 .arg(UnitsToStr(qApp->patternUnit()));
                    break;
                }
                case Tool::SplinePath:
                {
                    const QSharedPointer<VSplinePath> splPath = m_data->GeometricObject<VSplinePath>(toolId);
                    SCASSERT(not splPath.isNull())
                    iconFileName = ":/toolicon/32x32/splinePath.png";
                    objName = splPath->NameForHistory(tr("Spline - Interactive"));
                    break;

                }
                case Tool::CubicBezierPath:
                {
                   const QSharedPointer<VCubicBezierPath> splPath = m_data->GeometricObject<VCubicBezierPath>(toolId);
                    SCASSERT(not splPath.isNull())
                    iconFileName = ":/toolicon/32x32/cubic_bezier_path.png";
                    objName = splPath->NameForHistory(tr("Spline - Fixed"));
                    break;

                }
                case Tool::PointOfContact:
                    iconFileName = ":/toolicon/32x32/point_of_contact.png";
                    objName = tr("%4 - Point - Intersect Arc with Center Point %1 & Line %2_%3")
                             .arg(getPointName(attrUInt(domElem, AttrCenter)))
                             .arg(getPointName(attrUInt(domElem, AttrFirstPoint)))
                             .arg(getPointName(attrUInt(domElem, AttrSecondPoint)))
                             .arg(getPointName(toolId));
                    break;

                case Tool::Height:
                    iconFileName = ":/toolicon/32x32/height.png";
                    objName = tr("Point - Intersect Line & Perpendicular %1 to Line %2_%3")
                             .arg(getPointName(attrUInt(domElem, AttrBasePoint)))
                             .arg(getPointName(attrUInt(domElem, AttrP1Line)))
                             .arg(getPointName(attrUInt(domElem, AttrP2Line)));
                    break;

                case Tool::Triangle:
                    iconFileName = ":/toolicon/32x32/triangle.png";
                    objName = tr("Point - Intersect Axis Axis %1_%2 & Triangle Points %3 & %4")
                             .arg(getPointName(attrUInt(domElem, AttrAxisP1)))
                             .arg(getPointName(attrUInt(domElem, AttrAxisP2)))
                             .arg(getPointName(attrUInt(domElem, AttrFirstPoint)))
                             .arg(getPointName(attrUInt(domElem, AttrSecondPoint)));
                    break;

                case Tool::PointOfIntersection:
                    iconFileName = ":/toolicon/32x32/point_intersectxy_icon.png";
                    objName = tr("%1 - Point - Intersect XY from Points %2 & %3")
                             .arg(getPointName(toolId))
                             .arg(getPointName(attrUInt(domElem, AttrFirstPoint)))
                             .arg(getPointName(attrUInt(domElem, AttrSecondPoint)));
                    break;

                case Tool::CutArc:
                {
                    const QSharedPointer<VArc> arc = m_data->GeometricObject<VArc>(attrUInt(domElem, AttrArc));
                    SCASSERT(not arc.isNull())
                    iconFileName = ":/toolicon/32x32/arc_cut.png";
                    objName = tr("%1 - Point - On Arc %2")
                             .arg(getPointName(toolId))
                             .arg(arc->NameForHistory(tr("Arc")));
                    break;

                }
                case Tool::CutSpline:
                {
                    const quint32 splineId = attrUInt(domElem, VToolCutSpline::AttrSpline);
                    const QSharedPointer<VAbstractCubicBezier> spl = m_data->GeometricObject<VAbstractCubicBezier>(splineId);
                    SCASSERT(not spl.isNull())
                    iconFileName = ":/toolicon/32x32/spline_cut_point.png";
                    objName = tr("%1 - Point - On Curve %2")
                             .arg(getPointName(toolId))
                             .arg(spl->NameForHistory(tr("Curve")));
                    break;

                }
                case Tool::CutSplinePath:
                {
                    const quint32 splinePathId = attrUInt(domElem, VToolCutSplinePath::AttrSplinePath);
                    const QSharedPointer<VAbstractCubicBezierPath> splPath =
                    m_data->GeometricObject<VAbstractCubicBezierPath>(splinePathId);
                    SCASSERT(not splPath.isNull())
                    iconFileName = ":/toolicon/32x32/splinePath_cut_point.png";
                    objName = tr("%1 - Point - On Spline %2")
                             .arg(getPointName(toolId))
                             .arg(splPath->NameForHistory(tr("Curve Path")));
                    break;

                }
                case Tool::LineIntersectAxis:
                    iconFileName = ":/toolicon/32x32/line_intersect_axis.png";
                    objName = tr("%1 - Point - Intersect Line %2_%3 & Axis through Point %4")
                             .arg(getPointName(toolId))
                             .arg(getPointName(attrUInt(domElem, AttrP1Line)))
                             .arg(getPointName(attrUInt(domElem, AttrP2Line)))
                             .arg(getPointName(attrUInt(domElem, AttrBasePoint)));
                    break;

                case Tool::CurveIntersectAxis:
                    iconFileName = ":/toolicon/32x32/arc_intersect_axis.png";
                    objName = tr("%1 - Point - Intersect Curve & Axis")
                             .arg(getPointName(toolId));
                  break;

                case Tool::PointOfIntersectionArcs:
                    iconFileName = ":/toolicon/32x32/point_of_intersection_arcs.png";
                    objName = tr("%1 - Point - Intersect Arcs").arg(getPointName(toolId));
                    break;

                case Tool::PointOfIntersectionCircles:
                    iconFileName = ":/toolicon/32x32/point_of_intersection_circles.png";
                    objName = tr("%1 - Point - Intersect Circles").arg(getPointName(toolId));
                    break;

                case Tool::PointOfIntersectionCurves:
                    iconFileName = ":/toolicon/32x32/intersection_curves.png";
                    objName = tr("%1 - Point - Intersect Curves").arg(getPointName(toolId));
                    break;

                case Tool::PointFromCircleAndTangent:
                    iconFileName = ":/toolicon/32x32/point_from_circle_and_tangent.png";
                    objName = tr("%1 - Point - Intersect Circle & Tangent").arg(getPointName(toolId));
                    break;

                case Tool::PointFromArcAndTangent:
                    iconFileName = ":/toolicon/32x32/point_from_arc_and_tangent.png";
                    objName = tr("%1 - Point - Intersect Arc & Tangent ").arg(getPointName(toolId));
                    break;

                case Tool::TrueDarts:
                    iconFileName = ":/toolicon/32x32/true_darts.png";
                    objName = tr("True Dart %1_%2_%3")
                             .arg(getPointName(attrUInt(domElem, AttrDartP1)))
                             .arg(getPointName(attrUInt(domElem, AttrDartP2)))
                             .arg(getPointName(attrUInt(domElem, AttrDartP2)));
                    break;

                case Tool::EllipticalArc:
                {
                    const QSharedPointer<VEllipticalArc> elArc = m_data->GeometricObject<VEllipticalArc>(toolId);
                    SCASSERT(not elArc.isNull())
                    iconFileName = ":/toolicon/32x32/el_arc.png";
                    objName = tr("%1")
                             .arg(elArc->NameForHistory(tr("Arc - Elliptical")));
                    break;
                }
                //Because "history" not only show history of pattern, but help restore current data for each pattern's
                //piece, we need add record about details and nodes, but don't show them.
                case Tool::Piece:
                case Tool::Union:
                case Tool::NodeArc:
                case Tool::NodeElArc:
                case Tool::NodePoint:
                case Tool::NodeSpline:
                case Tool::NodeSplinePath:
                case Tool::Group:
                case Tool::Rotation:
                case Tool::MirrorByLine:
                case Tool::MirrorByAxis:
                case Tool::Move:
                case Tool::InternalPath:
                case Tool::AnchorPoint:
                case Tool::InsertNodes:
                    return;
            }
            qCDebug(WidgetGroups, "Icon Name = %s", qUtf8Printable(iconFileName));
            QListWidgetItem *item = new QListWidgetItem(objName);
            item->setIcon(QIcon(iconFileName));
            item->setData(Qt::UserRole, toolId);
            ui->groupItems_ListWidget->addItem(item);
            return;
        }

        catch (const VExceptionBadId &e)
        {
            qDebug()<<e.ErrorMessage()<<Q_FUNC_INFO;
            QListWidgetItem *item = new QListWidgetItem(tr("Unknown Object"));
            item->setIcon(QIcon(":/icons/win.icon.theme/16x16/status/dialog-warning.png"));
            item->setData(Qt::UserRole, toolId);
            ui->groupItems_ListWidget->addItem(item);
            return;
        }
}

QT_WARNING_POP

QString GroupsWidget::getPointName(quint32 toolId)
{
    return m_data->GeometricObject<VPointF>(toolId)->name();
}

quint32 GroupsWidget::attrUInt(const QDomElement &domElement, const QString &name)
{
    return m_doc->GetParametrUInt(domElement, name, "0");
}

QString GroupsWidget::getObjName(quint32 toolId)
{
    try
    {
        const auto obj = m_data->GetGObject(toolId);
        return obj->name();
    }
    catch (const VExceptionBadId &e)
    {
        qCDebug(WidgetGroups, "Error! Couldn't get object name by id = %s. %s %s", qUtf8Printable(QString().setNum(toolId)),
            qUtf8Printable(e.ErrorMessage()),
            qUtf8Printable(e.DetailedInformation()));
        return QString("Unknown");// Return Unknown string
    }
}

//---------------------------------------------------------------------------------------------------------------------
void GroupsWidget::groupItemContextMenu(const QPoint &pos)
{
    const quint32 groupId = getGroupId();
    const int row = ui->groupItems_ListWidget->currentRow();
    if (ui->groupItems_ListWidget->count() == 0 || row == -1 || row >= ui->groupItems_ListWidget->count())
    {
        return;
    }

    QListWidgetItem *rowItem = ui->groupItems_ListWidget->item(row);
    SCASSERT(rowItem != nullptr);
    const quint32 toolId = rowItem->data(Qt::UserRole).toUInt();

    QMenu menu;
    // Add Move Group Item menu
    QMap<quint32,QString> groupsNotContainingItem =  m_doc->getGroupsContainingItem(toolId, 0, false);
    QActionGroup *actionMoveGroupMenu= new QActionGroup(this);

    if(not groupsNotContainingItem.empty())
    {
        QMenu *menuMoveGroupItem = menu.addMenu(QIcon("://icon/32x32/list-move_32.png"), tr("Move to group"));
        QStringList list = QStringList(groupsNotContainingItem.values());
        list.sort(Qt::CaseInsensitive);

        for(int i=0; i<list.count(); ++i)
        {
            QAction *actionMoveGroupItem = menuMoveGroupItem->addAction(list[i]);
            actionMoveGroupMenu->addAction(actionMoveGroupItem);
            const quint32 groupId = groupsNotContainingItem.key(list[i]);
            actionMoveGroupItem->setData(groupId);
            //groupsNotContainingItem.remove(groupId);   // delete any duplicate groups
        }
    }
    // Add Remove Group Item menu
    QAction *actionRemove = menu.addAction(QIcon::fromTheme("edit-delete"), tr("Remove"));

    QAction *selectedAction = menu.exec(ui->groupItems_ListWidget->viewport()->mapToGlobal(pos));
    if(selectedAction == nullptr)
    {
        return;
    }
    else if (selectedAction == actionRemove)
    {
        const bool locked = m_doc->getGroupLock(groupId);
        if (locked == false)
        {
            qCDebug(WidgetGroups, "Remove Tool %s from Group %s.",
                    qUtf8Printable(QString().setNum(toolId)),
                    qUtf8Printable(QString().setNum(groupId)));

            delete ui->groupItems_ListWidget->item(row);

            QDomElement item = m_doc->deleteGroupItem(toolId, 0, groupId);

            VMainGraphicsScene *scene = qobject_cast<VMainGraphicsScene *>(qApp->getCurrentScene());
            SCASSERT(scene != nullptr)
            scene->clearSelection();

            VAbstractMainWindow *window = qobject_cast<VAbstractMainWindow *>(qApp->getMainWindow());
            SCASSERT(window != nullptr)
            {
                DeleteGroupItemUndoCmd *command = new DeleteGroupItemUndoCmd(item, m_doc, groupId);
                connect(command, &DeleteGroupItemUndoCmd::updateGroups, window, &VAbstractMainWindow::updateGroups);
                qApp->getUndoStack()->push(command);
            }
        }
    }
    else if (selectedAction->actionGroup() == actionMoveGroupMenu)
    {
      const quint32 newGroupId = selectedAction->data().toUInt();
      const bool locked = m_doc->getGroupLock(groupId);
      const bool newLocked = m_doc->getGroupLock(newGroupId);
      if ((locked == false) && (newLocked == false))      //only move if both groups are unlocked
      {

          qCDebug(WidgetGroups, "Move Tool %s from Group %s to Group %s.",
                  qUtf8Printable(QString().setNum(toolId)),
                  qUtf8Printable(QString().setNum(groupId)),
                  qUtf8Printable(QString().setNum(newGroupId)));

          delete ui->groupItems_ListWidget->item(row);

          QDomElement item = m_doc->deleteGroupItem(toolId, 0, groupId);
          QDomElement newitem = m_doc->addGroupItem(toolId, 0, newGroupId);
      }
    }
}
