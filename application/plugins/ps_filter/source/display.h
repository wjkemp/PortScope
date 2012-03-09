#ifndef __DISPLAY_H__
#define __DISPLAY_H__

class Analyzer;
#include <QLabel>
#include <QTimer>


class Display : public QWidget
{
    Q_OBJECT

public:
    Display(Analyzer* analyzer);
    ~Display();

protected slots:
    void onUpdate();

private:
    Analyzer* _analyzer;
    QLabel* _bytesProcessed;
    QTimer* _updateTimer;

};

#endif
