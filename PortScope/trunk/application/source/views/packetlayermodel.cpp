/*  packetlayermodel.cpp
 *
 *  Copyright (C) 2012 Willem Kemp <http://www.thenocturnaltree.com>
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
#include "packetlayermodel.h"


//-----------------------------------------------------------------------------
PacketLayerModel::PacketLayerModel(Packet* packet, QObject *parent) :
    QAbstractItemModel(parent),
    _packet(packet)
{

}


//-----------------------------------------------------------------------------
PacketLayerModel::~PacketLayerModel()
{

}


//-----------------------------------------------------------------------------
PacketInfo* PacketLayerModel::getInfoAt(const QModelIndex& index) const
{
    PacketInfo* info = static_cast<PacketInfo*>(index.internalPointer());
    return info;
}


//-----------------------------------------------------------------------------
int PacketLayerModel::columnCount(const QModelIndex& parent) const
{
    return ColumnCount;
}


//-----------------------------------------------------------------------------
QVariant PacketLayerModel::data(const QModelIndex& index, int role) const
{
    QVariant result;

    if (index.isValid()) {

        if (role == Qt::DisplayRole) {

            switch (index.column()) {
                case ColInfo: {
                    PacketInfo* info = static_cast<PacketInfo*>(index.internalPointer());
                    PacketLayerInfo* layerInfo = dynamic_cast<PacketLayerInfo*>(info);
                    if (layerInfo) {
                        result = layerInfo->info().c_str();
                    } else {
                        PacketFieldInfo* fieldInfo = dynamic_cast<PacketFieldInfo*>(info);
                        if (fieldInfo) {
                            result = fieldInfo->info().c_str();
                        }
                    }
                } break;
            }
        }
    }

    return result;
}


//-----------------------------------------------------------------------------
Qt::ItemFlags PacketLayerModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags flags = 0;

    if (index.isValid()) {
        flags = (Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    }

    return flags;
}


//-----------------------------------------------------------------------------
QVariant PacketLayerModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant result;

    if ((orientation == Qt::Horizontal) && (role == Qt::DisplayRole)) {
        switch (section) {
            case ColInfo:
            result = "Information";
            break;
        }
    }

    return result;
}


//-----------------------------------------------------------------------------
QModelIndex PacketLayerModel::index(int row, int column, const QModelIndex& parent) const
{
    QModelIndex result;

    if (hasIndex(row, column, parent)) {

        // If the parent index is invalid, this is a top-level item
        if (!parent.isValid()) {

            // Check the validity of the row
            if (row < _packet->layerInfoCount()) {

                // Get a pointer to the layer info
                PacketLayerInfo* info = _packet->layerInfoAt(row);

                // Create the index
                result = createIndex(row, column, info);
            }

        // The parent is valid, the parent can be either layer info or field info
        } else {

            PacketInfo* info = static_cast<PacketInfo*>(parent.internalPointer());
            PacketLayerInfo* layerInfo = dynamic_cast<PacketLayerInfo*>(info);
            if (layerInfo) {
                if (row < layerInfo->fieldCount()) {
                    return createIndex(row, column, layerInfo->fieldAt(row));
                }
            }
        }
    }

    return result;
}


//-----------------------------------------------------------------------------
QModelIndex PacketLayerModel::parent(const QModelIndex& index) const
{
    QModelIndex result;

    if (index.isValid()) {
        PacketInfo* info = static_cast<PacketInfo*>(index.internalPointer());
        PacketFieldInfo* fieldInfo = dynamic_cast<PacketFieldInfo*>(info);
        if (fieldInfo) {
            PacketLayerInfo* parent = fieldInfo->parent();
            result = createIndex(parent->layerIndex(), 0, parent);
        }
    }

    return result;
}


//-----------------------------------------------------------------------------
int PacketLayerModel::rowCount(const QModelIndex& parent) const
{
    int result = 0;

    if (!parent.isValid()) {
        result = _packet->layerInfoCount();
    } else {
        PacketInfo* info = static_cast<PacketInfo*>(parent.internalPointer());
        PacketLayerInfo* layerInfo = dynamic_cast<PacketLayerInfo*>(info);
        if (layerInfo) {
            result = layerInfo->fieldCount();
        }
    }

    return result;
}

