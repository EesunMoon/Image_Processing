#include "GEO.h"

UChar NearesetNeighbor(UChar* Data, Double srcX, Double srcY, Int Stride) // Stride : WIDTH
{
	// 반올림을 통해서 가장 가까운 화소의 위치를 반환
	return Data[((int)(srcY + 0.5) * Stride + (int)(srcX + 0.5))];
}

// Bilinear - 역매핑된 위치를 기준으로 주변 정수 화서 4개를 이용
// (double)srcX, (double)srcY - 역매핑된 화소의 위치
// (int)srcX, (int)srcY - 역매핑된 위치의 왼쪽 위에 있는 화소 위치 (TL)
UChar Bilinear(UChar* Data, Double srcX, Double srcY, Int Stride)
{
	int SrcX_Plus1, SrcY_Plus1; // SrcX_Plus1 : srcX의 오른쪽, SrcY_Plus1 : srcY의 아래
	double Hor_Wei, Ver_Wei; //Horizontal Weight, Vertical Weight
	int TL, TR, BL, BR; //각 화소 위치

	// 역매핑된 위치에서 왼쪽 위에 있는 정수 화소 위치가 기준
	SrcX_Plus1 = CLIPPIC_HOR(srcX+1); //
	SrcY_Plus1 = CLIPPIC_VER(srcY+1); //

	// 가중치 계산할 때 쓰이는 거리
	Hor_Wei = srcX - (int)srcX;
	Ver_Wei = srcY - (int)srcY;

	// 각 화소 위치를 정확하게 원본 영상 내의 어느위치인지 알기 위해서
	// 현재 SrcX, SrcY는 단순히 역매핑된 위치에서 왼쪽 위에, 왼쪽 아래, 오른쪽 위에, 오른쪽 아래에 있는 화소임
	TL = (int)srcY *Stride + (int)srcX; //
	TR = (int)srcY *Stride + SrcX_Plus1; //
	BL =  SrcY_Plus1 *Stride + (int)srcX; //
	BR =  SrcY_Plus1 *Stride + SrcX_Plus1; //

	// TMP를 구할 때 반올림을 해야함
	UChar TMP =
		(1 - Ver_Wei) * (((1-Hor_Wei) *Data[TL]) + (Hor_Wei *Data[TR])) + /*위쪽*/ 

		Ver_Wei * (((1-Hor_Wei) *Data[BL]) + (Hor_Wei *Data[BR])) + 0.5; /* 아래쪽*/

	return TMP;
}

// B-Spline
double BSpline_function(Double x) {

	double result;

	if (fabs(x) >= 0 && fabs(x) < 1) {
		result = (1.0/2.0 * (pow(fabs(x), 3.0))) + (-1.0 * pow(fabs(x), 2.0)) + (2.0/3.0);
	}
	else if (fabs(x) >= 1 && fabs(x) < 2) {
		result = (-1.0 * 1.0/6.0 * pow(fabs(x), 3.0)) + pow(fabs(x), 2.0) + (-1.0*2.0*fabs(x)) + 4.0/3.0;
	}
	else
		result = 0;

	return result;
}

UChar B_Spline(UChar* Data, Double srcX, Double srcY, Int Stride)
{
	int Src_X_Minus_1, Src_X_Plus_1, Src_X_Plus_2;
	int Src_Y_Minus_1, Src_Y_Plus_1, Src_Y_Plus_2;
	double Hor_Wei, Ver_Wei; //Horizontal Weight, Vertical Weight
	double TMP_Hor[4] = { 0,0,0,0 };
	double TMP = 0;

	// 위치 화소 좌표
	// 어느 화소를 기준으로 주변 화소를 구할 것인지
	Src_X_Plus_1 = CLIPPIC_HOR((int)srcX+1);
	Src_X_Plus_2 = CLIPPIC_HOR((int)srcX+2); 
	Src_Y_Plus_1 = CLIPPIC_VER((int)srcY+1);
	Src_Y_Plus_2 = CLIPPIC_VER((int)srcY+2);

	Src_X_Minus_1 = CLIPPIC_HOR((int)srcX-1);
	Src_Y_Minus_1 = CLIPPIC_VER((int)srcY-1);

	Hor_Wei = srcX - (int)srcX;
	Ver_Wei = srcY - (int)srcY;


	int X_Pix[] = { Src_X_Minus_1, (int)srcX, Src_X_Plus_1, Src_X_Plus_2 };
	int Y_Pix[] = { Src_Y_Minus_1, (int)srcY, Src_Y_Plus_1, Src_Y_Plus_2 };
	double Distance_Hor[] = { Hor_Wei + 1, Hor_Wei, 1 - Hor_Wei, (1 - Hor_Wei) + 1 };
	double Distance_Ver[] = { Ver_Wei + 1, Ver_Wei, 1 - Ver_Wei, (1 - Ver_Wei) + 1 };

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			TMP_Hor[i] += BSpline_function(Distance_Hor[j]) * Data[Y_Pix[i] * Stride + X_Pix[j]];
		}
	}

	for (int i = 0; i < 4; i++)
	{
		TMP += BSpline_function(Distance_Ver[i]) * TMP_Hor[i];
	}

	// TMP는 화소값 - clipping & 반올림 과정
	TMP += 0.5;
	TMP = TMP > maxVal ? maxVal : TMP < minVal ? minVal : TMP;

	return (UChar)TMP;
}

double Cubic_function(Double x) {

	double result;
	double a = 0.5;

	if (fabs(x) >= 0 && fabs(x) < 1) {
		result = ((a+2)*(pow(fabs(x), 3.0))) +(-1.0*(a+3)*pow(fabs(x), 2.0)) + 1.0;
	}
	else if (fabs(x) >= 1 && fabs(x) < 2) {
		result = (a*pow(fabs(x), 3.0)) + (-1.0*5.0*a*pow(fabs(x), 2.0)) + 8.0*a*fabs(x) - 4.0*a;
	}
	else
		result = 0;

	return result;
}

UChar Cubic(UChar* Data, Double srcX, Double srcY, Int Stride)
{
	int Src_X_Minus_1, Src_X_Plus_1, Src_X_Plus_2;
	int Src_Y_Minus_1, Src_Y_Plus_1, Src_Y_Plus_2;
	double Hor_Wei, Ver_Wei; //Horizontal Weight, Vertical Weight
	double TMP_Hor[4] = { 0,0,0,0 };
	double TMP = 0;

	// 위치 화소 좌표
	// 어느 화소를 기준으로 주변 화소를 구할 것인지
	Src_X_Plus_1 = CLIPPIC_HOR((int)srcX+1);
	Src_X_Plus_2 = CLIPPIC_HOR((int)srcX+2);
	Src_Y_Plus_1 = CLIPPIC_VER((int)srcY+1);
	Src_Y_Plus_2 = CLIPPIC_VER((int)srcY+2);

	Src_X_Minus_1 = CLIPPIC_HOR((int)srcX-1);
	Src_Y_Minus_1 = CLIPPIC_VER((int)srcY-1);

	Hor_Wei = srcX - (int)srcX;
	Ver_Wei = srcY - (int)srcY;


	int X_Pix[] = { Src_X_Minus_1, (int)srcX, Src_X_Plus_1, Src_X_Plus_2 };
	int Y_Pix[] = { Src_Y_Minus_1, (int)srcY, Src_Y_Plus_1, Src_Y_Plus_2 };
	double Distance_Hor[] = { Hor_Wei + 1, Hor_Wei, 1 - Hor_Wei, (1 - Hor_Wei) + 1 };
	double Distance_Ver[] = { Ver_Wei + 1, Ver_Wei, 1 - Ver_Wei, (1 - Ver_Wei) + 1 };

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			TMP_Hor[i] += Cubic_function(Distance_Hor[j]) * Data[Y_Pix[i] * Stride + X_Pix[j]];
		}
	}

	for (int i = 0; i < 4; i++)
	{
		TMP += Cubic_function(Distance_Ver[i]) * TMP_Hor[i];
	}

	// TMP는 화소값 - clipping & 반올림 과정
	TMP += 0.5;
	TMP = TMP > maxVal ? maxVal : TMP < minVal ? minVal : TMP;

	return (UChar)TMP;
}