/*  captureengine.h
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
#ifndef __CAPTUREENGINE_H__
#define __CAPTUREENGINE_H__

#include <QThread>
#include <QMutex>
#include "captureengineconfiguration.h"
#include "packetdatabase/packetdatabase.h"
#include "packetdatabase/packetparser.h"


class CaptureEngine : public QThread
{
    Q_OBJECT

public:
    CaptureEngine(PacketDatabase* packetDatabase, PacketParser* packetParser, std::list<FramingProtocolParser*> framingProtocolParsers);
    ~CaptureEngine();

    bool start(const CaptureEngineConfiguration& config);
    void stop();

signals:
    void started();
    void stopped();
    void error(const QString& error);
    void packetReceived(Packet* packet);

protected:
    void run();
    void process(const unsigned char* data, size_t length, PacketFlags flags);

private:
    PacketDatabase* _packetDatabase;
    PacketParser* _packetParser;
    std::list<FramingProtocolParser*> _framingProtocolParsers;
    CaptureEngineConfiguration _config;
    QMutex _lock;
    bool _stop;

};

#endif
