#include "analyzer.h"
#include "display.h"



//-----------------------------------------------------------------------------
Analyzer::Analyzer(const QString& name) :
    _name(name),
    _bytesProcessed(0)
{
    _display = new Display(this);
    _display->setWindowTitle(name);
    connect(this, SIGNAL(addToDisplay(const QString&)), _display, SLOT(insertPlainText(const QString&)));
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
    const unsigned char* ptr = (const unsigned char*)data;
    QString format;


    for (size_t i = 0; i < length; ++i) {
        format += QString("%1 ").arg(ptr[i], 2, 16, QChar('0')).toUpper();
        _bytesProcessed++;

        if (_bytesProcessed == 8) {
            format += "  ";

        } else if (_bytesProcessed == 16) {
            _bytesProcessed = 0;
            format += "\n";
        }

    }

    emit addToDisplay(format);

    // Forward
    ProtocolAnalyzer* child;
    foreach (child, _children) {
        child->processData(data, length, flags);
    }
}
