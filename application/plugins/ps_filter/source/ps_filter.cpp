/*  ps_filter.cpp
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
#include "ps_filter.h"
#include "analyzer.h"


//-----------------------------------------------------------------------------
PS_filter::PS_filter()
{
}


//-----------------------------------------------------------------------------
PS_filter::~PS_filter()
{

}


//-----------------------------------------------------------------------------
QString PS_filter::displayText() const
{
    return "Filter";
}

//-----------------------------------------------------------------------------
ProtocolAnalyzer* PS_filter::createProtocolAnalyzer(const QString& name, const QString& parameters)
{
    if (parameters == "TX") {
        return new Analyzer(name, ProtocolAnalyzer::TransmitData);
    } else {
        return new Analyzer(name, ProtocolAnalyzer::ReceiveData);
    }
}


//-----------------------------------------------------------------------------
Q_EXPORT_PLUGIN2(ps_filter, PS_filter)
