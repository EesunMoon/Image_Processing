#include "Hist.h"
#include "Imgout.h"

// histogram equalization & histogram specification
void HISTOGRAM_MODIFICATION(UChar* Data, Int wid, Int hei, Int max, Int min)
{
	Char String[3][10] = { "Equal", "Spec", "End" };
	Int    LUT[pixRange] = { 0 };

	// step1. 각 화소의 개수를 count한 histogram 생성
	for (Int i = 0; i < wid * hei; i++)
		LUT[Data[i]]++;

	/////////////////////////////////////////////////////////////////
	// 2) Histogram Equalization - 영상의 히스토그램을 균일하게 만드는 과정 (명암 대비를 최대화)
	// step 1. 각 화소의 개수를 count한 histogram 생성(PDF)
	// step 2. 누적합 계산 (CDF)
	// step 3. Normalization

	Int    EQUAL_LUT[pixRange] = { 0 };
	Int    Accum_Sum[pixRange] = { 0 }; // 누적 빈도수

	UChar* EQUAL_IMG = (UChar*)calloc((wid * hei), sizeof(UChar));

	// step2. 누적합 계산
	Accum_Sum[0] = LUT[0];
	for (Int i = 1; i < pixRange; i++)
		Accum_Sum[i] = Accum_Sum[i - 1] + LUT[i]; // 전 누적 빈도수 + 현재 화소값

	// step3. 정규화 - 히스토그램 최대 높이에 맞게
	// 정규화 = (화소의 최대값 / 전체 화소 수) * 현재 누적값 + 0.5(반올림)
	for (Int i = 0; i < pixRange; i++)
		EQUAL_LUT[i] = ((double)max / ((double)wid * (double)hei)) * (double)Accum_Sum[i] + 0.5;

	for (Int i = 0; i < wid * hei; i++)
		EQUAL_IMG[i] = EQUAL_LUT[Data[i]];

	ImageOutput(EQUAL_IMG, wid, hei, String[0]);
	HISTOGRAM(EQUAL_IMG, wid, hei, max, String[0]);
	/////////////////////////////////////////////////////////////////


	/////////////////////////////////////////////////////////////////
	// 3) Histogram Specification - 원하는 히스토그램으로 변환(:: Histogram matching)
	// step1. histogram 생성(PDF)
	// step2. histogram equalization & disired histogram
	// step3. disired histogram equaliztion
	// step4. (step3)의 historgram reverse equalization
	// step5. LUT을 이용하여 specification

	Int    LUT_BUF[pixRange] = { 0 };
	Int    Speci_Accum_Sum[pixRange] = { 0 }; // 명세화된 누적 빈도수
	Int    SPECI_LUT[pixRange] = { 0 };
	Int    INV_EQUAL_LUT[pixRange] = { 0 };

	UChar* SPECI_IMG = (UChar*)calloc((wid * hei), sizeof(UChar));

	// step2. disired histogram(: 128~255) LUT 만들기
	for (Int i = pixRange / 2; i < pixRange; i++)
		LUT_BUF[i] = 2048;

	// step3. disired histogram equalization
	// -> 누적값(CDF)
	Speci_Accum_Sum[0] = LUT_BUF[0];
	for (Int i = 1; i < pixRange; i++)
		Speci_Accum_Sum[i] = Speci_Accum_Sum[i - 1] + LUT_BUF[i];
	// -> equalization
	for (Int i = 0; i < pixRange; i++)
		INV_EQUAL_LUT[i] = ((double)max / ((double)wid * (double)hei)) * (double)Speci_Accum_Sum[i] + 0.5;

	// step4. specification을 위한 LUT 생성
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
	// 4) End-in search : 밝은 곳은 더 밝게, 어두운 곳은 더 어둡게 (임계값 사용)

	UChar* End_IMG = (UChar*)calloc((wid * hei), sizeof(UChar));

	Int Low_num, High_num, Low_sum = 0, High_sum = 0;
	Int Low_Pos = 0, High_Pos = 255;

	////////////////////////////////////////////////////////////////
	// Low_Pos, High_Pos 구하기
	// Low_Pos 구하기
	Low_num = (wid * hei) * (LOW_Thres / 100.0);
	while (1)
	{
		Low_sum += LUT[Low_Pos];
		if (Low_sum > Low_num) break;
		else Low_Pos++;
	}

	// High_Pos 구하기
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
		if (Data[i] <= Low_Pos) End_IMG[i] = min; // Low 임계값보다 낮으면 화소의 최소값으로
		else if (Data[i] >= High_Pos) End_IMG[i] = max; // High 임계값보다 높으면 화소의 최댓값으로
		else // 중간값은 stretching
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