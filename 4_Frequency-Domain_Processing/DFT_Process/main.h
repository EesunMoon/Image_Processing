#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include <time.h>
#define _CRT_SECURE_NO_WARNINGS

#define WIDTH      512              // ������ ���� ũ��
#define HEIGHT     512              // ������ ���� ũ��

#define maxVal    255
#define minVal    0

#define pi        3.141592653589793238462643383279 // cos, sin ����� ��

#define CLIP(x) (x < minVal) ? minVal : x > maxVal ? maxVal : x

#define BLOCK_SIZE   512                                                     //(������ �����ؾ���) :: 8x8 block�� ����� ���̳�, 512x512 block�� ����� ���̳�
#if BLOCK_SIZE == 512
#define TransType 1 //1�̸� ���� ����� 512x512, 0�̸� �� �̿��� ���
#if TransType
#define flagLPF   0 // flagLPF :: LPF ���� ����, 1�̸� ����, 0�̸� ������               //(������ �����ؾ���)
#if flagLPF
#define D0				32.0 // Cut off frequency (����)
#define N0				4.0  // Filter dimension (����)
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
	Double* picReal; // �Ǽ���
	Double* picImag; // �����

	Double* picMagnitude; // ũ��
	Double* picPhase; // ����
}DFT_Val;

typedef struct _Image_Buffer
{
	UChar* ori; //���� ���� ������ ���� ���� ����
	UChar* rec; //���� ���� ����

}Img_Buf;

void DFT_Process(Img_Buf* img, DFT_Val* DFT);
void DFT_Func(UChar* curBlk, Int blkSize, Int blkRow, Int blkCol, DFT_Val* DFT);

void IDFT_Process(Img_Buf* img, DFT_Val* DFT);
void IDFT_Func(Double* blkMag, Double* blkPha, Int blkSize, Int blkRow, Int blkCol, Img_Buf* img);

void LPF(Double* blkReal, Double* blkImag, Int wid, Int hei);
void PSNR(Img_Buf* img);