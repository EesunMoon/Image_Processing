#include "Hist.h"

// Data : 원본 영상
// Histogram 정규화
void HISTOGRAM(UChar* Data, Int wid, Int hei, Int max, Char String[])
{
	FILE* fp;

	int    LUT[pixRange] = { 0 };
	double Normal_LUT[pixRange] = { 0 };
	UChar Output[pixRange][pixRange] = { 0 };

	char Name_Hist[50] = "Hist_";
	char Name_extension[10] = ".raw";

	int max_cnt = 0;

	// 0~255 빈도수 세기
	for (int i = 0; i < wid * hei; i++)
		LUT[Data[i]]++;

	// 빈도수가 가장 높은 화소 찾기
	for (int i = 0; i < pixRange; i++)
		max_cnt = max_cnt < LUT[i] ? LUT[i] : max_cnt;

	// 빈도수 가장 높은 화소를 최대 높이로 지정
	for (int i = 0; i < pixRange; i++)
		Normal_LUT[i] = (double)LUT[i] / (double)max_cnt * 255.0;

	// 막대그래프를 그리기 위함 : 영상의 graph가 y축으로 갈수록 -가 되기 때문에 j--로 접근
	for (int i = 0; i < pixRange; i++)
	{
		for (int j = max; j > max - Normal_LUT[i]; j--)
		{
			Output[j][i] = 255;
		}
	}

	strcat_s(Name_Hist, 20,String);
	strcat_s(Name_Hist, 20,Name_extension);

	fopen_s(&fp, Name_Hist, "wb");
	fwrite(Output, sizeof(UChar), pixRange * pixRange, fp);
	fclose(fp);
}