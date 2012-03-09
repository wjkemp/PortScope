#ifndef __PROTOCOLSTACKITEM_H__
#define __PROTOCOLSTACKITEM_H__

#include <QWidget>
#include <QtPlugin>
#include "protocolanalyzer.h"



class ProtocolStackItem
{
public:
    ProtocolStackItem(
        const QString& name,
        const QString& type,
        ProtocolAnalyzer* analyzer,
        ProtocolStackItem* parent);
    ~ProtocolStackItem();

    QString name() const;
    QString type() const;
    ProtocolAnalyzer* analyzer() const;

    // Child functions
    void appendChild(ProtocolStackItem *child);
    const QList<ProtocolStackItem*>& children() const;

    // Model implementation
    ProtocolStackItem* child(int row);
    int childCount() const;
    int row() const;
    ProtocolStackItem* parent();



protected:
    QString _name;
    QString _type;
    ProtocolAnalyzer* _analyzer;
    QList<ProtocolStackItem*> _children;
    ProtocolStackItem* _parent;

};


#endif