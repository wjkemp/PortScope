/*  analyzer.cpp
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
#include "analyzer.h"
#include "display.h"



//-----------------------------------------------------------------------------
Analyzer::Analyzer(const QString& name) :
    _name(name),
    _bytesProcessed(0)
{
    _display = new Display();
    _display->setWindowTitle(name);
    connect(this, SIGNAL(updateDisplay()), _display, SLOT(update()));
}


//-----------------------------------------------------------------------------
Analyzer::~Analyzer()
{

}


//-----------------------------------------------------------------------------
void Analyzer::addChild(ProtocolAnalyzer* child)
{
    _children.append(child);
}


//-----------------------------------------------------------------------------
QWidget* Analyzer::displayWidget()
{
    return _display;
}


//-----------------------------------------------------------------------------
void Analyzer::processData(const void* data, size_t length, Flags flags)
{

    _display->addData((const unsigned char*)data, length);

    emit updateDisplay();

    // Forward
    ProtocolAnalyzer* child;
    foreach (child, _children) {
        child->processData(data, length, flags);
    }
}