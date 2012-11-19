/*  packetlistmodel.h
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
#ifndef __PACKETLISTMODEL_H__
#define __PACKETLISTMODEL_H__

#include <QAbstractTableModel>
#include "packetdatabase/packetdatabase.h"


//-----------------------------------------------------------------------------
//  Class Definition
//-----------------------------------------------------------------------------
class PacketListModel : public QAbstractTableModel, public PacketDatabaseListener
{
    Q_OBJECT


public:

    enum Columns
    {
        ColTimestamp,
        ColFrameLength,
        ColFlags,
        ColInfo,
        ColumnCount
    };

public:
    PacketListModel(PacketDatabase* packetDatabase, QObject* parent = 0);
    ~PacketListModel();

    QVariant data(const QModelIndex& index, int role) const;
    Qt::ItemFlags flags(const QModelIndex& index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;


protected:
    void beginInsertPackets(size_t current, size_t count);
    void endInsertPackets();
    void beginClearDatabase(size_t count);
    void endClearDatabase();


private:
    PacketDatabase* _packetDatabase;

};
#endif
