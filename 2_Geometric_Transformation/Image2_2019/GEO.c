#include "GEO.h"

void Scaling(UChar* Data, Int dstWid, Int dstHei, Double scaleVal)
{
	SCALE scale;
	double srcX, srcY; //�� ������ ���� ȭ�� ��ġ (���� ��ġ�� �ƴ� �� ����)

	Char String[4][10] = { "Near", "Bi" , "BS" ,"Cu" };

	scale.Near = (UChar*)calloc(dstWid * dstHei, sizeof(UChar));
	scale.Bi = (UChar*)calloc(dstWid * dstHei, sizeof(UChar));
	scale.BS = (UChar*)calloc(dstWid * dstHei, sizeof(UChar));
	scale.Cu = (UChar*)calloc(dstWid * dstHei, sizeof(UChar));

	// ��� ������ ȭ�� �ϳ��ϳ��� �������ؾ� ��
	for (int i = 0; i < dstHei; i++)
	{
		for (int j = 0; j < dstWid; j++)
		{
			// ���������� ������ �ϴ� �κ� - ���� ���󿡼� ��� ���󸸵� ���� ���ϱ⿴����, ������󿡼� ���� ������ ���� ���� ������
			srcX = (double)j / scaleVal;
			srcY = (double)i / scaleVal;

			// �������� ȭ�� ��ġ�� ���� ��ġ�ϼ��� ������, �Ǽ� ��ġ�� ��찡 ���� ���� ������ ������ �մϴ�.
			// DATA : ���� ������ ���� - ���������� ���� ȭ�� ��ġ�� ���� ���� ���� �ֱ� ����,
			// srcX, srcY : �������� ȭ�� ��ġ
			// WIDTH : ȭ�Ұ� 2���� ��ǥ������, 1�������� ���� ���� WIDTH
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
	double srcX, srcY;	// Source ��ġ

	int New_X, New_Y;
	// int Center_X = WIDTH / 2, Center_Y = HEIGHT / 2; // ������ ���
	int Center_X = 0, Center_Y = 0; // (0,0)�� �߽����� �� ��

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
				// ��� ���
				srcX = cos(Seta)*((double)j - Center_X) + sin(Seta)*((double)i - Center_Y) + Center_X;
				srcY = -sin(Seta)*((double)j - Center_X) + cos(Seta)*((double)i - Center_Y) + Center_Y;

				New_X = (int)srcX;
				New_Y = (int)srcY;

				// �������� ���� ȭ���� ��ġ�� ���� ���� ���� �ִ��� Ȯ��
				if (!(New_X < 0 || New_X >= WIDTH - 1 || New_Y < 0 || New_Y >= HEIGHT - 1)) // ���� ȭ�Ұ� ���� ��� �ۿ� ��ġ
				{
					// interpolation�Լ�
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
		//���ϴ� �е�
		for (i = 0; i < width; i++)
		{
			img->padding[(width + Mask_size - 1) * line + Mask_size / 2 + i] = Buf[i];
			img->padding[(width + Mask_size - 1) * (height + Mask_size - 2 - line) + Mask_size / 2 + i] = Buf[i + (width * (height - 1))];
		}

		//�¿��� �е�
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
				/*** �»�� �е� ***/
				if (line == 0)
				{
					img->padding[(width + Mask_size - 1) * i + j] = Buf[0];
				}
				/*** ���� �е� ***/
				else if (line == 1)
				{
					img->padding[(width + Mask_size - 1) * i + Mask_size / 2 + width + j] = Buf[width - 1];
				}
				/*** ���ϴ� �е� ***/
				else if (line == 2)
				{
					img->padding[(width + Mask_size - 1) * (height + Mask_size - 2 - i) + j] = Buf[width * (height - 1)];
				}
				/*** ���ϴ� �е� ***/
				else
				{
					img->padding[(width + Mask_size - 1) * (height + Mask_size - 2 - i) + Mask_size / 2 + width + j] = Buf[width * height - 1];
				}
			}
		}
	}

	/*** ���� ���� �ҷ����� ***/
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

	// scaling�� ���� ����
	Int dstWid; // �����ϸ� ����� ������ ���� ����
	Int dstHei; // �����ϸ� ����� ������ ���� ����

	dstWid = wid * scaleVal + 0.5; // �ݿø�
	dstHei = hei * scaleVal + 0.5; // �ݿø�

	Rotation(Data); //ȸ��

	if (scaleVal < 1) // ��� �� ���� ���� ���� ���� - ������ �ϰ� ���� ����� ���� ��
	{
		Image_Filtering(Data, img);
		memcpy(Data, img->Result_Blurring, sizeof(UChar) * wid * hei);
		free(img->Result_Blurring);
	}

	Scaling(Data, dstWid, dstHei, scaleVal); // ����, ��������� ����, ��������� ����, scaling���
}