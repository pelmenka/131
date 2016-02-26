#include <GL/glew.h>
#include <string.h>
#include <string>
#include <stdarg.h>
#include <stdio.h>

#include "../gui.h"
#include "../../render/resources.h"

#include <locale>
#include <wchar.h>

int _getBytesCount(char);
bool _isStart(char);
bool _isASCII(char);
unsigned int _getSymbol(char*, int);

using namespace std;

BEGIN_GUI_NAMESPACE

text::text(font *i)
{
    color = {1, 1, 1, 1};
    scale = 1;
    style = 0;
    _glyphs = 0;
    if(i)
        _font = i;
    else
        _font = &_resource::defaultFont;
}

text::text(const std::string str, font *i):text(i)
{
    make(str);
}

text::text(const std::wstring str, font *i):text(i)
{
    make(str);
}

void text::make(const std::wstring str)
{
    _value = str;
    _remath();
}

void text::push_back(const std::wstring str)
{
    _value += str;
    _remath();
}

void text::push_back(wchar_t sym)
{
    _value += sym;
    _remath();
}

void text::insert(size_t start, const std::wstring str, size_t start2, int len)
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

void text::format(std::wstring format, ...)
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


void text::make(const std::string str)
{
    _value = UTF8toWide(str);
    _remath();
}

void text::push_back(const std::string str)
{
    _value += UTF8toWide(str);
    _remath();
}

void text::insert(size_t start, const std::string str, size_t start2, int len)
{
    if(len < 0)
        len = str.length();

    _value.insert(start, UTF8toWide(str), start2, len);

    _remath();
}

void text::format(std::string format, ...)
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

const std::wstring text::value()
{
    return _value;
}

const std::wstring text::UTF8toWide(const std::string str)
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

END_GUI_NAMESPACE

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
