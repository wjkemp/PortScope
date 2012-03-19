#include "analyzer.h"
#include "display.h"



//-----------------------------------------------------------------------------
Analyzer::Analyzer(const QString& name) :
    _name(name),
    _bytesProcessed(0)
{
    _display = new Display();
    _display->setWindowTitle(name);
    connect(this, SIGNAL(updateDisplay()), _display, SLOT(update()));
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

    _display->addData((const unsigned char*)data, length);

    emit updateDisplay();

    // Forward
    ProtocolAnalyzer* child;
    foreach (child, _children) {
        child->processData(data, length, flags);
    }
}
