/*  displaymodel.cpp
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
#include "displaymodel.h"


//-----------------------------------------------------------------------------
DisplayModel::DisplayModel(QObject *parent) :
    QAbstractTableModel(parent)
{

}


//-----------------------------------------------------------------------------
DisplayModel::~DisplayModel()
{

}


//-----------------------------------------------------------------------------
void DisplayModel::addItem(const SlipFrame& frame)
{
    beginInsertRows(QModelIndex(), _items.size(), _items.size());
    _items.append(frame);
    endInsertRows();
}



//-----------------------------------------------------------------------------
int DisplayModel::rowCount(const QModelIndex& parent) const
{
    return _items.size();
}


//-----------------------------------------------------------------------------
int DisplayModel::columnCount(const QModelIndex& parent) const
{
    return ColumnCount;
}


//-----------------------------------------------------------------------------
QVariant DisplayModel::data(const QModelIndex& index, int role) const
{
    QVariant result;

    if (index.isValid()) {

        switch (index.column()) {

            // ColTimestamp
            case ColTimestamp: {

                switch (role) {
                    case Qt::DisplayRole: {
                        result = _items[index.row()].timestamp();
                    } break;
                }
            } break;

            // ColFrameLength
            case ColFrameLength: {

                switch (role) {
                    case Qt::DisplayRole: {
                        result = QString::number(_items[index.row()].frameLength());
                    } break;
                }
            } break;

            // ColPayload
            case ColPayload: {

                switch (role) {
                    case Qt::DisplayRole: {
                        result = _items[index.row()].payload();
                    } break;
                }
            } break;

        }
    }



    return result;
}


//-----------------------------------------------------------------------------
Qt::ItemFlags DisplayModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags flags = 0;

    if (index.isValid()) {

        switch (index.column()) {

            // Timestamp
            case ColTimestamp: {
                flags = (Qt::ItemIsEnabled);
            } break;

            // ColFrameLength
            case ColFrameLength: {
                flags = (Qt::ItemIsEnabled);
            } break;

            // Payload
            case ColPayload: {
                flags = (Qt::ItemIsEnabled);
            } break;
        }
    }

    return flags;
}


//-----------------------------------------------------------------------------
QVariant DisplayModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant result;

    if ((role == Qt::DisplayRole) && (orientation == Qt::Horizontal)) {
        switch (section) {

        case ColTimestamp:
            result = "Timestamp";
            break;
        case ColFrameLength:
            result = "Length";
            break;
        case ColPayload:
            result = "Payload";
            break;
        }
    }
    return result;
}
