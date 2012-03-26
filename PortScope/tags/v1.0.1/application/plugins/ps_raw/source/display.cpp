/*  display.cpp
 *
 *  Copyright (C) 2012 Willem Kemp <http://www.thenocturnaltree.com>
 *  All rights reserved.
 *
 *  This file is part of PortScope.
 *
 *  PortScope is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  PortScope is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with PortScope. If not, see http://www.gnu.org/licenses/.
 *
 */
#include "display.h"
#include "analyzer.h"
#include <QPainter>
#include <QPaintEvent>
#include <QHBoxLayout>
#include <QVBoxLayout>


//-----------------------------------------------------------------------------
static char _hex[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };




//-----------------------------------------------------------------------------
class AddressDisplay : public QWidget
{
public:
    AddressDisplay(Display* display) :
        _display(display) {

        setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    }

protected:
    QSize sizeHint() const {
        return QSize(80, 0);
    }

    void paintEvent(QPaintEvent* event) {
        _display->paintAddressWidget(this);
    }

private:
    Display* _display;
};



//-----------------------------------------------------------------------------
class HexDisplay : public QWidget
{
public:
    HexDisplay(Display* display) :
        _display(display) {

        setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    }

protected:
    QSize sizeHint() const {
        return QSize(380, 0);
    }

    void paintEvent(QPaintEvent* event) {
        _display->paintHexWidget(this);
    }

private:
    Display* _display;
};



//-----------------------------------------------------------------------------
class AsciiDisplay : public QWidget
{
public:
    AsciiDisplay(Display* display) :
        _display(display) {

        setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    }

protected:
    QSize sizeHint() const {
        return QSize(140, 0);
    }

    void paintEvent(QPaintEvent* event) {
        _display->paintAsciiWidget(this);
    }

private:
    Display* _display;
};




//-----------------------------------------------------------------------------
Display::Display()
{
    setStyleSheet("font-family: \"Courier New\";");
    _font = font();

    _lineWidth = 16;
    _linesPerBlob = 1024;
    _blobSize = (_lineWidth * _linesPerBlob);

    _bytesInCurrentBlob = 0;
    _currentBlob = 0;
    _scrollLine = 0;

    // Allocate the initial blob
    _blobs.append(new unsigned char[_blobSize]);


    // Allocate the format buffer
    _formatBuffer = new char[_lineWidth * 3];
    memset(_formatBuffer, 0, _lineWidth * 3);


    // Create the displays
    _addressDisplay = new AddressDisplay(this);
    _hexDisplay = new HexDisplay(this);
    _asciiDisplay = new AsciiDisplay(this);

    // Create the scrollbar
    _vScroll = new QScrollBar(Qt::Vertical, this);
    connect(_vScroll, SIGNAL(valueChanged(int)), SLOT(onScrollVertical(int)));

    // Lay out
    QHBoxLayout* layout = new QHBoxLayout();
    layout->addWidget(_addressDisplay);
    layout->addWidget(_hexDisplay);
    layout->addWidget(_asciiDisplay);
    layout->addWidget(_vScroll);

    setLayout(layout);
    updateScrollLimits();


    // Set the palette
    QPalette palette(palette());
    palette.setColor(QPalette::Window, Qt::white);
    setPalette(palette);
    setAutoFillBackground(true);

}


//-----------------------------------------------------------------------------
Display::~Display()
{

    delete _formatBuffer;
    for (int i=0; i < _blobs.size(); ++i) {
        delete [] _blobs[i];
    }
}


//-----------------------------------------------------------------------------
void Display::addData(const unsigned char* buffer, size_t length)
{

    const unsigned char* source = buffer;

    while (length) {

        size_t bytesAvailableInBlob = _blobSize - _bytesInCurrentBlob;
        size_t bytesToCopy = length;

        if (bytesToCopy > bytesAvailableInBlob) {
            bytesToCopy = bytesAvailableInBlob;
        }

        unsigned char* destination = &_blobs[_currentBlob][_bytesInCurrentBlob];
        memcpy(destination, source, bytesToCopy);

        length -= bytesToCopy;
        _bytesInCurrentBlob += bytesToCopy;
        if (_bytesInCurrentBlob == _blobSize) {

            _currentBlob++;
            _bytesInCurrentBlob = 0;
            _blobs.append(new unsigned char[_blobSize]);
        }
    }
}


//-----------------------------------------------------------------------------
void Display::update()
{
    updateScrollLimits();
    scrollToBottom();
    repaint();
}


//-----------------------------------------------------------------------------
void Display::onScrollVertical(int position)
{
    _scrollLine = position;
    repaint();
}


//-----------------------------------------------------------------------------
QSize Display::sizeHint() const
{
    return QSize(640, 480);
}


//-----------------------------------------------------------------------------
void Display::paintEvent(QPaintEvent* event)
{

}


//-----------------------------------------------------------------------------
void Display::paintAddressWidget(QWidget* widget)
{
    QPainter painter(widget);
    QRect rect(widget->rect());

    QFontMetrics fontMetrics(_font);


    size_t totalLines = (_currentBlob * _linesPerBlob) + (_bytesInCurrentBlob / _lineWidth);
    if (_bytesInCurrentBlob % _lineWidth) {
        totalLines++;
    }

    if (totalLines) {

        // Calculate the amount of lines visible on screen
        size_t linesOnScreen = (rect.height() / fontMetrics.height());
        if ((_scrollLine + linesOnScreen) > totalLines) {
            linesOnScreen = totalLines - _scrollLine;
        }


        // Draw
        for (size_t i=0; i < linesOnScreen; i++) {
            size_t lineIndex  = (_scrollLine + i) % _linesPerBlob;
            size_t address = (lineIndex * _lineWidth);

            QRect rect(0, (i * fontMetrics.height()), rect.width(), fontMetrics.height());
            formatAddress(_formatBuffer, address);
            
            painter.drawText(rect, _formatBuffer);
        }
    }
}


//-----------------------------------------------------------------------------
void Display::paintHexWidget(QWidget* widget)
{
    QPainter painter(widget);
    QRect rect(widget->rect());

    QFontMetrics fontMetrics(_font);

    size_t totalLines = (_currentBlob * _linesPerBlob) + (_bytesInCurrentBlob / _lineWidth);
    if (_bytesInCurrentBlob % _lineWidth) {
        totalLines++;
    }

    if (totalLines) {

        // Calculate the amount of lines visible on screen
        size_t linesOnScreen = (rect.height() / fontMetrics.height());
        if ((_scrollLine + linesOnScreen) > totalLines) {
            linesOnScreen = totalLines - _scrollLine;
        }


        // Draw
        for (size_t i=0; i < linesOnScreen; i++) {
            size_t blobIndex = (_scrollLine + i) / _linesPerBlob;
            size_t lineIndex  = (_scrollLine + i) % _linesPerBlob;
            size_t tailLength = (_bytesInCurrentBlob % _lineWidth);
            if (tailLength == 0) {
                tailLength = _lineWidth;
            }

            size_t lineWidth = ((_scrollLine + i)  == (totalLines - 1)) ? tailLength : _lineWidth;

            QRect rect(0, (i * fontMetrics.height()), rect.width(), fontMetrics.height());
            formatHex(_formatBuffer, &_blobs[blobIndex][lineIndex * _lineWidth], lineWidth);
            
            painter.drawText(rect, _formatBuffer);
        }
    }
}


//-----------------------------------------------------------------------------
void Display::paintAsciiWidget(QWidget* widget)
{
    QPainter painter(widget);
    QRect rect(widget->rect());

    QFontMetrics fontMetrics(_font);


    size_t totalLines = (_currentBlob * _linesPerBlob) + (_bytesInCurrentBlob / _lineWidth);
    if (_bytesInCurrentBlob % _lineWidth) {
        totalLines++;
    }

    if (totalLines) {

        // Calculate the amount of lines visible on screen
        size_t linesOnScreen = (rect.height() / fontMetrics.height());
        if ((_scrollLine + linesOnScreen) > totalLines) {
            linesOnScreen = totalLines - _scrollLine;
        }


        // Draw
        for (size_t i=0; i < linesOnScreen; i++) {
            size_t blobIndex = (_scrollLine + i) / _linesPerBlob;
            size_t lineIndex  = (_scrollLine + i) % _linesPerBlob;
            size_t tailLength = (_bytesInCurrentBlob % _lineWidth);
            if (tailLength == 0) {
                tailLength = _lineWidth;
            }

            size_t lineWidth = ((_scrollLine + i)  == (totalLines - 1)) ? tailLength : _lineWidth;

            QRect rect(0, (i * fontMetrics.height()), rect.width(), fontMetrics.height());
            formatAscii(_formatBuffer, &_blobs[blobIndex][lineIndex * _lineWidth], lineWidth);
            
            painter.drawText(rect, _formatBuffer);
        }
    }
}


//-----------------------------------------------------------------------------
void Display::updateScrollLimits()
{
    size_t totalLines = (_currentBlob * _linesPerBlob) + (_bytesInCurrentBlob / _lineWidth);
    if (_bytesInCurrentBlob % _lineWidth) {
        totalLines++;
    }

    if (totalLines) {
        _vScroll->setMinimum(0);
        _vScroll->setMaximum(totalLines - 1);
        _vScroll->show();
    } else {
        _vScroll->hide();
    }

}


//-----------------------------------------------------------------------------
void Display::scrollToBottom()
{
    QPainter painter(_hexDisplay);
    QRect rect(_hexDisplay->rect());

    QFontMetrics fontMetrics(_font);

    // Calculate the total lines
    size_t totalLines = (_currentBlob * _linesPerBlob) + (_bytesInCurrentBlob / _lineWidth);
    if (_bytesInCurrentBlob % _lineWidth) {
        totalLines++;
    }

    if (totalLines) {


        // Calculate the amount of lines visible on screen
        size_t linesOnScreen = (rect.height() / fontMetrics.height());
        if ((_scrollLine + linesOnScreen) > totalLines) {
            linesOnScreen = totalLines - _scrollLine;
        }

        if (linesOnScreen > totalLines) {
            _vScroll->setValue(0);

        } else {
            _vScroll->setValue(totalLines - linesOnScreen);
        }
    }

}


//-----------------------------------------------------------------------------
void Display::formatAddress(char* dest, size_t addr) const
{
    int shift = 28;
    for (int i=0; i < 8; ++i) {
        *dest++ = _hex[((addr >> shift) & 0xF)];
        shift -= 4;
    }

    *dest++ = ':';
    *dest++ = 0;

}


//-----------------------------------------------------------------------------
void Display::formatHex(char* dest, const unsigned char* source, size_t length) const
{
    for (size_t i=0; i < length; ++i) {
        *dest++ = _hex[((source[i] >> 4) & 0xF)];
        *dest++ = _hex[(source[i] & 0xF)];
        *dest++ = ' ';
    }

    *dest = 0;
}


//-----------------------------------------------------------------------------
void Display::formatAscii(char* dest, const unsigned char* source, size_t length) const
{
    for (size_t i=0; i < length; ++i) {
        if (isalnum(source[i])) {
            *dest++ = source[i];
        } else {
            *dest++ = '.';
        }
    }

    *dest = 0;
}
