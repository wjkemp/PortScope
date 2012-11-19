/*  parserconfigurationloader.cpp
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
#include "parserconfigurationloader.h"
#include "framingprotocolparserfactory.h"
#include "packetlayerparserfactory.h"
#include <QFile>


//-----------------------------------------------------------------------------
ParserConfigurationLoader::ParserConfigurationLoader()
{
}


//-----------------------------------------------------------------------------
ParserConfigurationLoader::~ParserConfigurationLoader()
{
}


//-----------------------------------------------------------------------------
ParserConfiguration* ParserConfigurationLoader::load(const QString& path)
{
    ParserConfiguration* config = 0;

    QDomDocument doc;
    QFile file(path);
    if (file.open(QIODevice::ReadOnly)) {
        if (doc.setContent(&file)) {
            QDomElement docElem = doc.documentElement();
            std::list<FramingProtocolParser*> framingProtocolParsers;
            std::list<PacketLayerParser*> packetLayerParsers;

            // Find the Framing Protocol Parser Node
            QDomElement framingProtocolParserNode(docElem.firstChildElement("FramingProtocolParsers"));
            if (!framingProtocolParserNode.isNull()) {
                FramingProtocolParserFactory factory;
                QDomNodeList nodeList(framingProtocolParserNode.elementsByTagName("Parser"));
                for (int i=0; i < nodeList.count(); ++i) {
                    QDomElement parserElement(nodeList.at(i).toElement());
                    FramingProtocolParser* parser = factory.createParser(parserElement.attribute("name").toStdString(), parserElement.attribute("flags").toStdString());
                    if (parser) {
                        framingProtocolParsers.push_back(parser);
                    }
                }
            }


            // Find the Packet Layer Parser Node
            QDomElement packetLayerParserNode(docElem.firstChildElement("PacketLayerParsers"));
            if (!packetLayerParserNode.isNull()) {
                PacketLayerParserFactory factory;
                QDomNodeList nodeList(packetLayerParserNode.elementsByTagName("Parser"));
                for (int i=0; i < nodeList.count(); ++i) {
                    QDomElement parserElement(nodeList.at(i).toElement());
                    PacketLayerParser* parser = factory.createParser(parserElement.attribute("name").toStdString(), i);
                    if (parser) {
                        packetLayerParsers.push_back(parser);
                    }
                }
            }

            // Create the packer parser
            PacketParser* parser = new PacketParser(packetLayerParsers);

            // Create the configuration
            config = new ParserConfiguration(parser, framingProtocolParsers);
        }

        file.close();
    }

    return config;
}
