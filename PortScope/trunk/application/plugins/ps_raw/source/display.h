#ifndef __DISPLAY_H__
#define __DISPLAY_H__

class Analyzer;
#include <QTextEdit>


class Display : public QTextEdit
{
    Q_OBJECT

public:
    Display(Analyzer* analyzer);
    ~Display();

public slots:
    void insertPlainText(const QString& text);

private:
    Analyzer* _analyzer;

};

#endif
