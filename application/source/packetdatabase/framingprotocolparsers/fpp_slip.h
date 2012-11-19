/*  fpp_slip.h
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
#ifndef __FPP_SLIP_H__
#define __FPP_SLIP_H__

#include "packetdatabase/packet.h"
#include "packetdatabase/framingprotocolparser.h"



class fpp_Slip : public FramingProtocolParser
{

public:
    fpp_Slip(PacketFlags flags, size_t bufferLength);
    ~fpp_Slip();

    std::list<Packet*> parse(const unsigned char* data, size_t length);
    PacketFlags supportedFlags() const { return _flags; }

private:
    PacketFlags _flags;
    unsigned char* _buffer;
    size_t _bufferLength;
    size_t _index;
    bool _inEscape;

};

#endif
