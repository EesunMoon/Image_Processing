#include "GEO.h"

void Scaling(UChar* Data, Int dstWid, Int dstHei, Double scaleVal)
{
	SCALE scale;
	double srcX, srcY; //역 추적한 원본 화소 위치 (정수 위치가 아닐 수 있음)

	Char String[4][10] = { "Near", "Bi" , "BS" ,"Cu" };

	scale.Near = (UChar*)calloc(dstWid * dstHei, sizeof(UChar));
	scale.Bi = (UChar*)calloc(dstWid * dstHei, sizeof(UChar));
	scale.BS = (UChar*)calloc(dstWid * dstHei, sizeof(UChar));
	scale.Cu = (UChar*)calloc(dstWid * dstHei, sizeof(UChar));

	// 결과 영상의 화소 하나하나를 역추적해야 함
	for (int i = 0; i < dstHei; i++)
	{
		for (int j = 0; j < dstWid; j++)
		{
			// 실질적으로 역추적 하는 부분 - 원본 영상에서 결과 영상만들 때는 곱하기였으니, 결과영상에서 원본 영상을 만들 때는 나눗셈
			srcX = (double)j / scaleVal;
			srcY = (double)i / scaleVal;

			// 역추적한 화소 위치가 정수 위치일수도 있지만, 실수 위치일 경우가 더욱 많기 때문에 보간을 합니다.
			// DATA : 원본 영상의 정보 - 역추적했을 때의 화소 위치가 원본 영상 내에 있기 때문,
			// srcX, srcY : 역추적한 화소 위치
			// WIDTH : 화소가 2차원 좌표계지만, 1차원으로 쓰기 위해 WIDTH
			scale.Near[i * dstWid + j] = NearesetNeighbor(Data, srcX, srcY, WIDTH);
			scale.Bi[i * dstWid + j] = Bilinear(Data, srcX, srcY, WIDTH);
			scale.BS[i * dstWid + j] = B_Spline(Data, srcX, srcY, WIDTH);
			scale.Cu[i * dstWid + j] = Cubic(Data, srcX, srcY, WIDTH);
		}
	}

	ImageOutput(scale.Near, dstWid, dstHei, String[0]);
	ImageOutput(scale.Bi, dstWid, dstHei, String[1]);
	ImageOutput(scale.BS, dstWid, dstHei, String[2]);
	ImageOutput(scale.Cu, dstWid, dstHei, String[3]);

	free(scale.Near);
	free(scale.Bi);
	free(scale.BS);
	free(scale.Cu);
}

void Rotation(UChar* Data)
{
	ROTATION rot;
	FILE* up1, * up2, * up3, * up4;

	double Angle;
	double srcX, srcY;	// Source 위치

	int New_X, New_Y;
	// int Center_X = WIDTH / 2, Center_Y = HEIGHT / 2; // 영상의 가운데
	int Center_X = 0, Center_Y = 0; // (0,0)을 중심으로 할 때

	fopen_s(&up1, "Ro_Near.raw", "wb");
	fopen_s(&up2, "Ro_Bi.raw", "wb");
	fopen_s(&up3, "Ro_BS.raw", "wb");
	fopen_s(&up4, "Ro_Cu.raw", "wb");

	rot.Near = (UChar*)calloc(WIDTH * HEIGHT, sizeof(UChar));
	rot.Bi = (UChar*)calloc(WIDTH * HEIGHT, sizeof(UChar));
	rot.BS = (UChar*)calloc(WIDTH * HEIGHT, sizeof(UChar));
	rot.Cu = (UChar*)calloc(WIDTH * HEIGHT, sizeof(UChar));

	for (Angle = 0; Angle <= 360; Angle += 4)
	{
		double Seta = PI / 180.0 * Angle;

		for (int i = 0; i < HEIGHT; i++)
		{
			for (int j = 0; j < WIDTH; j++)
			{
				// 행렬 계산
				srcX = cos(Seta)*((double)j - Center_X) + sin(Seta)*((double)i - Center_Y) + Center_X;
				srcY = -sin(Seta)*((double)j - Center_X) + cos(Seta)*((double)i - Center_Y) + Center_Y;

				New_X = (int)srcX;
				New_Y = (int)srcY;

				// 역매핑한 원본 화소의 위치가 원본 영상 내에 있는지 확인
				if (!(New_X < 0 || New_X >= WIDTH - 1 || New_Y < 0 || New_Y >= HEIGHT - 1)) // 원시 화소가 영상 경계 밖에 위치
				{
					// interpolation함수
					rot.Near[i * WIDTH + j] = NearesetNeighbor(Data, srcX, srcY, WIDTH);
					rot.Bi[i * WIDTH + j] = Bilinear(Data, srcX, srcY, WIDTH);
					rot.BS[i * WIDTH + j] = B_Spline(Data, srcX, srcY, WIDTH);
					rot.Cu[i * WIDTH + j] = Cubic(Data, srcX, srcY, WIDTH);
				}
				else
				{
					rot.Near[i * WIDTH + j] = 0;
					rot.Bi[i * WIDTH + j] = 0;
					rot.BS[i * WIDTH + j] = 0;
					rot.Cu[i * WIDTH + j] = 0;
				}
			}
		}

		fwrite(rot.Near, sizeof(UChar), (WIDTH * HEIGHT), up1);
		fwrite(rot.Bi, sizeof(UChar), (WIDTH * HEIGHT), up2);
		fwrite(rot.BS, sizeof(UChar), (WIDTH * HEIGHT), up3);
		fwrite(rot.Cu, sizeof(UChar), (WIDTH * HEIGHT), up4);
	}

	free(rot.Near);
	free(rot.Bi);
	free(rot.BS);
	free(rot.Cu);

	fclose(up1);
	fclose(up2);
	fclose(up3);
	fclose(up4);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Image_Padding(Img_Buf* img, UChar* Buf, int width, int height, int Mask_size)
{
	int line, i, j;

	img->padding = (UChar*)calloc((width + Mask_size - 1) * (height + Mask_size - 1), sizeof(UChar));
	for (line = 0; line < (Mask_size / 2); line++)
	{
		//상하단 패딩
		for (i = 0; i < width; i++)
		{
			img->padding[(width + Mask_size - 1) * line + Mask_size / 2 + i] = Buf[i];
			img->padding[(width + Mask_size - 1) * (height + Mask_size - 2 - line) + Mask_size / 2 + i] = Buf[i + (width * (height - 1))];
		}

		//좌우측 패딩
		for (i = 0; i < height; i++)
		{
			img->padding[(width + Mask_size - 1) * (Mask_size / 2 + i) + line] = Buf[i * width];
			img->padding[(width + Mask_size - 1) * (Mask_size / 2 + 1 + i) - 1 - line] = Buf[i * width + (width - 1)];
		}
	}

	for (line = 0; line < 4; line++)
	{
		for (i = 0; i < (Mask_size / 2); i++)
		{
			for (j = 0; j < (Mask_size / 2); j++)
			{
				/*** 좌상단 패딩 ***/
				if (line == 0)
				{
					img->padding[(width + Mask_size - 1) * i + j] = Buf[0];
				}
				/*** 우상단 패딩 ***/
				else if (line == 1)
				{
					img->padding[(width + Mask_size - 1) * i + Mask_size / 2 + width + j] = Buf[width - 1];
				}
				/*** 좌하단 패딩 ***/
				else if (line == 2)
				{
					img->padding[(width + Mask_size - 1) * (height + Mask_size - 2 - i) + j] = Buf[width * (height - 1)];
				}
				/*** 우하단 패딩 ***/
				else
				{
					img->padding[(width + Mask_size - 1) * (height + Mask_size - 2 - i) + Mask_size / 2 + width + j] = Buf[width * height - 1];
				}
			}
		}
	}

	/*** 원본 버퍼 불러오기 ***/
	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			img->padding[(width + Mask_size - 1) * (Mask_size / 2 + i) + Mask_size / 2 + j] = Buf[i * width + j];
		}
	}
}

UChar Blurring(UChar* buf, int Mask_size)
{
	double Mask_Coeff[] = { 1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0 };
	double Convolution_All_coeff = 0;

	for (int i = 0; i < Mask_size * Mask_size; i++)
		Convolution_All_coeff += (Mask_Coeff[i] * (double)buf[i]);

	return Convolution_All_coeff = Convolution_All_coeff > maxVal ? maxVal : Convolution_All_coeff < minVal ? minVal : Convolution_All_coeff;
}

void Image_Filtering(UChar* Data, Img_Buf* img)
{
	int Mask_size = 3;         //MxM size
	int Add_size = Mask_size / 2 + 1;
	UChar Padding_buf[9] = { 0 };

	Image_Padding(img, Data, WIDTH, HEIGHT, 3);

	img->Result_Blurring = (UChar*)calloc(WIDTH * HEIGHT, sizeof(UChar));

	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			for (int k = 0; k < Mask_size; k++)
				for (int l = 0; l < Mask_size; l++)
					Padding_buf[k * Mask_size + l] = img->padding[(i + k) * (WIDTH + Add_size) + (j + l)];

			img->Result_Blurring[i * WIDTH + j] = Blurring(&Padding_buf, Mask_size);
		}
	}
	free(img->padding);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Geometric_Transformation(UChar* Data, Img_Buf* img)
{
	Int wid = WIDTH; Int hei = HEIGHT; 
	Int min = minVal; Int max = maxVal;

	Double scaleVal = scaleConstant;

	// scaling을 위한 변수
	Int dstWid; // 스케일링 적용된 영상의 가로 길이
	Int dstHei; // 스케일링 적용된 영상의 세로 길이

	dstWid = wid * scaleVal + 0.5; // 반올림
	dstHei = hei * scaleVal + 0.5; // 반올림

	Rotation(Data); //회전

	if (scaleVal < 1) // 축소 시 원본 영상 블러링 적용 - 예습을 하고 싶은 사람만 봐도 됨
	{
		Image_Filtering(Data, img);
		memcpy(Data, img->Result_Blurring, sizeof(UChar) * wid * hei);
		free(img->Result_Blurring);
	}

	Scaling(Data, dstWid, dstHei, scaleVal); // 원본, 결과영상의 가로, 결과영상의 세로, scaling상수
}