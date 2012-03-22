/*  protocolstackitem.cpp
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
#include "protocolstackitem.h"



//----------------------------------------------------------------
ProtocolStackItem::ProtocolStackItem(
    const QString& name,
    const QString& type,
    ProtocolAnalyzer* analyzer,
    ProtocolStackItem* parent) :
    _name(name),
    _type(type),
    _analyzer(analyzer),
    _parent(parent)
{
    if (_parent) {
        _parent->appendChild(this);
    }
}


//----------------------------------------------------------------
ProtocolStackItem::~ProtocolStackItem()
{
    delete _analyzer;
    qDeleteAll(_children);
}


//----------------------------------------------------------------
QString ProtocolStackItem::name() const
{
    return _name;
}


//----------------------------------------------------------------
QString ProtocolStackItem::type() const
{
    return _type;
}


//----------------------------------------------------------------
ProtocolAnalyzer* ProtocolStackItem::analyzer() const
{
    return _analyzer;
}


//----------------------------------------------------------------
void ProtocolStackItem::appendChild(ProtocolStackItem* item)
{
    _children.append(item);
    _analyzer->addChild(item->analyzer());
}


//----------------------------------------------------------------
const QList<ProtocolStackItem*>& ProtocolStackItem::children() const
{
    return _children;
}


//----------------------------------------------------------------
ProtocolStackItem* ProtocolStackItem::child(int row)
{
    return _children.value(row);
}


//----------------------------------------------------------------
int ProtocolStackItem::childCount() const
{
    return _children.count();
}




//----------------------------------------------------------------
ProtocolStackItem* ProtocolStackItem::parent()
{
    return _parent;
}


//----------------------------------------------------------------
int ProtocolStackItem::row() const
{
    if (_parent) {
        return _parent->children().indexOf(const_cast<ProtocolStackItem*>(this));

    } else {
        return 0;
    }
}
