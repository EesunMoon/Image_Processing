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
	// ���� �ۼ�

	// 4x4�̱� ������ -> �Է¹޴� �� 8x8�̴ϱ� 4x4�� �����
	Int blk4 = 4;

	// �� ����ȭ ����
	// ������ �κ��� 0
	for (Int i = 0; i < Blk; i++)
		for (Int j = 0; j < Blk; j++)
			DCTBlk[i * Blk + j] = 0;
	// 4x4�� �ش��ϴ� �κи��� ������ȭ
	for (Int i = 0; i < blk4; i++)
		for (Int j = 0; j < blk4; j++)
			DCTBlk[i * Blk + j] = quantBlk[i * blk4 + j] * Quant_Table[i * blk4 + j];

	// ����ȯ
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
					// ������ȭ�� ���
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

			// clipping & ����ȭ
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