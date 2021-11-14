#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define _CRT_SECURE_NO_WARNINGS


typedef unsigned char UChar;
typedef char          Char;
typedef int           Int;

#define pixRange  256          // 흑백 영상 범위

#define LOW_Thres  30
#define HIGH_Thres 30

void HISTOGRAM(UChar* Data, Int wid, Int hei, Int max, Char String[]);
void HISTOGRAM_MODIFICATION(UChar* Data, Int Wid, Int Hei, Int max, Int min);