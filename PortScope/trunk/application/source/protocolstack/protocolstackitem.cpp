/******************************************************************************
	protocolstackitem.cpp
 ******************************************************************************/
#include "protocolstackitem.h"



//----------------------------------------------------------------
ProtocolStackItem::ProtocolStackItem(
    const QString& name,
    const QString& type,
    ProtocolAnalyzer* analyzer,
    ProtocolStackItem* parent) :
    _name(name),
    _type(type),
    _analyzer(analyzer),
    _parent(parent)
{
    if (_parent) {
        _parent->appendChild(this);
    }
}


//----------------------------------------------------------------
ProtocolStackItem::~ProtocolStackItem()
{
    delete _analyzer;
    qDeleteAll(_children);
}


//----------------------------------------------------------------
QString ProtocolStackItem::name() const
{
    return _name;
}


//----------------------------------------------------------------
QString ProtocolStackItem::type() const
{
    return _type;
}


//----------------------------------------------------------------
ProtocolAnalyzer* ProtocolStackItem::analyzer() const
{
    return _analyzer;
}


//----------------------------------------------------------------
void ProtocolStackItem::appendChild(ProtocolStackItem* item)
{
    _children.append(item);
    _analyzer->addChild(item->analyzer());
}


//----------------------------------------------------------------
const QList<ProtocolStackItem*>& ProtocolStackItem::children() const
{
    return _children;
}


//----------------------------------------------------------------
ProtocolStackItem* ProtocolStackItem::child(int row)
{
    return _children.value(row);
}


//----------------------------------------------------------------
int ProtocolStackItem::childCount() const
{
    return _children.count();
}




//----------------------------------------------------------------
ProtocolStackItem* ProtocolStackItem::parent()
{
    return _parent;
}


//----------------------------------------------------------------
int ProtocolStackItem::row() const
{
    if (_parent) {
        return _parent->children().indexOf(const_cast<ProtocolStackItem*>(this));

    } else {
        return 0;
    }
}
