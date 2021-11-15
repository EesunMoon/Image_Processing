#include "main.h"

// 역추적 mapping을 했을 때 화소가 영상의 범위를 벗어나는 경우에 가장 가까운 화소로 mapping 해 줌
#define CLIPPIC_HOR(x) (x < 0) ? 0 : x >= WIDTH  ? WIDTH  - 1 : x
#define CLIPPIC_VER(x) (x < 0) ? 0 : x >= HEIGHT ? HEIGHT - 1 : x

// 회전할 때 쓰는 변수
#define PI			  3.141592653589793238462643383279

// scale 할 때의 변수
typedef struct _Scale_Buffer
{
	UChar* Near; // nearest neighbor interpolation
	UChar* Bi; // bilinear interpolation
	UChar* BS; // B-Spline interpolation
	UChar* Cu; // Cubic interpolation
}SCALE;

// 회전 할 때의 변수
typedef struct _Rotation_Buffer
{
	UChar* Near; // nearest neighbor interpolation
	UChar* Bi; // bilinear interpolation
	UChar* BS; // B-Spline interpolation
	UChar* Cu; // Cubic interpolation
}ROTATION;

// 축소할 때 blurring을 위함
typedef struct _Image_Buffer
{
	UChar* padding; // 패딩 영상 저장 버퍼
	UChar* Result_Blurring;         // 축소 영상을 위한 블러링 결과 저장 

}Img_Buf;

void Geometric_Transformation(UChar* Data, Img_Buf* img);


void ImageOutput(UChar* Data, Int wid, Int hei, Char String[]);

UChar NearesetNeighbor(UChar* Data, Double srcX, Double srcY, Int Stride);
UChar Bilinear(UChar* Data, Double srcX, Double srcY, Int Stride);
UChar B_Spline(UChar* Data, Double srcX, Double srcY, Int Stride);
UChar Cubic(UChar* Data, Double srcX, Double srcY, Int Stride);