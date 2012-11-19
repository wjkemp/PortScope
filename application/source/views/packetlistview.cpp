/*  packetlistview.cpp
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
#include "packetlistview.h"
#include <QHeaderView>


//-----------------------------------------------------------------------------
PacketListView::PacketListView(QWidget* parent)
{
    setStyleSheet("font-family: Consolas;");
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);

    QPalette palette(palette());
    palette.setBrush(QPalette::Inactive, QPalette::Highlight, QBrush(QColor(51, 153, 255)));
    palette.setColor(QPalette::Inactive, QPalette::HighlightedText, Qt::white);
    palette.setBrush(QPalette::Active, QPalette::Highlight, QBrush(QColor(51, 153, 255)));
    setPalette(palette);
}


//-----------------------------------------------------------------------------
PacketListView::~PacketListView()
{

}


//-----------------------------------------------------------------------------
void PacketListView::setModel(QAbstractItemModel* model)
{

    // Store the old model
    QAbstractItemModel* oldModel(QTableView::model());

    // Set the new model
    QTableView::setModel(model);


    // Update the view if the model is valid
    if (model) {
        horizontalHeader()->show();
        verticalHeader()->hide();
        verticalHeader()->setDefaultSectionSize(18);
        horizontalHeader()->setResizeMode(0, QHeaderView::Fixed);
        horizontalHeader()->resizeSection(0, 80);
        horizontalHeader()->setResizeMode(1, QHeaderView::Fixed);
        horizontalHeader()->resizeSection(1, 80);
        horizontalHeader()->setResizeMode(2, QHeaderView::Fixed);
        horizontalHeader()->resizeSection(2, 80);
        horizontalHeader()->setResizeMode(3, QHeaderView::Stretch);
    }

    // Delete the old model if valid
    if (oldModel) {
        delete oldModel;
    }
}


//-----------------------------------------------------------------------------
void PacketListView::endInsertPackets()
{
    scrollToBottom();
}


//-----------------------------------------------------------------------------
void PacketListView::beginClearDatabase(size_t count)
{
    clearSelection();
}
