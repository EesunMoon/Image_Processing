#include "Filter.h"

void main()
{
	FILE *fp;
	UChar *ori; //���� ���� ȭ�Ұ����� �����ϱ� ���� ����
	Img_Buf image; 


	Int wid = WIDTH; Int hei = HEIGHT;
	Int min = minVal; Int max = maxVal;

	Int picSize = wid * hei; //���� ������


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// filtering : Embossing, Sharpening, DoG, Sobel, Homogeneity
	fopen_s(&fp, "lena_512x512.raw", "rb"); //���� ���� ����
	ori = (UChar*)malloc(sizeof(UChar) * picSize); //���� ���� ũ�⸸ŭ ���� ����
	memset(ori, 0, sizeof(UChar) * picSize); //0���� �ʱ�ȭ
	fread(ori, sizeof(UChar), picSize, fp); // ���� ���� �б�(���� ������ �ȼ� ���� �迭 ������ ����)

	ImageFiltering(ori, &image);

	free(ori);
	fclose(fp);
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Median, Blur
	fopen_s(&fp, "Lena-scratch-noise.raw", "rb"); //���� ���� ���� - �Է� ���� lena scratch ����
	ori = (UChar*)malloc(sizeof(UChar) * picSize); //���� ���� ũ�⸸ŭ ���� ����
	memset(ori, 0, sizeof(UChar) * picSize); //0���� �ʱ�ȭ
	fread(ori, sizeof(UChar), picSize, fp); // ���� ���� �б�(���� ������ �ȼ� ���� �迭 ������ ����)

	ImageFilteringBlur(ori, &image); // �Է� - scratch lena

	free(ori);
	fclose(fp);
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}