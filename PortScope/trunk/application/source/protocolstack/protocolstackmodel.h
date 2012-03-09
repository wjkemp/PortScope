/******************************************************************************
    protocolstackmodel.h
 ******************************************************************************/
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
