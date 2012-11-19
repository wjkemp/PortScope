/*  packetlayerview.h
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
#ifndef __PACKETLAYERVIEW_H__
#define __PACKETLAYERVIEW_H__

#include <QTreeView>


//-----------------------------------------------------------------------------
//  Class Definition
//-----------------------------------------------------------------------------
class PacketLayerView : public QTreeView
{
    Q_OBJECT

public:
    PacketLayerView(QWidget* parent = 0);
    ~PacketLayerView();
    void setModel(QAbstractItemModel* newModel);

};
#endif
