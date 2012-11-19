/*  packetparser.cpp
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
#include "packetparser.h"


//-----------------------------------------------------------------------------
PacketParser::PacketParser(std::list<PacketLayerParser*> packetLayerParsers) :
    _packetLayerParsers(packetLayerParsers)
{

}


//-----------------------------------------------------------------------------
PacketParser::~PacketParser()
{

}


//-----------------------------------------------------------------------------
std::string PacketParser::parseInfo(Packet* packet)
{
    std::string result;

    // Run through the layer parsers
    size_t layerOffset = 0;
    size_t layerLength = packet->length();
    bool first = true;

    std::list<PacketLayerParser*>::const_iterator i;
    for (i = _packetLayerParsers.begin(); i != _packetLayerParsers.end(); ++i) {
        PacketLayerParser* packetLayerParser(*i);
        std::string info = packetLayerParser->parseInfo(packet->data(), packet->length(), layerOffset, layerLength);

        if (info.length() > 0) {
            if (!first) {
                result += ", ";
            } else {
                first = false;
            }

            result += info;
        }
    }

    return result;
}


//-----------------------------------------------------------------------------
void PacketParser::parseDetail(Packet* packet)
{
    // Run through the layer parsers
    size_t layerOffset = 0;
    size_t layerLength = packet->length();

    std::list<PacketLayerParser*>::const_iterator i;
    for (i = _packetLayerParsers.begin(); i != _packetLayerParsers.end(); ++i) {
        PacketLayerParser* packetLayerParser(*i);
        PacketLayerInfo* packetLayerInfo = packetLayerParser->parseDetail(packet->data(), packet->length(), layerOffset, layerLength);
        if (packetLayerInfo) {
            packet->addLayerInfo(packetLayerInfo);
        }
    }

    // Set the parsed flag
    packet->setParsed();
}

