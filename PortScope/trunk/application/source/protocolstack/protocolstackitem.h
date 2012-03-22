/*  protocolstackitem.h
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
#ifndef __PROTOCOLSTACKITEM_H__
#define __PROTOCOLSTACKITEM_H__

#include <QWidget>
#include <QtPlugin>
#include "protocolanalyzer.h"



class ProtocolStackItem
{
public:
    ProtocolStackItem(
        const QString& name,
        const QString& type,
        ProtocolAnalyzer* analyzer,
        ProtocolStackItem* parent);
    ~ProtocolStackItem();

    QString name() const;
    QString type() const;
    ProtocolAnalyzer* analyzer() const;

    // Child functions
    void appendChild(ProtocolStackItem *child);
    const QList<ProtocolStackItem*>& children() const;

    // Model implementation
    ProtocolStackItem* child(int row);
    int childCount() const;
    int row() const;
    ProtocolStackItem* parent();



protected:
    QString _name;
    QString _type;
    ProtocolAnalyzer* _analyzer;
    QList<ProtocolStackItem*> _children;
    ProtocolStackItem* _parent;

};


#endif