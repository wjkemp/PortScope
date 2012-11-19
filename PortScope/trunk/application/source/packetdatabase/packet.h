/*  packet.h
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
#ifndef __PACKET_H__
#define __PACKET_H__

#include "packetlayerinfo.h"
#include "packetflags.h"


class Packet
{
public:
    Packet(const unsigned char* data, size_t length, PacketFlags flags);
    ~Packet();

    const unsigned char* data() const { return _data; }
    size_t length() const { return _length; }
    PacketFlags flags() const { return _flags; }

    void setInfo(const std::string& info) { _info = info; }
    std::string info() const { return _info; }

    void setTimestamp(unsigned long long timestamp) { _timestamp = timestamp; }
    unsigned long long timestamp() const { return _timestamp; }

    bool parsed() const;
    void setParsed();

    void addLayerInfo(PacketLayerInfo* info);
    int layerInfoCount() const;
    PacketLayerInfo* layerInfoAt(int index) const;

private:
    bool _parsed;
    unsigned char* _data;
    size_t _length;
    PacketFlags _flags;
    std::string _info;
    unsigned long long _timestamp;
    std::vector<PacketLayerInfo*> _layers;
};


#endif
