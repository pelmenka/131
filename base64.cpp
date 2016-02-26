#include "base64.h"

inline bool valid(uchar c)
{
    return (c >= 'A' && c <= 'Z') ||
           (c >= 'a' && c <= 'z') ||
           (c >= '0' && c <= '9') ||
           c == '+' ||
           c == '/' ||
           c == '=';
}

unsigned char char2int(char c)
{
    if(c == '=') return 0;
         if(c >= 'A' && c <= 'Z') return (c - 'A');
    else if(c >= 'a' && c <= 'z') return (c - 'a'+26);
    else if(c >= '0' && c <= '9') return (c - '0'+52);
    else if(c == '+') return 62;
    return 63;
}

char int2char(uchar c)
{
         if(c < 26) return c + 'A';
    else if(c < 52) return c + 'a' - 26;
    else if(c < 62) return c + '0' - 52;
    else if(c == 62) return '+';
    return '/';
}

namespace base64{

uint decode(const char *in, uint len, uchar *out, uint outLen)
{
    int i = 0, j = 0, c = 0;
    uchar temp[4];
    char tempIn;

    while(len-- && valid(in[j]))
    {
        temp[i] = char2int(in[j++]);
        if(++i < 4) continue;
        out[c++] = (temp[0] << 0x2) | ((temp[1] & 0x30) >> 4);
        out[c++] = ((temp[1] & 0xf) << 4) | ((temp[2] & 0x3c) >> 2);
        out[c++] = ((temp[2] & 0x3) << 6) | temp[3];
        i = 0;
    }
    return c;
}

uint encode(const uchar *in, uint len, char *out, uint outLen)
{
    int i = 0, j = 0, c = 0;
    uint temp = 0;
    uchar tempOut[4];
    if(!outLen)
        outLen = encodeLen(len);
    while(len--)
    {

        temp |= (in[j++] << 8*(2-i++));
        if(len)
            if(i < 3) continue;

        for(c = 3-i; c < 4; c++)
            tempOut[c] = ((temp>>c*6)&0x3f);

        while(c != 3-i && outLen--)
            *(out++) = int2char(tempOut[--c]);

        i = 0;
        temp = 0;
    }
    while(c-- && outLen--)
        *(out++) = '=';
}

inline uint encodeLen(uint size)
{
    if(size%3)
        size += size%3+1;
    return (size*4)/3;
}

uint decodeLen(uint size)
{
    return (size*3)/4;
}

}
