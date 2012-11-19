/*  packet.cpp
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
#include "packet.h"
#include <string.h>


//-----------------------------------------------------------------------------
Packet::Packet(const unsigned char* data, size_t length, PacketFlags flags) :
    _parsed(false),
    _length(length),
    _flags(flags),
    _timestamp(0)
{
    _data = new unsigned char[length];
    memcpy(_data, data, length);
}


//-----------------------------------------------------------------------------
Packet::~Packet()
{
    delete [] _data;
}


//-----------------------------------------------------------------------------
bool Packet::parsed() const
{
    return _parsed;
}


//-----------------------------------------------------------------------------
void Packet::setParsed()
{    
    _parsed = true;
}


//-----------------------------------------------------------------------------
void Packet::addLayerInfo(PacketLayerInfo* info)
{
    _layers.push_back(info);
}


//-----------------------------------------------------------------------------
int Packet::layerInfoCount() const
{
    return _layers.size();
}


//-----------------------------------------------------------------------------
PacketLayerInfo* Packet::layerInfoAt(int index) const
{
    return _layers[index];
}
