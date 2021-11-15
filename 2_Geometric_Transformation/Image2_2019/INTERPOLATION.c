#include "GEO.h"

UChar NearesetNeighbor(UChar* Data, Double srcX, Double srcY, Int Stride) // Stride : WIDTH
{
	// �ݿø��� ���ؼ� ���� ����� ȭ���� ��ġ�� ��ȯ
	return Data[((int)(srcY + 0.5) * Stride + (int)(srcX + 0.5))];
}

// Bilinear - �����ε� ��ġ�� �������� �ֺ� ���� ȭ�� 4���� �̿�
// (double)srcX, (double)srcY - �����ε� ȭ���� ��ġ
// (int)srcX, (int)srcY - �����ε� ��ġ�� ���� ���� �ִ� ȭ�� ��ġ (TL)
UChar Bilinear(UChar* Data, Double srcX, Double srcY, Int Stride)
{
	int SrcX_Plus1, SrcY_Plus1; // SrcX_Plus1 : srcX�� ������, SrcY_Plus1 : srcY�� �Ʒ�
	double Hor_Wei, Ver_Wei; //Horizontal Weight, Vertical Weight
	int TL, TR, BL, BR; //�� ȭ�� ��ġ

	// �����ε� ��ġ���� ���� ���� �ִ� ���� ȭ�� ��ġ�� ����
	SrcX_Plus1 = CLIPPIC_HOR(srcX+1); //
	SrcY_Plus1 = CLIPPIC_VER(srcY+1); //

	// ����ġ ����� �� ���̴� �Ÿ�
	Hor_Wei = srcX - (int)srcX;
	Ver_Wei = srcY - (int)srcY;

	// �� ȭ�� ��ġ�� ��Ȯ�ϰ� ���� ���� ���� �����ġ���� �˱� ���ؼ�
	// ���� SrcX, SrcY�� �ܼ��� �����ε� ��ġ���� ���� ����, ���� �Ʒ�, ������ ����, ������ �Ʒ��� �ִ� ȭ����
	TL = (int)srcY *Stride + (int)srcX; //
	TR = (int)srcY *Stride + SrcX_Plus1; //
	BL =  SrcY_Plus1 *Stride + (int)srcX; //
	BR =  SrcY_Plus1 *Stride + SrcX_Plus1; //

	// TMP�� ���� �� �ݿø��� �ؾ���
	UChar TMP =
		(1 - Ver_Wei) * (((1-Hor_Wei) *Data[TL]) + (Hor_Wei *Data[TR])) + /*����*/ 

		Ver_Wei * (((1-Hor_Wei) *Data[BL]) + (Hor_Wei *Data[BR])) + 0.5; /* �Ʒ���*/

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

	// ��ġ ȭ�� ��ǥ
	// ��� ȭ�Ҹ� �������� �ֺ� ȭ�Ҹ� ���� ������
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

	// TMP�� ȭ�Ұ� - clipping & �ݿø� ����
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

	// ��ġ ȭ�� ��ǥ
	// ��� ȭ�Ҹ� �������� �ֺ� ȭ�Ҹ� ���� ������
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

	// TMP�� ȭ�Ұ� - clipping & �ݿø� ����
	TMP += 0.5;
	TMP = TMP > maxVal ? maxVal : TMP < minVal ? minVal : TMP;

	return (UChar)TMP;
}