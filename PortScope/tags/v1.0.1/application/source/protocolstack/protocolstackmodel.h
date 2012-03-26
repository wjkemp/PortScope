/*  protocolstackmodel.h
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
#ifndef __PROTOCOLSTACKMODEL_H__
#define __PROTOCOLSTACKMODEL_H__

#include <QAbstractItemModel>
#include "protocolstackitem.h"


//-----------------------------------------------------------------------------
//  Class Definition
//-----------------------------------------------------------------------------
class ProtocolStackModel : public QAbstractItemModel
{
    Q_OBJECT

public:

    enum Columns
    {
        ColName,
        ColType,
        ColumnCount
    };

public:
    ProtocolStackModel(ProtocolStackItem* root, QObject* parent = 0);
    ~ProtocolStackModel();

    ProtocolStackItem* itemAt(const QModelIndex& index) const;

    QVariant data(const QModelIndex& index, int role) const;
    Qt::ItemFlags flags(const QModelIndex& index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex& index) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;

public slots:
    void externalEditOccured();

private:
    ProtocolStackItem* _rootItem;
};
#endif
