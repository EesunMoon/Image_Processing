#include "GEO.h"

void main()
{
	FILE *fp;
	UChar *ori; //원본 영상 화소값들을 저장하기 위한 버퍼
	Img_Buf image; //블러링 용도


	Int wid = WIDTH; Int hei = HEIGHT;
	Int min = minVal; Int max = maxVal;

	fopen_s(&fp, "lena_512x512.raw", "rb"); //원본 영상 열기

	ori = (UChar*)malloc(sizeof(UChar) * (wid * hei)); //원본 영상 크기만큼 공간 선언
	memset(ori, 0, sizeof(UChar) * (wid * hei)); //0으로 초기화
	fread(ori, sizeof(UChar), (wid * hei), fp); // 원본 영상 읽기(원본 영상의 픽셀 값을 배열 변수에 저장)

	Geometric_Transformation(ori, &image);

	free(ori);
	fclose(fp);
}