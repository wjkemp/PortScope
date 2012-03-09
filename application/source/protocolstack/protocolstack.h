#ifndef __PROTOCOLSTACK_H__
#define __PROTOCOLSTACK_H__

#include "protocolstackitem.h"
#include "rootdispatcher.h"
#include <QDomDocument>


class ProtocolStack : public QObject
{
    Q_OBJECT

public:
    ProtocolStack(const QString& path);
    ~ProtocolStack();
    ProtocolStackItem* root() const { return _root; }
    QList<QWidget*> getDisplayWidgets() const;

private:
    bool load(const QString& path);
    ProtocolStackItem* createStackItem(const QDomElement& e, ProtocolStackItem* parent);
    void loadChildren(const QDomElement& e, ProtocolStackItem* parent);
    void queryDisplayWidgets(ProtocolStackItem* parent, QList<QWidget*>& list) const;

private:
    ProtocolStackItem* _root;
};


#endif
