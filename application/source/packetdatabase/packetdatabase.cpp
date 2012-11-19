/*  packetdatabase.cpp
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
#include "packetdatabase.h"


//-----------------------------------------------------------------------------
PacketDatabase::PacketDatabase()
{

}


//-----------------------------------------------------------------------------
PacketDatabase::~PacketDatabase()
{

}


//-----------------------------------------------------------------------------
void PacketDatabase::clear()
{
    std::list<PacketDatabaseListener*>::const_iterator l;
    for (l = _listeners.begin(); l != _listeners.end(); ++l) {
        PacketDatabaseListener* listener(*l);
        listener->beginClearDatabase(_packets.size());
    }    

    // Delete all the packets
    std::vector<Packet*>::const_iterator i;
    for (i = _packets.begin(); i != _packets.end(); ++i) {
        Packet* packet(*i);
        delete packet;
    }

    _packets.clear();

    for (l = _listeners.begin(); l != _listeners.end(); ++l) {
        PacketDatabaseListener* listener(*l);
        listener->endClearDatabase();
    }    
}


//-----------------------------------------------------------------------------
Packet* PacketDatabase::packetAt(size_t i) const
{
    return _packets[i];
}


//-----------------------------------------------------------------------------
size_t PacketDatabase::packetCount() const
{
    return _packets.size();
}


//-----------------------------------------------------------------------------
void PacketDatabase::addListener(PacketDatabaseListener* listener)
{
    _listeners.push_back(listener);
}


//-----------------------------------------------------------------------------
void PacketDatabase::addPacket(Packet* packet)
{
    std::list<PacketDatabaseListener*>::const_iterator i;
    for (i = _listeners.begin(); i != _listeners.end(); ++i) {
        PacketDatabaseListener* listener(*i);
        listener->beginInsertPackets(_packets.size(), 1);
    }

    _packets.push_back(packet);

    for (i = _listeners.begin(); i != _listeners.end(); ++i) {
        PacketDatabaseListener* listener(*i);
        listener->endInsertPackets();
    }
}
