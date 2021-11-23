#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include <time.h>
#define _CRT_SECURE_NO_WARNINGS

#define WIDTH      512              // 영상의 가로 크기
#define HEIGHT     512              // 영상의 세로 크기

#define maxVal    255
#define minVal    0

#define pi        3.141592653589793238462643383279 // cos, sin 계산할 때

#define CLIP(x) (x < minVal) ? minVal : x > maxVal ? maxVal : x

#define BLOCK_SIZE   512                                                     //(본인이 수정해야함) :: 8x8 block을 사용할 것이냐, 512x512 block을 사용할 것이냐
#if BLOCK_SIZE == 512
#define TransType 1 //1이면 현재 블록은 512x512, 0이면 그 이외의 블록
#if TransType
#define flagLPF   0 // flagLPF :: LPF 적용 유무, 1이면 적용, 0이면 미적용               //(본인이 수정해야함)
#if flagLPF
#define D0				32.0 // Cut off frequency (고정)
#define N0				4.0  // Filter dimension (고정)
#endif
#endif
#else
#define TransType 0 // 0 : 8x8 block, 1 : 512x512 block
#endif


typedef unsigned char UChar;
typedef char          Char;
typedef double		  Double;
typedef int           Int;


typedef struct _DFT_Buffer
{
	Double* picReal; // 실수부
	Double* picImag; // 허수부

	Double* picMagnitude; // 크기
	Double* picPhase; // 위상
}DFT_Val;

typedef struct _Image_Buffer
{
	UChar* ori; //원본 영상 저장을 위한 변수 선언
	UChar* rec; //복원 영상 저장

}Img_Buf;

void DFT_Process(Img_Buf* img, DFT_Val* DFT);
void DFT_Func(UChar* curBlk, Int blkSize, Int blkRow, Int blkCol, DFT_Val* DFT);

void IDFT_Process(Img_Buf* img, DFT_Val* DFT);
void IDFT_Func(Double* blkMag, Double* blkPha, Int blkSize, Int blkRow, Int blkCol, Img_Buf* img);

void LPF(Double* blkReal, Double* blkImag, Int wid, Int hei);
void PSNR(Img_Buf* img);