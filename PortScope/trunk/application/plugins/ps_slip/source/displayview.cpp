/******************************************************************************
	displayview.cpp
 ******************************************************************************/
#include "displayview.h"
#include <QHeaderView>


//-----------------------------------------------------------------------------
DisplayView::DisplayView(QWidget* parent)
{
    setStyleSheet("font-family: Consolas;");
}


//-----------------------------------------------------------------------------
DisplayView::~DisplayView()
{
    int x = 0;
}


//-----------------------------------------------------------------------------
void DisplayView::setModel(QAbstractItemModel* model)
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
        horizontalHeader()->setResizeMode(2, QHeaderView::Stretch);
    }

    // Delete the old model if valid
    if (oldModel) {
        delete oldModel;
    }
}


//-----------------------------------------------------------------------------
void DisplayView::updateDisplay()
{
    scrollToBottom();
}


//-----------------------------------------------------------------------------
QSize DisplayView::sizeHint() const
{
    return QSize(640, 480);
}

