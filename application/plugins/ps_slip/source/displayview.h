/******************************************************************************
	displayview.h
 ******************************************************************************/
#ifndef __DISPLAYVIEW_H__
#define __DISPLAYVIEW_H__

#include <QTableView>


//-----------------------------------------------------------------------------
//  Class Definition
//-----------------------------------------------------------------------------
class DisplayView : public QTableView
{
    Q_OBJECT

public:
    DisplayView(QWidget* parent = 0);
    ~DisplayView();

    void setModel(QAbstractItemModel* model);
};
#endif
