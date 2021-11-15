#include "Filter.h"

UChar BlurFunc(UChar* Data, int maskSize)
{
	double Conv = 0;
	double Mask_Coeff[] =
	{ 1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0,
		1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0,
		1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0 };

	for (int i = 0; i < maskSize * maskSize; i++)
		Conv += (Mask_Coeff[i] * (double)Data[i]);

	return (UChar)CLIP(Conv);
}

UChar MedianFunc(UChar* Data, int maskSize)
{
	// �������� or �������� 
	for (int i = 0; i < maskSize * maskSize -1; i++) {
		for (int j = i+1; j < maskSize * maskSize ; j++) {
			if (Data[i] > Data[j])
				SWAP(Data[i], Data[j]);
		}
	}

	// ��� ��
	return Data[maskSize * maskSize / 2];
}

void ImageFilteringBlur(UChar* Data, Img_Buf* img)
{
	Int wid = WIDTH; Int hei = HEIGHT;
	Int min = minVal; Int max = maxVal;

	Int picSize = wid * hei; //���� ������

	Int maskSize = 3;         //MxM size
	Int Add_size = (maskSize / 2) * 2;;
	UChar *paddingBlock;

	Int Stride = wid;
	Int PStride = wid + Add_size;

	Char String[8][10] = { "Embo", "Sharp", "Sobel", "Homogene", "Median", "DoG", "Blur", "LoG"};

	img->Blur = (UChar*)calloc(picSize, sizeof(UChar));
	// blurring 6�� ����
	for (int cnt = 0; cnt < 6; cnt++)
	{
		paddingBlock = (UChar*)calloc(maskSize * maskSize, sizeof(UChar));

		if (cnt == 0)
			ImagePadding(img, Data, wid, hei, maskSize); // ù��° - �������� padding
		else
			ImagePadding(img, img->Blur, wid, hei, maskSize); // ������ - ���� padding ������ padding

		for (int i = 0; i < hei; i++)
			for (int j = 0; j < wid; j++)
			{
				OutputBlock(img, paddingBlock, maskSize, i, j, PStride);
				img->Blur[i * wid + j] = BlurFunc(paddingBlock, maskSize);
			}
		free(img->padding);
		free(paddingBlock);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Median ���� ����� �κ�(10ȸ ����), ó�� �Է¿����� ���� ��ũ��ġ ����, 2ȸ���ʹ� Median�� ����� ����

	img->Median = (UChar*)calloc(picSize, sizeof(UChar));
	// median 10�� ����
	for (int cnt = 0; cnt <10; cnt++)
	{
		paddingBlock = (UChar*)calloc(maskSize * maskSize, sizeof(UChar));

		if (cnt == 0)
			ImagePadding(img, Data, wid, hei, maskSize); // ù��° - �������� padding
		else
			ImagePadding(img, img->Median, wid, hei, maskSize); // ������ - ���� padding ������ padding

		for (int i = 0; i < hei; i++)
			for (int j = 0; j < wid; j++)
			{
				OutputBlock(img, paddingBlock, maskSize, i, j, PStride);
				img->Median[i * wid + j] = MedianFunc(paddingBlock, maskSize);
			}
		free(img->padding);
		free(paddingBlock);
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	ImageOutput(img->Blur, wid, hei, String[6]);
	ImageOutput(img->Median, wid, hei, String[4]);

	free(img->Blur);
	free(img->Median);
}