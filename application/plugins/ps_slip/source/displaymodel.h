/******************************************************************************
	displaymodel.h
 ******************************************************************************/
#ifndef __DISPLAYMODEL_H__
#define __DISPLAYMODEL_H__

#include <QAbstractTableModel>
#include <QVector>
#include "slipframe.h"


//-----------------------------------------------------------------------------
//  Class Definition
//-----------------------------------------------------------------------------
class DisplayModel : public QAbstractTableModel
{
    Q_OBJECT


public:

    enum Columns
    {
        ColTimestamp,
        ColFrameLength,
        ColPayload,
        ColumnCount
    };

public:
    DisplayModel(QObject* parent = 0);
    ~DisplayModel();

    void addItem(const SlipFrame& frame);

    QVariant data(const QModelIndex& index, int role) const;
    Qt::ItemFlags flags(const QModelIndex& index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;


private:
    QVector<SlipFrame> _items;


};
#endif
