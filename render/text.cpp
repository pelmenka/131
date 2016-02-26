#include <GL/glew.h>
#include <string.h>
#include <string>
#include <stdarg.h>
#include <stdio.h>

#include <locale>
#include <wchar.h>
#include "text.h"
#include "../window.h"

int _getBytesCount(char);
bool _isStart(char);
bool _isASCII(char);
unsigned int _getSymbol(char*, int);

using namespace std;

namespace render{

text::text(font *i)
{
    color = {0, 0, 0, 1};
    scale = 1;
    style = 0;
    _glyphs = 0;
    if(i)
        _font = i;
    else
        _font = 0;
}

text::text(const std::string &str, font *i):text(i)
{
    make(str);
}

text::text(const std::wstring &str, font *i):text(i)
{
    make(str);
}

void text::make(const std::wstring &str)
{
    _value = str;
    _remath();
}

void text::push_back(const std::wstring &str)
{
    _value += str;
    _remath();
}

void text::push_back(wchar_t sym)
{
    _value += sym;
    _remath();
}

void text::insert(size_t start, const std::wstring &str, size_t start2, int len)
{
    if(len < 0)
        len = str.length();

    _value.insert(start, str, start2, len);

    _remath();
}

void text::insert(size_t start, wchar_t val)
{
    _value.insert(start, &val, 1);
    _remath();
}

void text::format(const std::wstring &format, ...)
{
    if(format.find('%') == -1)
    make(format);
    else
    {
        wchar_t buffer[1024];

        va_list ap;
        va_start(ap, format);
        vsnwprintf(buffer, 1024, format.c_str(), ap);
        va_end(ap);

        make(buffer);
    }
}


void text::make(const std::string &str)
{
    _value = UTF8toWide(str);
    _remath();
}

void text::push_back(const std::string &str)
{
    _value += UTF8toWide(str);
    _remath();
}

void text::insert(size_t start, const std::string &str, size_t start2, int len)
{
    if(len < 0)
        len = str.length();

    _value.insert(start, UTF8toWide(str), start2, len);

    _remath();
}

void text::format(const std::string &format, ...)
{
    if(format.find('%') == -1)
            make(format);
    else
    {
        char buffer[1024];

        va_list ap;
        va_start(ap, format);
        vsprintf(buffer, format.c_str(), ap);
        va_end(ap);

        make(buffer);
    }
}


void text::pop_back()
{
    _value.pop_back();
    _remath();
}

void text::clear()
{
    _value.clear();
    _data.clear();
}

void text::erase(size_t start, size_t _size)
{
    _value.erase(start, _size);
    _remath();
}

size_t text::length()
{
    return _value.length();
}

size_t text::size()
{
    return _value.size();
}

void text::setFont(font *f)
{
    _font = f;
    _remath();
}

void text::_remath()
{
    if(!_font)
        return;

    _data.clear();
    _data.reserve(_value.length());

    uint temp;
    for(wchar_t i: _value)
        if(_mathChar(i, &temp))
            _data.push_back(temp);

    if(_glyphs)
        delete [] _glyphs;
    _glyphs = new font::drawGlyphInfo[_data.size()];
}

bool text::_mathChar(wchar_t c, uint *result)
{
    for(uint i = 0; i < _font->_charList.size(); i++)
        if(_font->_charList[i].code == c)
        {
            *result = i;
            return 1;
        }
    return 0;
}

void text::draw(vec2f pos)
{
    if(!_font)
    {
        _font = &render::defaultResources::textFont;
        _remath();
    }

    uint _drawCount = 0;

    vec2f gPos;

    float maxHeight = 0;
    float maxWidth = 0;

    uint currentString = 0;

    //_mathParams();
    uint count = _spaceSize.size();
    for(int i = 0; i < _data.size(); i++)
    {
        uint &val = _data[i];

        if(_font->_charList[val].code == ' ')
        {
            gPos.x += _font->_charList[val].size.x;
            continue;
        }

        if(_font->_charList[val].code == '\n')
        {
            if(maxWidth < gPos.x)
                maxWidth = gPos.x;

            gPos.x = 0;
            gPos.y += maxHeight;
            //currentString++;
            continue;
        }

        float kernInfo = 0;

        if(i)
        {
            for(auto &c: _font->_charList[_data[i-1]].kerning)
            {
                if(c.code == _font->_charList[val].code)
                    kernInfo = c.offset;
            }
        }

        _glyphs[_drawCount].code = val;
        _glyphs[_drawCount].pos = pos+gPos*scale*_font->defaultScale;

        _glyphs[_drawCount].color = color;

        gPos.x += _font->_charList[val].size.x;//+kernInfo;
        if(maxHeight < _font->_charList[val].size.y)
            maxHeight = _font->_charList[val].size.y;

        _drawCount++;
    }
    if(maxWidth < gPos.x)
        maxWidth = gPos.x;

    rect = vec2f(maxWidth, gPos.y+maxHeight);
    rect *= scale*_font->defaultScale;

    _font->draw(_glyphs, _drawCount, scale);
}

vec2f text::getCaretPos(uint pos)
{
    if(!_font)
    {
        _font = &render::defaultResources::textFont;
        _remath();
    }

    if(pos > _data.size())
        pos = _data.size();

    vec2f gPos;
    float maxWidth = 0;
    float maxHeight = 0;

    for(int i = 0; i < pos; i++)
    {
        uint &val = _data[i];

        if(_font->_charList[val].code == ' ')
        {
            gPos.x += _font->_charList[val].size.x;
            continue;
        }

        if(_font->_charList[val].code == '\n')
        {
            if(maxWidth < gPos.x)
                maxWidth = gPos.x;

            gPos.x = 0;
            gPos.y += maxHeight;
            //currentString++;
            continue;
        }
        gPos.x += _font->_charList[val].size.x;//+kernInfo;
        if(maxHeight < _font->_charList[val].size.y)
            maxHeight = _font->_charList[val].size.y;
    }
    return gPos*scale*_font->defaultScale;
}

const std::wstring &text::value()
{
    return _value;
}

std::string text::WidetoUTF8(const std::wstring &str)
{
    /*char temp[6];
    int len;
    std::string result;
    for(const wchar_t &c: str)
    {
             if(c <= 0x7f) // ANSI
             {
                 result += c&0x7f;
                 continue;
             }
        else if(c <= 0x7ff) len = 2;
        else if(c <= 0xffff) len = 3;
        else if(c <= 0x1fffff) len = 4;
        else if(c <= 0x3ffffff) len = 5;
        else if(c <= 0x7fffffff) len = 6;
        else continue;
        memset(temp, 0, len);
        int bytes = 6+5*len-1;

        for(int i = 0; i < len; i++)
            temp[0] |= 1<<(7-i);

        for(int i = 0; i < 7-len; i++)
            temp[0] |= ((c>>(bytes--))&1)<<i;

        for(int i = 1; i < len; i++)
        {
            temp[i] |= 0x80;
            for(int j = 0; j < 6 && bytes; j++)
                temp[i] |= ((c>>(bytes--))&1)<<j;
        }

        for(int i = 0; i < len; i++, printf(" %x ", temp[i-1]))
            for(int j = 7; j >= 0; j--)
                printf("%d", (temp[i]>>j)&1);
        putchar('\n');

        for(int j = 15; j >= 0; j--)
            printf("%d", (c>>j)&1);
        putchar('\n');

        result.append(temp, len);
    }
    return result;*/
    std::string result;
    char temp[6];
    for(int i = 0; i < str.size(); i++)
    {
        if(str[i] < 0x80)
            result += str[i]&0xFF;
        else
        {
            if(str[i]>0x7fffffff) continue; //вне диапазона UTF-8
            memset(temp, 0, 6);
            int len = 2;
            if(str[i]>0x7ff) len++;
            if(str[i]>0xffff) len++;
            if(str[i]>0x1fffff) len++;
            if(str[i]>0x3ffffff) len++;

            for(int j = 0; j < len; j++)
                temp[0] |= 1<<(7-j);

            uint fbyte = 6+5*(len-1);

            for(int j = 0; j < 8-len; j++)
                temp[0] |= (((str[i]>>fbyte--)&1)<<(7-len-j));

            for(int j = 1; j < len; j++)
            {
                temp[j] |= 0x80;
                for(int k = 0; k < 6; k++)
                    temp[j] |= ((str[i]>>fbyte--)&1)<<(5-k);
            }
            result.append(temp, len);
        }
    }

    return result;
}

std::wstring text::UTF8toWide(const std::string &str)
{
    char temp[4];
    int len;
    std::wstring result;

    for(int i = 0; i < str.size(); i++)
        if(_isASCII(str[i]))
            result += (wchar_t)str[i];
        else
        {
            memset(temp, 0, 4);
            len = _getBytesCount(str[i])+1;
            for(int j = 0; j <= len; j++)
                temp[j] = str[i+j];

            i += len-1;
            result += (wchar_t)(_getSymbol(temp, len)&0xffff);
        }
    return result;
}

void text::_mathParams()
{
    _spaceSize.clear();

    std::vector<float> strLen;
    std::vector<float> strSize;
    strLen.push_back(0.0);
    strSize.push_back(0);
    float maxWidth = 0;
    uint num = 0;
    for(uint i: _data)
    {
        if(_font->_charList[i].code == ' ')
            continue;
        if(_font->_charList[i].code == '\n')
        {
            if(maxWidth < strLen[num])
                maxWidth = strLen[num];

            strLen.push_back(0.0);
            strSize.push_back(0);
            num++;
            continue;
        }
        strSize[num]++;
        strLen[num] += _font->_charList[i].size.x;
    }

    if(maxWidth < strLen[num])
        maxWidth = strLen[num];
    if(!num)
        num++;
    for(int i = 0; i < num; i++)
    {
        float temp = maxWidth/strLen[num];
        _spaceSize.push_back(temp);
    }
}

}

int _getBytesCount(char c)
{
    int ret = 0;
    for(int i = 0; i < 8; i++)
    {
        if((c>>(7-i))&1) ret++;
        else break;
    }
    if(ret > 0) ret--;
    return ret;
}

unsigned int _getSymbol(char *data, int size)
{
    unsigned int result = 0, mask;

    for(int i = 0; i < size; i++)
    {
        if(i) mask = 0x3f;
        else  mask = (0x1f>>(size-2))&0x1f;

        result |= (data[i]&mask)<<(6*(size-i-1));
    }
    return result;
}

bool _isASCII(char c)
{
    return !((c>>7)&1);
}

bool _isStart(char c)
{
    return (((c>>6)&3) == 2);
}
