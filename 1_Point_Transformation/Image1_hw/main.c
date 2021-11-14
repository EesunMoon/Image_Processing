// 헤더 파일
#include "main.h" // 상위 헤더
#include "Arithmetic.h"
#include "Hist.h"
#include "Imgout.h"


void main()
{
	UChar *ori; //원본 영상 화소값들을 저장하기 위한 버퍼
	FILE *fp;

	Int wid = WIDTH; Int hei = HEIGHT;
	Int min = minVal; Int max = maxVal;

	Char String[5][10] = { "Ori", "Add" , "Sub" ,"Mul" ,"Div" };

	fopen_s(&fp, "lena_512x512.raw", "rb"); //원본 영상 열기

	ori = (UChar*)malloc(sizeof(UChar) * (WIDTH * HEIGHT)); // 원본 영상 크기 동적 할당
	memset(ori, 0, sizeof(UChar) * (WIDTH * HEIGHT)); // 0으로 초기화
	fread(ori, sizeof(UChar), (WIDTH * HEIGHT), fp); // 원본 영상 읽기 (원본 영상의 픽셀 값을 배열 변수에 저장)

	// Arithmetic_Operation(ori, wid, hei, min, max);
	HISTOGRAM_MODIFICATION(ori, wid, hei, max, min);
	// HISTOGRAM(ori, wid, hei, max, String[0]); // 히스토그램 출력 함수

	free(ori);
	fclose(fp);
}