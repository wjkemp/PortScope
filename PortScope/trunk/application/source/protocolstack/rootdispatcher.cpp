#include "rootdispatcher.h"


//-----------------------------------------------------------------------------
QString RootDispatcher::name() const
{
    return "";
}

//-----------------------------------------------------------------------------
QWidget* RootDispatcher::displayWidget()
{
    return 0;
}


//-----------------------------------------------------------------------------
void RootDispatcher::addChild(ProtocolAnalyzer* child)
{
    _children.append(child);
}


//-----------------------------------------------------------------------------
void RootDispatcher::processData(const void* data, size_t length, Flags flags)
{
    // Forward
    ProtocolAnalyzer* child;
    foreach (child, _children) {
        child->processData(data, length, flags);
    }
}
