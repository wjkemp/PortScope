#ifndef __SLIPPARSER_H__
#define __SLIPPARSER_H__



class SlipParser
{

public:
    SlipParser(size_t bufferLength);
    ~SlipParser();
    void process(const void* data, size_t length);

protected:
    virtual void frameReceived(const void* data, size_t length) = 0;

private:
    unsigned char* _buffer;
    size_t _bufferLength;
    size_t _index;
    bool _inEscape;

};

#endif
