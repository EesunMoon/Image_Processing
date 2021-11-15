#include "main.h"

#define CLIP(x) (x < minVal) ? minVal : x > maxVal ? maxVal : x // 영상의 화소값의 범위를 넘어가는지 알려주는 clipping
#define CLIPPIC_HOR(x) (x < 0) ? 0 : x >= WIDTH  ? WIDTH  - 1 : x // 화소가 영상 밖에 있는지 알려주는 clipping
#define CLIPPIC_VER(x) (x < 0) ? 0 : x >= HEIGHT ? HEIGHT - 1 : x // 화소가 영상 밖에 있는지 알려주는 clipping

#define MAX(x, y) x > y ? x : y
#define SWAP(x, y) {int tmp; tmp = x; x = y; y = tmp;}

// 결과영상
typedef struct _Image_Buffer
{
	UChar* padding; // 패딩 영상 저장 버퍼

	UChar* Embossing;
	UChar* Sharpening;
	UChar* DoG;
	UChar* LoG; // LoG - operator 새로 만들기
	UChar* Median;
	UChar* Sobel;
	UChar* Blur;
	UChar* homogeneity;

}Img_Buf;

void ImageFiltering(UChar* Data, Img_Buf* img);
void ImageFilteringBlur(UChar* Data, Img_Buf* img);
void ImagePadding(Img_Buf* img, UChar* Buf, int width, int height, int maskSize);
void ImageOutput(UChar* Data, Int wid, Int hei, Char String[]);

void OutputBlock(Img_Buf* img, UChar *paddingBlock, int maskSize, int i, int j, int PStride);