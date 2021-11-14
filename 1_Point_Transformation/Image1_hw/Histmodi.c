#include "Hist.h"
#include "Imgout.h"

// histogram equalization & histogram specification
void HISTOGRAM_MODIFICATION(UChar* Data, Int wid, Int hei, Int max, Int min)
{
	Char String[3][10] = { "Equal", "Spec", "End" };
	Int    LUT[pixRange] = { 0 };

	// step1. �� ȭ���� ������ count�� histogram ����
	for (Int i = 0; i < wid * hei; i++)
		LUT[Data[i]]++;

	/////////////////////////////////////////////////////////////////
	// 2) Histogram Equalization - ������ ������׷��� �����ϰ� ����� ���� (��� ��� �ִ�ȭ)
	// step 1. �� ȭ���� ������ count�� histogram ����(PDF)
	// step 2. ������ ��� (CDF)
	// step 3. Normalization

	Int    EQUAL_LUT[pixRange] = { 0 };
	Int    Accum_Sum[pixRange] = { 0 }; // ���� �󵵼�

	UChar* EQUAL_IMG = (UChar*)calloc((wid * hei), sizeof(UChar));

	// step2. ������ ���
	Accum_Sum[0] = LUT[0];
	for (Int i = 1; i < pixRange; i++)
		Accum_Sum[i] = Accum_Sum[i - 1] + LUT[i]; // �� ���� �󵵼� + ���� ȭ�Ұ�

	// step3. ����ȭ - ������׷� �ִ� ���̿� �°�
	// ����ȭ = (ȭ���� �ִ밪 / ��ü ȭ�� ��) * ���� ������ + 0.5(�ݿø�)
	for (Int i = 0; i < pixRange; i++)
		EQUAL_LUT[i] = ((double)max / ((double)wid * (double)hei)) * (double)Accum_Sum[i] + 0.5;

	for (Int i = 0; i < wid * hei; i++)
		EQUAL_IMG[i] = EQUAL_LUT[Data[i]];

	ImageOutput(EQUAL_IMG, wid, hei, String[0]);
	HISTOGRAM(EQUAL_IMG, wid, hei, max, String[0]);
	/////////////////////////////////////////////////////////////////


	/////////////////////////////////////////////////////////////////
	// 3) Histogram Specification - ���ϴ� ������׷����� ��ȯ(:: Histogram matching)
	// step1. histogram ����(PDF)
	// step2. histogram equalization & disired histogram
	// step3. disired histogram equaliztion
	// step4. (step3)�� historgram reverse equalization
	// step5. LUT�� �̿��Ͽ� specification

	Int    LUT_BUF[pixRange] = { 0 };
	Int    Speci_Accum_Sum[pixRange] = { 0 }; // ��ȭ�� ���� �󵵼�
	Int    SPECI_LUT[pixRange] = { 0 };
	Int    INV_EQUAL_LUT[pixRange] = { 0 };

	UChar* SPECI_IMG = (UChar*)calloc((wid * hei), sizeof(UChar));

	// step2. disired histogram(: 128~255) LUT �����
	for (Int i = pixRange / 2; i < pixRange; i++)
		LUT_BUF[i] = 2048;

	// step3. disired histogram equalization
	// -> ������(CDF)
	Speci_Accum_Sum[0] = LUT_BUF[0];
	for (Int i = 1; i < pixRange; i++)
		Speci_Accum_Sum[i] = Speci_Accum_Sum[i - 1] + LUT_BUF[i];
	// -> equalization
	for (Int i = 0; i < pixRange; i++)
		INV_EQUAL_LUT[i] = ((double)max / ((double)wid * (double)hei)) * (double)Speci_Accum_Sum[i] + 0.5;

	// step4. specification�� ���� LUT ����
	Int Val1, Val2;
	for (Int i = 0; i < pixRange - 1; i++)
	{
		Val1 = INV_EQUAL_LUT[i];
		Val2 = INV_EQUAL_LUT[i + 1];
		if (Val1 != Val2)
		{
			for (Int j = Val1; j < Val2; j++)
				SPECI_LUT[j] = i + 1;
		}
		else
			SPECI_LUT[Val1] = 0;

	}
	SPECI_LUT[pixRange - 1] = max;

	// step5. specification image(buffer)
	for (Int i = 0; i < wid * hei; i++)
		SPECI_IMG[i] = SPECI_LUT[EQUAL_IMG[i]];

	ImageOutput(SPECI_IMG, wid, hei, String[1]);
	HISTOGRAM(SPECI_IMG, wid, hei, max, String[1]);
	/////////////////////////////////////////////////////////////////


	/////////////////////////////////////////////////////////////////
	// 4) End-in search : ���� ���� �� ���, ��ο� ���� �� ��Ӱ� (�Ӱ谪 ���)

	UChar* End_IMG = (UChar*)calloc((wid * hei), sizeof(UChar));

	Int Low_num, High_num, Low_sum = 0, High_sum = 0;
	Int Low_Pos = 0, High_Pos = 255;

	////////////////////////////////////////////////////////////////
	// Low_Pos, High_Pos ���ϱ�
	// Low_Pos ���ϱ�
	Low_num = (wid * hei) * (LOW_Thres / 100.0);
	while (1)
	{
		Low_sum += LUT[Low_Pos];
		if (Low_sum > Low_num) break;
		else Low_Pos++;
	}

	// High_Pos ���ϱ�
	High_num = (wid * hei) * (HIGH_Thres / 100.0);
	while (1)
	{
		High_sum += LUT[High_Pos];
		if (High_sum > High_num) break;
		else High_Pos--;
	}

	////////////////////////////////////////////////////////////////

	for (int i = 0; i < wid * hei; i++)
	{
		if (Data[i] <= Low_Pos) End_IMG[i] = min; // Low �Ӱ谪���� ������ ȭ���� �ּҰ�����
		else if (Data[i] >= High_Pos) End_IMG[i] = max; // High �Ӱ谪���� ������ ȭ���� �ִ�����
		else // �߰����� stretching
		{
			double buf_out_pix;
			buf_out_pix = (((double)Data[i] - (double)Low_Pos) / ((double)High_Pos - (double)Low_Pos)) * 255.0 + 0.5;

			if (buf_out_pix < min)      End_IMG[i] = min;
			else if (buf_out_pix > max) End_IMG[i] = max;
			else                        End_IMG[i] = buf_out_pix;
		}
	}

	ImageOutput(End_IMG, wid, hei, String[2]);
	HISTOGRAM(End_IMG, wid, hei, max, String[2]);
	/////////////////////////////////////////////////////////////////

	free(EQUAL_IMG);
	free(SPECI_IMG);
	free(End_IMG);

}