#include "Filter.h"

UChar EmbossingFunc(UChar* Data, int maskSize)
{
	int Conv = 0;
	int Mask_Coeff[] = 
	{  -1, 0, 0,
		0, 0, 0,
		0, 0, 1 };

	for (int i = 0; i < maskSize * maskSize; i++)
		Conv += (Mask_Coeff[i] * (int)Data[i]);
	Conv = CLIP(Conv);

	return (UChar)CLIP(Conv + 128);
}

UChar SharpeningFunc(UChar* Data, int maskSize)
{
	int Conv = 0;
	int Mask_Coeff[] =
	{ -1, -1, -1,
		-1, 9, -1,
		-1, -1, -1 };

	for (int i = 0; i < maskSize * maskSize; i++)
		Conv += (Mask_Coeff[i] * (int)Data[i]);
	Conv = CLIP(Conv);

	return (UChar)CLIP(Conv);
}

UChar DoGFunc(UChar* Data, int maskSize)
{
	int Conv = 0;
	int Mask_Coeff[] =
	{ 0, 0, -1, -1, -1, 0, 0,
		0, -2, -3, -3, -3, -2, 0,
		-1, -3, 5, 5, 5, -3, -1,
		-1, -3, 5, 16, 5, -3, -1,
		-1, -3, 5, 5, 5, -3, -1,
		0, -2, -3, -3, -3, -2, 0,
		0, 0, -1, -1, -1, 0, 0 };

	for (int i = 0; i < maskSize * maskSize; i++)
		Conv += (Mask_Coeff[i] * (int)Data[i]);
	Conv = CLIP(Conv);

	return (UChar)Conv;
}

UChar LoGFunc(UChar* Data, int maskSize)
{
	int Conv = 0;
	int Mask_Coeff[] =
	{ 0, 0, -1, 0, 0,
		0, -1, -2, -1, 0,
		-1, -2, 16, -2, -1,
		0, -1, -2, -1, 0,
		 0, 0, -1, 0, 0 };

	for (int i = 0; i < maskSize * maskSize; i++)
		Conv += (Mask_Coeff[i] * (int)Data[i]);
	Conv = CLIP(Conv);

	return (UChar)Conv;
}

UChar SobelFunc(UChar* Data, int maskSize)
{
	int ConvVer = 0;
	int ConvHor = 0;
	int Mask_Coeff_Ver[] =
	{ 1, 0, -1,
	2, 0, -2,
	1, 0, -1 };

	int Mask_Coeff_Hor[] =
	{ -1, -2, -1,
	0, 0, 0,
	1, 2, 1 };

	// ����/������ ���ؼ� convolution
	for (int i = 0; i < maskSize * maskSize; i++) {
		ConvVer += (Mask_Coeff_Ver[i] * (int)Data[i]);
		ConvHor += (Mask_Coeff_Hor[i] * (int)Data[i]);
	}

	ConvVer = CLIP(ConvVer);
	ConvHor = CLIP(ConvHor);

	return (UChar)CLIP(ConvVer + ConvHor);
}

// 1�� ���翬���� 
UChar HomogeneityFunc(UChar* Data, int maskSize)
{
	int median_pixel = (int)Data[maskSize * maskSize / 2];
	int Conv = 0;
	int result = 0;
	// max(abs(���-�ֺ�)) - clipping 1
	for (int i = 0; i < maskSize * maskSize; i++) {
		Conv = abs(median_pixel - (int)Data[i]); // �� ���ϱ�
		result = max(result, Conv); // ���� ū �� result�� �ֱ�
	}
	result = CLIP(result);

	return (UChar)CLIP(result+60);
}

void OutputBlock(Img_Buf* img, UChar *paddingBlock, int maskSize, int i, int j, int PStride)
{
	for (int k = 0; k < maskSize; k++)
		for (int l = 0; l < maskSize; l++)
			paddingBlock[k * maskSize + l] = img->padding[(i + k) * PStride + (j + l)];
}

void ImageFiltering(UChar* Data, Img_Buf* img)
{
	Int wid = WIDTH; Int hei = HEIGHT;
	Int min = minVal; Int max = maxVal;

	Int picSize = wid * hei; //���� ������

	Int maskSize = 3;         //MxM size - ��κ� mask�� 3x3
	Int Add_size = (maskSize / 2) * 2;; // 3x3�̸� ���پ�, 5x5�� ���پ�, 7x7�̸� ���پ�
	UChar *paddingBlock;

	Int Stride  = wid;
	Int PStride = wid + Add_size; // padding�� ����

	Char String[8][10] = { "Embo", "Sharp", "Sobel", "Homogene", "Median", "DoG", "Blur", "LoG"};

	img->Embossing     = (UChar*)calloc(picSize, sizeof(UChar));
	img->Sharpening    = (UChar*)calloc(picSize, sizeof(UChar));
	img->homogeneity   = (UChar*)calloc(picSize, sizeof(UChar));

	paddingBlock = (UChar*)calloc(maskSize * maskSize, sizeof(UChar));
	ImagePadding(img, Data, wid, hei, maskSize); // ���� ���� padding - median �̳� blurring�� ���� �� �� ���� ������ input�̸� X
	for (int i = 0; i < hei; i++)
		for (int j = 0; j < wid; j++)
		{
			OutputBlock(img, paddingBlock, maskSize, i, j, PStride); // padding�� ���󿡼� mask size��ŭ ���� ��
			img->Embossing  [i * wid + j] = EmbossingFunc  (paddingBlock, maskSize);
			img->Sharpening [i * wid + j] = SharpeningFunc (paddingBlock, maskSize);
			img->homogeneity[i * wid + j] = HomogeneityFunc(paddingBlock, maskSize);
		}
	free(img->padding);
	free(paddingBlock);


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Sobel ���� ����� �κ�, �Է¿����� Embossing�� ����� ����
	// �ʱ�ȭ -> padding -> sobel 
	
	// ���� �ʱ�ȭ
	img->Sobel = (UChar*)calloc(picSize, sizeof(UChar));

	// padding
	paddingBlock = (UChar*)calloc(maskSize * maskSize, sizeof(UChar));
	ImagePadding(img, img->Embossing, wid, hei, maskSize); // Embossing�� ����� ����

	for (int i = 0; i < hei; i++)
		for (int j = 0; j < wid; j++)
		{
			OutputBlock(img, paddingBlock, maskSize, i, j, PStride); // padding�� ���󿡼� mask size��ŭ ���� ��
			img->Sobel[i * wid + j] = SobelFunc(paddingBlock, maskSize);
		}
	free(img->padding);
	free(paddingBlock);


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// DoG ���� ����� �κ�, �Է¿����� Sharpening�� ����� ����, Mask ũ��� 7x7

	maskSize = 7; //MxM size
	Add_size = (maskSize / 2) * 2;; // 3x3�̸� ���پ�, 5x5�� ���پ�, 7x7�̸� ���پ�

	Stride = wid;
	PStride = wid + Add_size; // padding�� ����

	// ���� �ʱ�ȭ
	img->DoG = (UChar*)calloc(picSize, sizeof(UChar));

	// padding
	paddingBlock = (UChar*)calloc(maskSize * maskSize, sizeof(UChar));
	ImagePadding(img, img->Sharpening, wid, hei, maskSize); // sharpening�� ����� ����

	for (int i = 0; i < hei; i++)
		for (int j = 0; j < wid; j++)
		{
			OutputBlock(img, paddingBlock, maskSize, i, j, PStride); // padding�� ���󿡼� mask size��ŭ ���� ��
			img->DoG[i * wid + j] = DoGFunc(paddingBlock, maskSize);
		}
	free(img->padding);
	free(paddingBlock);





	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// LoG ���� ����� �κ�, �Է¿����� �Ϲ� lena, Mask ũ��� 7x7

	maskSize = 5; //MxM size - 5x5
	Add_size = (maskSize / 2) * 2;; // 3x3�̸� ���پ�, 5x5�� ���پ�, 7x7�̸� ���پ�

	Stride = wid;
	PStride = wid + Add_size; // padding�� ����

	// ���� �ʱ�ȭ
	img->LoG = (UChar*)calloc(picSize, sizeof(UChar));

	// padding
	paddingBlock = (UChar*)calloc(maskSize * maskSize, sizeof(UChar));
	ImagePadding(img, Data, wid, hei, maskSize); // ���� ����

	for (int i = 0; i < hei; i++)
		for (int j = 0; j < wid; j++)
		{
			OutputBlock(img, paddingBlock, maskSize, i, j, PStride); // padding�� ���󿡼� mask size��ŭ ���� ��
			img->LoG[i * wid + j] = LoGFunc(paddingBlock, maskSize);
		}
	free(img->padding);
	free(paddingBlock);





	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	ImageOutput(img->Embossing,   wid, hei, String[0]);
	ImageOutput(img->Sharpening,  wid, hei, String[1]);
	ImageOutput(img->Sobel,       wid, hei, String[2]);
	ImageOutput(img->homogeneity, wid, hei, String[3]);
	ImageOutput(img->DoG,         wid, hei, String[5]);
	ImageOutput(img->LoG,		  wid, hei, String[7]);
	
	free(img->Embossing);
	free(img->Sharpening);
	free(img->Sobel);
	free(img->homogeneity);
	free(img->DoG);
	free(img->LoG);
}