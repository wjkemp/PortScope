/*  display.h
 *
 *  Copyright 2012 Willem Kemp.
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
#ifndef __DISPLAY_H__
#define __DISPLAY_H__

class Analyzer;
#include <QWidget>
#include <QScrollBar>


class Display : public QWidget
{
    Q_OBJECT

    friend class AddressDisplay;
    friend class HexDisplay;
    friend class AsciiDisplay;

public:
    Display();
    ~Display();

    void addData(const unsigned char* buffer, size_t length);


public slots:
    void update();
    void onScrollVertical(int position);

protected:
    QSize sizeHint() const;
    void paintEvent(QPaintEvent* event);
    void paintAddressWidget(QWidget* widget);
    void paintHexWidget(QWidget* widget);
    void paintAsciiWidget(QWidget* widget);


private:
    void updateScrollLimits();
    void scrollToBottom();
    void formatAddress(char* dest, size_t addr) const;
    void formatHex(char* dest, const unsigned char* source, size_t length) const;
    void formatAscii(char* dest, const unsigned char* source, size_t length) const;

private:

    bool _scrollToBottom;

    size_t _lineWidth;
    size_t _linesPerBlob;
    size_t _blobSize;
    size_t _bytesInCurrentBlob;
    size_t _currentBlob;
    QVector<unsigned char*> _blobs;

    size_t _scrollLine;

    char* _formatBuffer;


    QScrollBar* _vScroll;
    QWidget* _addressDisplay;
    QWidget* _hexDisplay;
    QWidget* _asciiDisplay;

    QFont _font;


};

#endif
