#include "main.h"

// ������ mapping�� ���� �� ȭ�Ұ� ������ ������ ����� ��쿡 ���� ����� ȭ�ҷ� mapping �� ��
#define CLIPPIC_HOR(x) (x < 0) ? 0 : x >= WIDTH  ? WIDTH  - 1 : x
#define CLIPPIC_VER(x) (x < 0) ? 0 : x >= HEIGHT ? HEIGHT - 1 : x

// ȸ���� �� ���� ����
#define PI			  3.141592653589793238462643383279

// scale �� ���� ����
typedef struct _Scale_Buffer
{
	UChar* Near; // nearest neighbor interpolation
	UChar* Bi; // bilinear interpolation
	UChar* BS; // B-Spline interpolation
	UChar* Cu; // Cubic interpolation
}SCALE;

// ȸ�� �� ���� ����
typedef struct _Rotation_Buffer
{
	UChar* Near; // nearest neighbor interpolation
	UChar* Bi; // bilinear interpolation
	UChar* BS; // B-Spline interpolation
	UChar* Cu; // Cubic interpolation
}ROTATION;

// ����� �� blurring�� ����
typedef struct _Image_Buffer
{
	UChar* padding; // �е� ���� ���� ����
	UChar* Result_Blurring;         // ��� ������ ���� ���� ��� ���� 

}Img_Buf;

void Geometric_Transformation(UChar* Data, Img_Buf* img);


void ImageOutput(UChar* Data, Int wid, Int hei, Char String[]);

UChar NearesetNeighbor(UChar* Data, Double srcX, Double srcY, Int Stride);
UChar Bilinear(UChar* Data, Double srcX, Double srcY, Int Stride);
UChar B_Spline(UChar* Data, Double srcX, Double srcY, Int Stride);
UChar Cubic(UChar* Data, Double srcX, Double srcY, Int Stride);