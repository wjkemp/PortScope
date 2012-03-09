/******************************************************************************
	protocolstackview.h
 ******************************************************************************/
#ifndef __PROTOCOLSTACKVIEW_H__
#define __PROTOCOLSTACKVIEW_H__

#include <QTreeView>
#include "protocolstackmodel.h"
#include "protocolstack.h"


//-----------------------------------------------------------------------------
//  Class Definition
//-----------------------------------------------------------------------------
class ProtocolStackView : public QTreeView
{
    Q_OBJECT

public:
    ProtocolStackView(QWidget* parent = 0);
    ~ProtocolStackView();
    void setProtocolStack(ProtocolStack* stack);

signals:
    void displayWidgetChanged(QWidget* widget);

protected slots:
    void onSelectionChanged(const QModelIndex& current, const QModelIndex& previous);

private:
    ProtocolStackModel* _model;

};
#endif
