#include "main.h"

void ImageCreate(Img_Buf *img, Int picSize)
{
	FILE* fp;

	fopen_s(&fp, "lena_512x512.raw", "rb"); //원본 영상 열기
	img->ori = (UChar*)malloc(sizeof(UChar) * picSize); //원본 영상 크기만큼 공간 선언
	img->rec = (UChar*)malloc(sizeof(UChar) * picSize); //결과 영상 크기만큼 공간 선언

	memset(img->ori, 0, sizeof(UChar) * picSize); //0으로 초기화
	fread(img->ori, sizeof(UChar), picSize, fp); // 원본 영상 읽기(원본 영상의 픽셀 값을 배열 변수에 저장)

	fclose(fp);
}

void ImageOutput(Img_Buf* img, Int picSize)
{
	FILE* fp;

	fopen_s(&fp, "Rec_DFT.raw", "wb"); //결과 영상 파일 열기
	
	fwrite(img->rec, sizeof(UChar), picSize, fp); // 원본 영상 읽기(원본 영상의 픽셀 값을 배열 변수에 저장)

	fclose(fp);
	free(img->ori);
	free(img->rec);
}

void main()
{
	
	Img_Buf img; // 원본영상과 복원영상의 버퍼를 정의하는 변수

	Int wid = WIDTH; Int hei = HEIGHT;
	Int min = minVal; Int max = maxVal;

	Int picSize = wid * hei; //영상 사이즈


	DFT_Val DFT;
	clock_t start, end; // 시간초를 세기 위한 변수
	float res;

	ImageCreate(&img, picSize); // 원본영상 읽어오기

	start = clock(); // 시간초 시작
	DFT_Process (&img, &DFT);
	IDFT_Process(&img, &DFT);
	PSNR(&img); // MSE, PSNR 계산
	end = clock(); // 시간초 끝
	res = (float)(end - start) / CLOCKS_PER_SEC; // 소요된 시간 계산
	ImageOutput(&img, picSize); // 복원 영상 출력

	printf("\nDFT 소요된 시간 : %.3f \n", res);
	printf("start = %d \n", start);
	printf("end   = %d \n", end);

	printf("\n\n\n");

	
}

void PSNR(Img_Buf* img)
{
	Int i, j;
	Int wid = WIDTH; Int hei = HEIGHT;
	Double mse = 0, psnr = 0, max = 0;
	UChar Img1 = 0, Img2 = 0;

	for (i = 0; i < hei; i++)
	{
		for (j = 0; j < wid; j++)
		{
			Img1 = img->ori[i * wid + j];
			Img2 = img->rec[i * wid + j];

			mse += ((Img1 - Img2) * (Img1 - Img2));
			if (Img1 > max)
				max = Img1;
		}
	}

	mse = mse / (wid * hei);
	printf("MSE : %lf\n", mse);
	psnr = 20 * log10(max / sqrt(mse));
	printf("PSNR : %lf\n", psnr);

	if (mse == 0)
		printf("\n영상 일치\n");
	else
		printf("\n영상 불일치\n");
}