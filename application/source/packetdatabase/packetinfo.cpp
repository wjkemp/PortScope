/*  packetinfo.cpp
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
#include "packetinfo.h"


//-----------------------------------------------------------------------------
PacketInfo::PacketInfo() :
    _offset(0),
    _length(0)
{
}

    
//-----------------------------------------------------------------------------
PacketInfo::PacketInfo(size_t offset, size_t length) :
    _offset(offset),
    _length(length)
{

}

//-----------------------------------------------------------------------------
PacketInfo::~PacketInfo()
{

}
