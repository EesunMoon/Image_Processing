#include "main.h"


TransQuantFunc(Int *resiBlk, Int Blk, Int *quantBlk)
{
	// 양자화 테이블
	Int Quant_Table[] = { 8, 10, 12, 14,
					  14, 16, 18, 20,
					  18, 20, 22, 24,
					  20, 22, 24, 26
	};

	// 변환했을 때 8x8 blk을 4x4 blk으로 만들기
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
			//DCT과정, DCTBlk에 변환 결과 저장
			for (Int k = 0; k < Blk; k++)
			{
				for (Int l = 0; l < Blk; l++)
				{
					DCTBlk[i * Blk + j] += resiBlk[k * Blk + l] * cos((2 * k + 1) * i * pi / (2 * Blk)) * cos((2 * l + 1) * j * pi / (2 * Blk));
				}
			}
			// C(u), C(v) 구하는 코드
			if (i == 0 && j == 0)
				DCTBlk[i * Blk + j] /= Blk;
			else if (i == 0 || j == 0)
				DCTBlk[i * Blk+ j] = (DCTBlk[i * Blk + j] * (1.0 / sqrt(2.0))) / (Blk / 2);
			else
				DCTBlk[i * Blk + j] = DCTBlk[i * Blk + j] / (Blk / 2);
			///////////////////////////////////////////////////////////////////////////////////////////////
		}
	}

	// DCTBlk와 Simple_LPF를 곱해줌으로써 4x4만 남겨둠
	for (Int i = 0; i < Blk; i++)
		for (Int j = 0; j < Blk; j++)
			DCTBlk[i * Blk + j] *= Simple_LPF[i * Blk + j];

	Int blk4 = 4;

	// 양자화
	for (Int i = 0; i < blk4; i++)
		for (Int j = 0; j < blk4; j++)
			quantBlk[i * blk4 + j] = (int)(DCTBlk[i * Blk + j] / Quant_Table[i * blk4 + j]);
}