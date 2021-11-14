#include "Arithmetic.h"
#include "Hist.h"
#include "Imgout.h"

void Arithmetic_Operation(UChar *Ori, Int wid, Int hei, Int min, Int max) //�ڵ� �ۼ� �ʿ�
{
	Img_Buf img;
	Int cur = 0; //���� ȭ�� ��ġ;

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

	ImageOutput(img.arithAdd, wid, hei, String[1]); // ��� ������ ����� ��� ����
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

	ImageOutput(img.arithSub, wid, hei, String[2]); // ��� ������ ����� ��� ����
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
			img.arithMul[cur] = (UChar)(Ori[cur] * gamma + 0.5); // ���� �� �ݿø�
		cur++;
	} while (cur < wid * hei);

	ImageOutput(img.arithMul, wid, hei, String[3]); // ��� ������ ����� ��� ����
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
			img.arithDiv[cur] = (UChar)(Ori[cur] / delta + 0.5); // ���� �� �ݿø�
		cur++;
	} while (cur < wid * hei);

	ImageOutput(img.arithDiv, wid, hei, String[4]); // ��� ������ ����� ��� ����
	HISTOGRAM(img.arithDiv, wid, hei, max, String[4]);  // histogram

	free(img.arithDiv);
}