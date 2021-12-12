#include "main.h"

InvTransQuantFunc(Int* quantBlk, Int Blk, Int* invResiBlk)
{
	Int Quant_Table[] = { 8, 10, 12, 14,
					  14, 16, 18, 20,
					  18, 20, 22, 24,
					  20, 22, 24, 26
	};


	Double DCTBlk[BLOCK_SIZE * BLOCK_SIZE];
	Double Recon_R;
	Double DCT_Real;

	/////////////////////////////////////////////////////////////////////////////
	// 내가 작성

	// 4x4이기 때문에 -> 입력받는 것 8x8이니까 4x4로 만들기
	Int blk4 = 4;

	// 역 양자화 과정
	// 나머지 부분은 0
	for (Int i = 0; i < Blk; i++)
		for (Int j = 0; j < Blk; j++)
			DCTBlk[i * Blk + j] = 0;
	// 4x4에 해당하는 부분만을 역양자화
	for (Int i = 0; i < blk4; i++)
		for (Int j = 0; j < blk4; j++)
			DCTBlk[i * Blk + j] = quantBlk[i * blk4 + j] * Quant_Table[i * blk4 + j];

	// 역변환
	for (Int i = 0; i < Blk; i++)
	{
		for (Int j = 0; j < Blk; j++)
		{
			Recon_R = 0;
			/////////////////////////////////////////////////////////////////////////////////// I-DCT
			for (Int k = 0; k < Blk; k++)
			{
				for (Int l = 0; l < Blk; l++)
				{
					// 역양자화된 블록
					DCT_Real = DCTBlk[k * Blk + l] * cos((2 * i + 1) * k * pi / (2 * Blk)) * cos((2 * j + 1) * l * pi / (2 * Blk));

					// C(u), C(v)
					if (k == 0 && l == 0)
						Recon_R += DCT_Real / Blk;
					else if (k == 0 || l == 0)
						Recon_R += (DCT_Real * (1.0 / sqrt(2.0))) / (Blk / 2);
					else
						Recon_R += DCT_Real / (Blk / 2);
				}
			}

			// clipping & 정수화
			if (Recon_R < 0)
				Recon_R = (int)(Recon_R - 0.5);
			else
				Recon_R = (int)(Recon_R + 0.5);

			///////////////////////////////////////////////////////////////////////////////////
			invResiBlk[i * Blk + j] = Recon_R;
		}
	}

	/////////////////////////////////////////////////////////////////////////////
}