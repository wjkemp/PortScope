#include "analyzer.h"
#include "display.h"



//-----------------------------------------------------------------------------
Analyzer::Analyzer() :
    _bytesProcessed(0)
{
    _display = new Display(this);
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
    _bytesProcessed += length;


    // Forward
    ProtocolAnalyzer* child;
    foreach (child, _children) {
        child->processData(data, length, flags);
    }
}
