/*  slipframe.cpp
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
#include "slipframe.h"


//-----------------------------------------------------------------------------
SlipFrame::SlipFrame()
{

}


//-----------------------------------------------------------------------------
SlipFrame::SlipFrame(const SlipFrame& obj) :
    _timestamp(obj._timestamp),
    _frameLength(obj._frameLength),
    _payload(obj._payload)
{

}


//-----------------------------------------------------------------------------
SlipFrame::SlipFrame(const void* payload, size_t length)
{
    _timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    _frameLength = length;

    unsigned char* ptr = (unsigned char*)payload;
    _payload.reserve(length * 3);
    for (size_t i=0; i < length; ++i) {
        _payload += QString("%1").arg(ptr[i], 2, 16, QChar('0')).toUpper();
        _payload += " ";
    }

}


//-----------------------------------------------------------------------------
SlipFrame::~SlipFrame()
{

}
