/*  protocolstackmodel.cpp
 *
 *  Copyright 2012 Willem Kemp.
 *  All rights reserved.
 *
 *  This file is part of PortScope.
 *
 *  PortScope is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  PortScope is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with PortScope. If not, see http://www.gnu.org/licenses/.
 *
 */
#include "protocolstackmodel.h"


//-----------------------------------------------------------------------------
ProtocolStackModel::ProtocolStackModel(ProtocolStackItem* root, QObject *parent) :
    QAbstractItemModel(parent),
    _rootItem(root)
{

}


//-----------------------------------------------------------------------------
ProtocolStackModel::~ProtocolStackModel()
{

}


//-----------------------------------------------------------------------------
ProtocolStackItem* ProtocolStackModel::itemAt(const QModelIndex& index) const
{
    ProtocolStackItem* item;

    if (index.isValid()) {
        item = static_cast<ProtocolStackItem*>(index.internalPointer());
    }

    return item;
}


//-----------------------------------------------------------------------------
int ProtocolStackModel::columnCount(const QModelIndex& parent) const
{
    return ColumnCount;
}


//-----------------------------------------------------------------------------
QVariant ProtocolStackModel::data(const QModelIndex& index, int role) const
{
    QVariant result;

    if (index.isValid()) {
        ProtocolStackItem *item = static_cast<ProtocolStackItem*>(index.internalPointer());
        if (role == Qt::DisplayRole) {

            switch (index.column()) {
                case ColName: {
                    result = item->name();
                } break;
                case ColType: {
                    result = item->type();
                } break;
            }

        }
    }

    return result;
}


//-----------------------------------------------------------------------------
Qt::ItemFlags ProtocolStackModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags flags = 0;

    if (index.isValid()) {
        ProtocolStackItem *item = static_cast<ProtocolStackItem*>(index.internalPointer());
        flags = (Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    }

    return flags;
}


//-----------------------------------------------------------------------------
QVariant ProtocolStackModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant result;

    if ((orientation == Qt::Horizontal) && (role == Qt::DisplayRole)) {
        switch (section) {
        case ColName:
            result = "Name";
            break;
        case ColType:
            result = "Type";
            break;
        }
    }

    return result;
}


//-----------------------------------------------------------------------------
QModelIndex ProtocolStackModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }

    ProtocolStackItem* parentItem;

    if (!parent.isValid()) {
        parentItem = _rootItem;
    } else {
        parentItem = static_cast<ProtocolStackItem*>(parent.internalPointer());
    }

    ProtocolStackItem *childItem = parentItem->child(row);
    if (childItem) {
        return createIndex(row, column, childItem);
    } else {
        return QModelIndex();
    }
}


//-----------------------------------------------------------------------------
QModelIndex ProtocolStackModel::parent(const QModelIndex& index) const
{
    if (!index.isValid()) {
        return QModelIndex();
    }

    ProtocolStackItem* childItem = static_cast<ProtocolStackItem*>(index.internalPointer());
    ProtocolStackItem* parentItem = childItem->parent();

    if (parentItem == _rootItem) {
        return QModelIndex();
    }

    return createIndex(parentItem->row(), 0, parentItem);
}


//-----------------------------------------------------------------------------
int ProtocolStackModel::rowCount(const QModelIndex& parent) const
{
    ProtocolStackItem* parentItem;
    if (parent.column() > 0) {
        return 0;
    }

    if (!parent.isValid()) {
        parentItem = _rootItem;
    } else {
        parentItem = static_cast<ProtocolStackItem*>(parent.internalPointer());
    }

    return parentItem->childCount();
}


//-----------------------------------------------------------------------------
void ProtocolStackModel::externalEditOccured()
{
    emit dataChanged(QModelIndex(), QModelIndex());
}

