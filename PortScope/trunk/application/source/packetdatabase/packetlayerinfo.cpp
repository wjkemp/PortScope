/*  packetlayerinfo.cpp
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
#include "packetlayerinfo.h"


//-----------------------------------------------------------------------------
PacketLayerInfo::PacketLayerInfo(const std::string& info, int layerIndex) :
    _info(info),
    _layerIndex(layerIndex)
{

}


//-----------------------------------------------------------------------------
PacketLayerInfo::PacketLayerInfo(size_t offset, size_t length, const std::string& info, int layerIndex) :
    PacketInfo(offset, length),
    _info(info),
    _layerIndex(layerIndex)
{

}


//-----------------------------------------------------------------------------
PacketLayerInfo::~PacketLayerInfo()
{

}


//-----------------------------------------------------------------------------
void PacketLayerInfo::addFieldInfo(PacketFieldInfo* fieldInfo)
{
    _fields.push_back(fieldInfo);
}


//-----------------------------------------------------------------------------
std::string PacketLayerInfo::info() const
{
    return _info;
}


//-----------------------------------------------------------------------------
int PacketLayerInfo::layerIndex() const
{
    return _layerIndex;
}


//-----------------------------------------------------------------------------
int PacketLayerInfo::fieldCount() const
{
    return _fields.size();
}


//-----------------------------------------------------------------------------
PacketFieldInfo* PacketLayerInfo::fieldAt(int index) const
{
    return _fields[index];
}
