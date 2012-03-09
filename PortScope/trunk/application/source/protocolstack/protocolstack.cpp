#include "protocolstack.h"
#include <QFile>
#include <QPluginLoader>
#include <QDir>
#include "../include/plugin.h"


//-----------------------------------------------------------------------------
ProtocolStack::ProtocolStack(const QString& path)
{
    if (!load(path)) {
        throw std::exception();
    }
}


//-----------------------------------------------------------------------------
ProtocolStack::~ProtocolStack()
{

}


//-----------------------------------------------------------------------------
QList<QWidget*> ProtocolStack::getDisplayWidgets() const
{
    QList<QWidget*> list;
    queryDisplayWidgets(_root, list);
    return list;
}


//-----------------------------------------------------------------------------
bool ProtocolStack::load(const QString& path)
{
    bool result = false;

    QDomDocument doc;
    QFile file(path);
    if (file.open(QIODevice::ReadOnly)) {
        if (doc.setContent(&file)) {
            QDomElement docElem = doc.documentElement();

            // Create the root item
            _root = new ProtocolStackItem("root", "root", new RootDispatcher(), 0);

            // Recursively load children
            loadChildren(docElem, _root);

            // Success
            result = true;
        }

        file.close();
    }

    return result;
}


//-----------------------------------------------------------------------------
ProtocolStackItem* ProtocolStack::createStackItem(const QDomElement& e, ProtocolStackItem* parent)
{
    ProtocolStackItem* item = 0;

    if (e.tagName() == "Protocol") {

        // attempt to load the plugin
        QString PluginDir("../plugins/bin/");
        QString PluginPath = PluginDir + e.attribute("plugin");
        QPluginLoader loader(PluginPath);
        QObject* obj = loader.instance();

        if (obj) {
            Plugin* plugin = qobject_cast<Plugin*>(obj);
            if (plugin) {

                // Get the attributes
                QString name(e.attribute("name", "unnamed"));
                QString type(plugin->displayText());

                // Create the stack item
                item = new ProtocolStackItem(name, type, plugin->createProtocolAnalyzer(), parent);
            }
        }
    }

    return item;
}


//-----------------------------------------------------------------------------
void ProtocolStack::loadChildren(const QDomElement& e, ProtocolStackItem* parent)
{


    // Load all the children
    QDomNode n = e.firstChild();
    while(!n.isNull()) {
        QDomElement c = n.toElement();
        if (!c.isNull()) {

            ProtocolStackItem* item = createStackItem(c, parent);
            if (item) {
                loadChildren(c, item);
            }
        }
        n = n.nextSibling();
    }
}


//-----------------------------------------------------------------------------
void ProtocolStack::queryDisplayWidgets(ProtocolStackItem* parent, QList<QWidget*>& list) const
{
    if (parent->analyzer()->displayWidget()) {
        list.append(parent->analyzer()->displayWidget());
    }

    const QList<ProtocolStackItem*>& children(parent->children());
    ProtocolStackItem* child;
    foreach(child, children) {
        queryDisplayWidgets(child, list);
    }
}
