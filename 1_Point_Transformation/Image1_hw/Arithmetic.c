#include "Arithmetic.h"
#include "Hist.h"
#include "Imgout.h"

void Arithmetic_Operation(UChar *Ori, Int wid, Int hei, Int min, Int max) //코드 작성 필요
{
	Img_Buf img;
	Int cur = 0; //현재 화소 위치;

	Char String[5][10] = { "Ori", "Add" , "Sub" ,"Mul" ,"Div" };


	// 1-1) Add
	img.arithAdd = (UChar*)calloc((wid * hei), sizeof(UChar));

	do
	{
		// Add & clipping
		if (Ori[cur] + alpha > max)
			img.arithAdd[cur] = max;
		else
			img.arithAdd[cur] = (UChar)(Ori[cur] + alpha);
		cur++;
	} while (cur < wid * hei);

	ImageOutput(img.arithAdd, wid, hei, String[1]); // 산술 연산이 저장된 결과 영상
	HISTOGRAM(img.arithAdd, wid, hei, max, String[1]);  // histogram

	free(img.arithAdd);


	// 1-2) Sub
	img.arithSub = (UChar*)calloc((wid * hei), sizeof(UChar));
	cur = 0;
	do
	{
		// Sub & clipping
		if (Ori[cur] - beta < min)
			img.arithSub[cur] = min;
		else
			img.arithSub[cur] = (UChar)(Ori[cur] - beta);
		cur++;
	} while (cur < wid * hei);

	ImageOutput(img.arithSub, wid, hei, String[2]); // 산술 연산이 저장된 결과 영상
	HISTOGRAM(img.arithSub, wid, hei, max, String[2]);  // histogram

	free(img.arithSub);


	// 1-3) Mul
	img.arithMul = (UChar*)calloc((wid * hei), sizeof(UChar));
	cur = 0;
	do
	{
		// Mul & clipping
		if (Ori[cur] * gamma + 0.5 > max)
			img.arithMul[cur] = max;
		else
			img.arithMul[cur] = (UChar)(Ori[cur] * gamma + 0.5); // 곱한 후 반올림
		cur++;
	} while (cur < wid * hei);

	ImageOutput(img.arithMul, wid, hei, String[3]); // 산술 연산이 저장된 결과 영상
	HISTOGRAM(img.arithMul, wid, hei, max, String[3]);  // histogram

	free(img.arithMul);


	// 1-4) Div
	img.arithDiv = (UChar*)calloc((wid * hei), sizeof(UChar));
	cur = 0;
	do
	{
		// Div & clipping
		if (Ori[cur] / delta + 0.5 < min)
			img.arithDiv[cur] = min;
		else
			img.arithDiv[cur] = (UChar)(Ori[cur] / delta + 0.5); // 나눈 후 반올림
		cur++;
	} while (cur < wid * hei);

	ImageOutput(img.arithDiv, wid, hei, String[4]); // 산술 연산이 저장된 결과 영상
	HISTOGRAM(img.arithDiv, wid, hei, max, String[4]);  // histogram

	free(img.arithDiv);
}