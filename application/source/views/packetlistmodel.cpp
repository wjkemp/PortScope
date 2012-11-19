/*  packetlistmodel.cpp
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
#include "packetlistmodel.h"
#include <QDateTime>
#include <QColor>


//-----------------------------------------------------------------------------
PacketListModel::PacketListModel(PacketDatabase* packetDatabase, QObject *parent) :
    QAbstractTableModel(parent),
    _packetDatabase(packetDatabase)
{
    _packetDatabase->addListener(this);
}


//-----------------------------------------------------------------------------
PacketListModel::~PacketListModel()
{

}


//-----------------------------------------------------------------------------
void PacketListModel::beginInsertPackets(size_t current, size_t count)
{
    beginInsertRows(QModelIndex(), current, current + count - 1);
}


//-----------------------------------------------------------------------------
void PacketListModel::endInsertPackets()
{
    endInsertRows();
}

//-----------------------------------------------------------------------------
void PacketListModel::beginClearDatabase(size_t count)
{
    beginRemoveRows(QModelIndex(), 0, count - 1);
}


//-----------------------------------------------------------------------------
void PacketListModel::endClearDatabase()
{
    endRemoveRows();
}


//-----------------------------------------------------------------------------
int PacketListModel::rowCount(const QModelIndex& parent) const
{
    return _packetDatabase->packetCount();
}


//-----------------------------------------------------------------------------
int PacketListModel::columnCount(const QModelIndex& parent) const
{
    return ColumnCount;
}


//-----------------------------------------------------------------------------
QVariant PacketListModel::data(const QModelIndex& index, int role) const
{
    QVariant result;

    if (index.isValid()) {

        switch (role) {
            case Qt::DisplayRole: {

                switch (index.column()) {

                    // ColTimestamp
                    case ColTimestamp: {
                        Packet* packet = _packetDatabase->packetAt(index.row());
                        QDateTime time(QDateTime::fromMSecsSinceEpoch(packet->timestamp()));
                        result = time.toString("hh:mm:ss");
                    } break;

                    // ColFrameLength
                    case ColFrameLength: {
                        Packet* packet = _packetDatabase->packetAt(index.row());
                        result = QString::number(packet->length());
                    } break;

                    // ColFlags
                    case ColFlags: {
                        Packet* packet = _packetDatabase->packetAt(index.row());
                        if (packet->flags() & PF_TX) {
                            result = "PF_TX";
                        } else if (packet->flags() & PF_RX) {
                            result = "PF_RX";
                        }
                    } break;

                    // ColInfo
                    case ColInfo: {
                        Packet* packet = _packetDatabase->packetAt(index.row());
                        result = packet->info().c_str();
                    } break;

                } break;
            } break;

            case Qt::BackgroundColorRole: {
                Packet* packet = _packetDatabase->packetAt(index.row());
                if (packet->flags() & PF_TX) {
                    result = Qt::white;
                } else if (packet->flags() & PF_RX) {
                    result = Qt::lightGray;
                }

            } break;
        }
    }



    return result;
}


//-----------------------------------------------------------------------------
Qt::ItemFlags PacketListModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags flags = 0;

    if (index.isValid()) {

        switch (index.column()) {

            // Timestamp
            case ColTimestamp: {
                flags = (Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            } break;

            // ColFrameLength
            case ColFrameLength: {
                flags = (Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            } break;

            // ColFlags
            case ColFlags: {
                flags = (Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            } break;

            // ColInfo
            case ColInfo: {
                flags = (Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            } break;
        }
    }

    return flags;
}


//-----------------------------------------------------------------------------
QVariant PacketListModel::headerData(int section, Qt::Orientation orientation, int role) const
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
        case ColFlags:
            result = "Flags";
            break;
        case ColInfo:
            result = "Info";
            break;
        }
    }
    return result;
}
