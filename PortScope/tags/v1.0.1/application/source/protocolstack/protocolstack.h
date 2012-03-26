/*  protocolstack.h
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
#ifndef __PROTOCOLSTACK_H__
#define __PROTOCOLSTACK_H__

#include "protocolstackitem.h"
#include "rootdispatcher.h"
#include <QDomDocument>


class ProtocolStack : public QObject
{
    Q_OBJECT

public:
    ProtocolStack(const QString& path);
    ~ProtocolStack();
    ProtocolStackItem* root() const { return _root; }
    QList<QWidget*> getDisplayWidgets() const;

private:
    bool load(const QString& path);
    ProtocolStackItem* createStackItem(const QDomElement& e, ProtocolStackItem* parent);
    void loadChildren(const QDomElement& e, ProtocolStackItem* parent);
    void queryDisplayWidgets(ProtocolStackItem* parent, QList<QWidget*>& list) const;

private:
    ProtocolStackItem* _root;
};


#endif
