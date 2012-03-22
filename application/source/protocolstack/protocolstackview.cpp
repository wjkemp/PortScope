/*  protocolstackview.cpp
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
#include "protocolstackview.h"
#include <QHeaderView>


//-----------------------------------------------------------------------------
ProtocolStackView::ProtocolStackView(QWidget* parent) :
    _model(0)
{
}


//-----------------------------------------------------------------------------
ProtocolStackView::~ProtocolStackView()
{

}


//-----------------------------------------------------------------------------
void ProtocolStackView::setProtocolStack(ProtocolStack* stack)
{

    // Clear the model
    setModel(0);

    // Kill the current model, if any
    if (_model) {
        delete _model;
        _model = 0;
    }

    // Create a new model
    if (stack) {
        _model = new ProtocolStackModel(stack->root());
        setModel(_model);
        setSelectionMode(QAbstractItemView::SingleSelection);
        QItemSelectionModel* selectionModel = new QItemSelectionModel(_model);
        setSelectionModel(selectionModel);
        connect(selectionModel, SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)), SLOT(onSelectionChanged(const QModelIndex&, const QModelIndex&)));

        header()->resizeSection(0, 200);
        header()->setResizeMode(1, QHeaderView::Stretch);
    }

}


//-----------------------------------------------------------------------------
void ProtocolStackView::onSelectionChanged(const QModelIndex& current, const QModelIndex& previous)
{
    ProtocolStackItem* item = _model->itemAt(current);
    if (item) {
        QWidget* displayWidget = item->analyzer()->displayWidget();
        if (displayWidget) {
            emit displayWidgetChanged(displayWidget);
        }
    }
}
