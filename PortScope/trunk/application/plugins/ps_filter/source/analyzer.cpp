#include "analyzer.h"
#include "display.h"



//-----------------------------------------------------------------------------
Analyzer::Analyzer(const QString& name, Flags filterMask) :
    _name(name),
    _filterMask(filterMask),
    _bytesProcessed(0)
{
    _display = new Display(this);
    _display->setWindowTitle(name);
}


//-----------------------------------------------------------------------------
Analyzer::~Analyzer()
{

}


//-----------------------------------------------------------------------------
void Analyzer::addChild(ProtocolAnalyzer* child)
{
    _children.append(child);
}


//-----------------------------------------------------------------------------
QWidget* Analyzer::displayWidget()
{
    return _display;
}


//-----------------------------------------------------------------------------
void Analyzer::processData(const void* data, size_t length, Flags flags)
{

    if (_filterMask & flags) {
        _bytesProcessed += length;

        // Forward
        ProtocolAnalyzer* child;
        foreach (child, _children) {
            child->processData(data, length, flags);
        }
    }
}
