/*  packetfieldinfo.cpp
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
#include "packetfieldinfo.h"
#include <stdio.h>
#include <stdarg.h>


//-----------------------------------------------------------------------------
PacketFieldInfo::PacketFieldInfo(PacketLayerInfo* parent, const std::string& info) : 
    _parent(parent),
    _info(info)
{

}


//-----------------------------------------------------------------------------
PacketFieldInfo::PacketFieldInfo(PacketLayerInfo* parent, const char* fmt, ...) :
    _parent(parent)
{
    const size_t formatBufferSize = 512;
    char formatBuffer[formatBufferSize];
    va_list args;
    va_start(args, fmt);
    vsnprintf(formatBuffer, sizeof(formatBuffer) - 1, fmt, args);
    _info = formatBuffer;
}


//-----------------------------------------------------------------------------
PacketFieldInfo::PacketFieldInfo(PacketLayerInfo* parent, size_t offset, size_t length, const std::string& info) :
    PacketInfo(offset, length),
    _parent(parent),
    _info(info)
{

}


//-----------------------------------------------------------------------------
PacketFieldInfo::PacketFieldInfo(PacketLayerInfo* parent, size_t offset, size_t length, const char* fmt, ...) :
    PacketInfo(offset, length),
    _parent(parent)
{
    const size_t formatBufferSize = 512;
    char formatBuffer[formatBufferSize];
    va_list args;
    va_start(args, fmt);
    vsnprintf(formatBuffer, sizeof(formatBuffer) - 1, fmt, args);
    _info = formatBuffer;

}


//-----------------------------------------------------------------------------
PacketFieldInfo::~PacketFieldInfo()
{

}
