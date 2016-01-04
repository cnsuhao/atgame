#pragma once

#if defined(_MSC_VER)
    typedef __int8     int8;
    typedef __int16    int16;
    typedef __int32    int32;
    typedef __int64    int64;

    typedef unsigned __int8     uint8;
    typedef unsigned __int16    uint16;
    typedef unsigned __int32    uint32;
    typedef unsigned __int64    uint64;
#else
    typedef char       int8;
    typedef short      int16;
    typedef int        int32;
    typedef long long  int64;
    
    typedef unsigned char       uint8;
    typedef unsigned short      uint16;
    typedef unsigned int        uint32;
    typedef unsigned long long  uint64; 
#endif
