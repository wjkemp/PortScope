#include "display.h"
#include "analyzer.h"


//-----------------------------------------------------------------------------
Display::Display(Analyzer* analyzer) :
    _analyzer(analyzer)
{
    setStyleSheet("font-family: Consolas;");
    setReadOnly(false);
}


//-----------------------------------------------------------------------------
Display::~Display()
{

}


//-----------------------------------------------------------------------------
void Display::insertPlainText(const QString& text)
{
    QTextEdit::setPlainText(text);
}
