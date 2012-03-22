/*  ps_slip.cpp
 *
 *  Copyright 2012 Willem Kemp.
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
#include "ps_slip.h"
#include "analyzer.h"


//-----------------------------------------------------------------------------
PS_slip::PS_slip()
{
}


//-----------------------------------------------------------------------------
PS_slip::~PS_slip()
{

}


//-----------------------------------------------------------------------------
QString PS_slip::displayText() const
{
    return "SLIP Framing Protocol";
}

//-----------------------------------------------------------------------------
ProtocolAnalyzer* PS_slip::createProtocolAnalyzer(const QString& name, const QString& parameters)
{
    return new Analyzer(name);
}


//-----------------------------------------------------------------------------
Q_EXPORT_PLUGIN2(ps_slip, PS_slip)
