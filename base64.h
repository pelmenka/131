#ifndef BASE64_H_INCLUDED
#define BASE64_H_INCLUDED

#include "common.h"

namespace base64
{
    uint encode(const uchar*, uint,  char*, uint);
    uint decode(const  char*, uint, uchar*, uint);

    uint encodeLen(uint);
    uint decodeLen(uint);
}

#endif // BASE64_H_INCLUDED
