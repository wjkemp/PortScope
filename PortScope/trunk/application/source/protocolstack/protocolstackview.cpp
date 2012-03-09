/******************************************************************************
	protocolstackview.cpp
 ******************************************************************************/
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
    }

}


//-----------------------------------------------------------------------------
void ProtocolStackView::onSelectionChanged(const QModelIndex& current, const QModelIndex& previous)
{
    ProtocolStackItem* item = _model->itemAt(current);
    if (item) {
        emit displayWidgetChanged(item->analyzer()->displayWidget());
    }
}
