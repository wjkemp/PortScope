/*  packetlayerparserfactory.cpp
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
#include "packetlayerparserfactory.h"
#include "packetlayerparsers/plp_idr.h"
#include "packetlayerparsers/plp_esploader.h"


//-----------------------------------------------------------------------------
PacketLayerParserFactory::PacketLayerParserFactory()
{

}


//-----------------------------------------------------------------------------
PacketLayerParserFactory::~PacketLayerParserFactory()
{

}


//-----------------------------------------------------------------------------
PacketLayerParser* PacketLayerParserFactory::createParser(const std::string& name, int layerIndex)
{
    if (name == "plp_idr") {
        return new plp_IDR(layerIndex);
    } else if (name == "plp_esploader") {
        return new plp_EspLoader(layerIndex);
    } else {
        return 0;
    }
}
