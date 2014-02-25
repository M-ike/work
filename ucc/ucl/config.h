#ifndef __CONFIG_H_
#define __CONFIG_H_

#undef  LITTLE_ENDIAN
#define LITTLE_ENDIAN     1

#define CHAR_SIZE         1
#define SHORT_SIZE        2
#define INT_SIZE          4
#define LONG_SIZE         4
#define LONG_LONG_SIZE    4
#define FLOAT_SIZE        4
#define DOUBLE_SIZE       8
#define LONG_DOUBLE_SIZE  8

#ifdef _WIN32
#define	WCHAR		USHORT
#else
#define WCHAR		ULONG
#endif

#endif
