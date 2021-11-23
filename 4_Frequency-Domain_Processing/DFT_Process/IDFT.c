#include "main.h"

void IDFT_Func(Double* blkMag, Double* blkPha, Int blkSize, Int blkRow, Int blkCol, Img_Buf* img)
{
	Double PI = pi;
	Int stride = WIDTH;

	Double* DFT_Real;
	Double* DFT_Imag;
	Double Recon_R;

	DFT_Real = (Double*)calloc(blkSize * blkSize, sizeof(Double));
	DFT_Imag = (Double*)calloc(blkSize * blkSize, sizeof(Double));

#if TransType
	Int count = 0;
	Double* cosTable, * sinTable;
	FILE* fp, * op;

	cosTable = (Double*)calloc(WIDTH * HEIGHT * 2, sizeof(Double));
	sinTable = (Double*)calloc(WIDTH * HEIGHT * 2, sizeof(Double));


	fopen_s(&fp, "cosTable.txt", "rb");
	fopen_s(&op, "sinTable.txt", "rb");

	fread(cosTable, sizeof(double), WIDTH * HEIGHT * 2, fp);
	fread(sinTable, sizeof(double), WIDTH * HEIGHT * 2, op);

	fclose(fp);
	fclose(op);
#endif

	for (Int i = 0; i < blkSize; i++)
	{
		for (Int j = 0; j < blkSize; j++)
		{
			// DFT 과정에서 전송된 magnitude와 phase를 가지고 실수부와 허수부 작성
			// 내가 작성한 것
			DFT_Real[i * blkSize + j] = blkMag[i * blkSize + j] * cos(blkPha[i * blkSize + j]); // |x|cos(위상)
			DFT_Imag[i * blkSize + j] = blkMag[i * blkSize + j] * sin(blkPha[i * blkSize + j]); // |x|sin(위상)
			// 내가 작성한 것
		}
	}

	for (Int i = 0; i < blkSize; i++)
	{
#if TransType 
		printf("IDFT : %.2f %%\n", (++count) / (double)(WIDTH / blkSize * HEIGHT) * 100);
#endif
		for (Int j = 0; j < blkSize; j++)
		{
			Recon_R = 0;
			for (Int k = 0; k < blkSize; k++)
			{
				for (Int l = 0; l < blkSize; l++)
				{
#if TransType // 512*512 block
					// 내가 작성한 것
					Recon_R += DFT_Real[k * blkSize + l] * cosTable[i * k + j * l] -
						DFT_Imag[k * blkSize + l] * sinTable[i * k + j * l];
#else // 8*8 block
					// a*cos(2*pi*((i*k+j*l)/blkSize)) - b*sin((i*k+j*l)/blkSize)) :: a:DFT_Real, b:DFT_Imag
					Recon_R += (DFT_Real[k * blkSize + l] * cos(2 * pi * (i * k + j * l) / blkSize) - 
						DFT_Imag[k * blkSize + l] * sin(2 * pi * (i * k + j * l) / blkSize));
					// 내가 작성한 것
#endif
				}
			}
			if (Recon_R < 0)
				Recon_R = (int)(Recon_R - 0.5);

			else
				Recon_R = (int)(Recon_R + 0.5);
			img->rec[(blkRow * blkSize + i) * stride + (blkCol * blkSize + j)] = CLIP(Recon_R); // 하나의 화소값이 복원됨
		}
	}

	free(DFT_Real);
	free(DFT_Imag);
#if TransType
	free(cosTable);
	free(sinTable);
#endif

}

void IDFT_Process(Img_Buf* img, DFT_Val* DFT)
{
	Int blkSize = BLOCK_SIZE;
	Int wid = WIDTH; Int hei = HEIGHT;
	Int stride = wid;

	// 실수부에서 magnitude와 phase만 가져 옴
	Double* TL_blkMag;
	Double* Mag_TMP;
	Double* blkMag;

	Double* TL_blkPha;
	Double* Pha_TMP;
	Double* blkPha;

	

#if TransType == 0
	Int count = 0;
#endif
	blkMag = (Double*)calloc(blkSize * blkSize, sizeof(Double));
	blkPha = (Double*)calloc(blkSize * blkSize, sizeof(Double));
	for (Int blkRow = 0; blkRow < hei / blkSize; blkRow++)
	{
		for (Int blkCol = 0; blkCol < wid / blkSize; blkCol++)
		{
			memset(blkMag, 0, sizeof(Double) * blkSize * blkSize);
			memset(blkPha, 0, sizeof(Double) * blkSize * blkSize);

			// DFT에서 magnitude와 phase를 block단위로 가져 옴
			// 
			// 내가 작성한 것
			TL_blkMag = DFT->picMagnitude + (blkRow * blkSize * stride + blkCol * blkSize);
			TL_blkPha = DFT->picPhase     + (blkRow * blkSize * stride + blkCol * blkSize);
			// 내가 작성한 것
			
			for (int k = 0; k < blkSize; k++)
				for (int l = 0; l < blkSize; l++)
				{
					// Magnitude에 대해서 block단위로 떼옴
					Mag_TMP = TL_blkMag + (k * stride + l);
					blkMag[k * blkSize + l] = Mag_TMP[0];

					// phase에 대해 block단위로 떼옴
					Pha_TMP = TL_blkPha + (k * stride + l);
					blkPha[k * blkSize + l] = Pha_TMP[0];
				}

			IDFT_Func(blkMag, blkPha, blkSize, blkRow, blkCol, img);
#if TransType == 0
			count += (blkSize * blkSize);
			printf("IDFT : %.2f %%\n", (double)count / (wid * hei) * 100);
#endif
		}
	}
	free(blkMag);
	free(blkPha);

	free(DFT->picMagnitude);
	free(DFT->picPhase);
	free(DFT->picReal);
	free(DFT->picImag);
}