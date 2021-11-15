#include "Filter.h"

void ImagePadding(Img_Buf* img, UChar* Buf, int width, int height, int maskSize)
{
	int line, i, j;
	int Add_size = (maskSize / 2) * 2;
	int Stride = width;
	int PStride = width + Add_size;
	img->padding = (UChar*)calloc((width + Add_size) * (height + Add_size), sizeof(UChar));

	for (line = 0; line < (maskSize / 2); line++)
	{
		//상하단 패딩
		for (i = 0; i < width; i++)
		{
			img->padding[line                               * PStride + (maskSize / 2 + i)] = Buf[i];
			img->padding[((height + (maskSize / 2)) + line) * PStride + (maskSize / 2 + i)] = Buf[(height - 1) * Stride + i];
		}

		//좌우측 패딩
		for (i = 0; i < height; i++)
		{
			img->padding[(maskSize / 2 + i)                 * PStride +         line] = Buf[i * Stride];
			img->padding[(maskSize / 2 + i)                 * PStride + width + line] = Buf[i * Stride + (width - 1)];
		}
	}

	for (i = 0; i < (maskSize / 2); i++)
	{
		for (j = 0; j < (maskSize / 2); j++)
		{
			/*** 좌상단 패딩 ***/
			img->padding[i * PStride + j] = Buf[0];
			
			/*** 우상단 패딩 ***/
			img->padding[i * PStride + ((maskSize / 2) + width + j)] = Buf[width - 1];
			
			/*** 좌하단 패딩 ***/
			img->padding[(height + (maskSize / 2) + i) * PStride + j] = Buf[(height - 1) * Stride];
			
			/*** 우하단 패딩 ***/
			img->padding[(height + (maskSize / 2) + i) * PStride + ((maskSize / 2) + width + j)] = Buf[(height - 1) * Stride + (width - 1)];
		}
	}

	/*** 원본 버퍼 불러오기 ***/
	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			img->padding[(i + (maskSize / 2)) * PStride + (j + (maskSize / 2))] = Buf[i * Stride + j];
		}
	}
}