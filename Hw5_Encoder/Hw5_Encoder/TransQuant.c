#include "main.h"


TransQuantFunc(Int *resiBlk, Int Blk, Int *quantBlk)
{
	// ����ȭ ���̺�
	Int Quant_Table[] = { 8, 10, 12, 14,
					  14, 16, 18, 20,
					  18, 20, 22, 24,
					  20, 22, 24, 26
	};

	// ��ȯ���� �� 8x8 blk�� 4x4 blk���� �����
	Int Simple_LPF[64] = {
		1, 1, 1, 1, 0, 0, 0, 0,
		1, 1, 1, 1, 0, 0, 0, 0,
		1, 1, 1, 1, 0, 0, 0, 0,
		1, 1, 1, 1, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0
	};

	// Double Temp, Spec;
	Double DCTBlk[BLOCK_SIZE * BLOCK_SIZE] = {0};

	for (Int i = 0; i < Blk; i++)
	{
		for (Int j = 0; j < Blk; j++)
		{
			///////////////////////////////////////////////////////////////////////////////////////////////
			//DCT����, DCTBlk�� ��ȯ ��� ����
			for (Int k = 0; k < Blk; k++)
			{
				for (Int l = 0; l < Blk; l++)
				{
					DCTBlk[i * Blk + j] += resiBlk[k * Blk + l] * cos((2 * k + 1) * i * pi / (2 * Blk)) * cos((2 * l + 1) * j * pi / (2 * Blk));
				}
			}
			// C(u), C(v) ���ϴ� �ڵ�
			if (i == 0 && j == 0)
				DCTBlk[i * Blk + j] /= Blk;
			else if (i == 0 || j == 0)
				DCTBlk[i * Blk+ j] = (DCTBlk[i * Blk + j] * (1.0 / sqrt(2.0))) / (Blk / 2);
			else
				DCTBlk[i * Blk + j] = DCTBlk[i * Blk + j] / (Blk / 2);
			///////////////////////////////////////////////////////////////////////////////////////////////
		}
	}

	// DCTBlk�� Simple_LPF�� ���������ν� 4x4�� ���ܵ�
	for (Int i = 0; i < Blk; i++)
		for (Int j = 0; j < Blk; j++)
			DCTBlk[i * Blk + j] *= Simple_LPF[i * Blk + j];

	Int blk4 = 4;

	// ����ȭ
	for (Int i = 0; i < blk4; i++)
		for (Int j = 0; j < blk4; j++)
			quantBlk[i * blk4 + j] = (int)(DCTBlk[i * Blk + j] / Quant_Table[i * blk4 + j]);
}