#ifndef PLMTEXT_H_INCLUDED
#define PLMTEXT_H_INCLUDED

#include <vector>
#include <string>
#include "../../common.h"

///=============Горизонтальное выравнивание
#define TEXT_ALIGN_LEFT      0
#define TEXT_ALIGN_CENTER    0x1
#define TEXT_ALIGN_RIGHT     0x2
///=============Вертикальное выравнивание
#define TEXT_VALIGN_TOP      0
#define TEXT_VALIGN_MIDDLE   0x4
#define TEXT_VALIGN_BOTTOM   0x8

#include "font.h"

BEGIN_GUI_NAMESPACE

class text
{
public:
    vec2f rect;

    text(font* = 0);
    text(const std::string, font* = 0);
    text(const std::wstring, font* = 0);

    void make(const std::string);
    void make(const std::wstring);

    void format(const std::string, ...);
    void format(const std::wstring, ...);

    void push_back(const std::string);
    void push_back(const std::wstring);

    //void push_back(char);
    void push_back(wchar_t);

    void insert(size_t, const std::string, size_t = 0, int = -1);
    void insert(size_t, const std::wstring, size_t = 0, int = -1);
    void insert(size_t, wchar_t);

    void pop_back();
    void clear();
    void erase(size_t = 0, size_t = -1);

    size_t length();
    size_t size();
    bool empty();

    void draw(vec2f);

    void setFont(font*);

    const std::wstring value();

    void operator+=(const std::string str){push_back(str);}
    void operator+=(const std::wstring str){push_back(str);}

    //void operator+=(char str){push_back(str);}
    void operator+=(wchar_t str){push_back(str);}

    void operator=(const std::string str){make(str);}
    void operator=(const std::wstring str){make(str);}
protected:
    std::wstring _value;

    void _remath();
    bool _mathChar(wchar_t, uint*);

public:
    vec4f color;
    float scale;
    uchar style;

    static const std::wstring UTF8toWide(const std::string);
private:

    void _mathParams();

    std::vector<uint> _data;
    std::vector<float> _spaceSize;

    font *_font;
    font::drawGlyphInfo *_glyphs;
};


END_GUI_NAMESPACE

#endif // PLMTEXT_H_INCLUDED
