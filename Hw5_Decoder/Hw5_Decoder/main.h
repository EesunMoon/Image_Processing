#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include <time.h>
#define _CRT_SECURE_NO_WARNINGS

#define WIDTH      832              // 영상의 가로 크기
#define HEIGHT     480              // 영상의 세로 크기
#define FRAME	   100

#define maxVal    255
#define minVal    0

#define pi        3.141592653589793238462643383279

#define CLIP(x) (x < minVal) ? minVal : x > maxVal ? maxVal : x

#define BLOCK_SIZE   8                                                     

typedef char			   Char;
typedef int                Int;
typedef double			   Double;
typedef unsigned int       UInt;
typedef unsigned char      UChar;
typedef unsigned char      Pel;

enum PredictionMode
{
	VERTICAL,
	HORIZONTAL,
	DIAGONAL,
	DC,
	modeNum
};

typedef struct _recBuffer
{
	UChar refPix[2][BLOCK_SIZE + 1];
}refBuf;

typedef struct _Image_information
{
	Int Best_Mode;
}Img_Info;

typedef struct _Image_Buffer
{
	Int* Quant_blk;

	Int Huff_codes[16];
	Int Huff_freq[16];
	Int Huff_ch[16];
	Int Huff_length[16];
	Int Huff_Size;

	FILE* Input_Bitstream;
	Img_Info info;
}Img_Buf;

Decoding_Process(UChar* recLuma, Int m_picWidthInCtu, Int m_picHeightInCtu, Img_Buf* img);
PredictionFunc(UChar* predBlk, refBuf ref, Int blkSize, Int blkStride, UInt mode);
InvTransQuantFunc(Int* quantBlk, Int Blk, Int* invResiBlk);
Entropy(Int BLK, Img_Buf* img);