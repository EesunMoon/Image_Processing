#include "main.h"

typedef struct _refsample
{
	Int(*at)(refBuf, Int, Int);
}refSam;

UChar refSample(refBuf ref, Int side, Int pos)
{
	return ref.refPix[side][pos];
}

PredictionFunc(UChar* predBlk, refBuf ref, Int blkSize, Int blkStride, UInt mode)
{
	UChar* refMain; // ���� ȭ�� ���� ����
	UChar* refSide; // ���� ȭ�� ���� ����

	UChar refAbove[2*BLOCK_SIZE+3]; // �� ���� ȭ��(i,a,b,c,d)
	UChar refLeft[2*BLOCK_SIZE+3]; // ���� ����ȭ��(i,h,g,f,e)

	UInt bIsModeVer = (mode == HORIZONTAL) ? 0 : 1; // ���� ���� ��尡 �����̳�, �����̳�
	Int intraPredAngle = mode == DIAGONAL ? -32 : 0; // ���� ���� ��尡 DIAGONAL ���� �ƴϳ�
	UChar predBuf[BLOCK_SIZE * BLOCK_SIZE]; // �ӽ÷� ���� ��� ����

	refSam pSrc;
	pSrc.at = refSample;

	// 8x8 ���
	if (intraPredAngle == 0) {
		// vertical, horizontal, DC �� ���� refMain, refSide�� ���� ���
		for (int i = 0; i <= blkSize; i++) {
			// refAbove
			refAbove[i] = pSrc.at(ref, 0, i); // 0�̸� ���, 1�̸� ����
		}
		for (int i = 0; i <= blkSize; i++) {
			// refLeft
			refLeft[i] = pSrc.at(ref, 1, i); // 0�̸� ���, 1�̸� ����
		}

		// refMain�� refSide ����
		refMain = bIsModeVer ? refAbove : refLeft; // ������尡 �����̸� Above�� main, Left�� side
		refSide = bIsModeVer ? refLeft : refAbove; // ������尡 �����̸� Above�� side, Left�� main
	}
	else {
		// diagonal
		for (int i = 0; i <= blkSize; i++) {
			// refAbove
			refAbove[i + blkSize] = pSrc.at(ref, 0, i); // 0�̸� ���, 1�̸� ���� 
			// blkSize��ŭ ������ �����ֱ�
		}
		for (int i = 0; i <= blkSize; i++) {
			// refLeft
			refLeft[i + blkSize] = pSrc.at(ref, 1, i); // 0�̸� ���, 1�̸� ����
			// blkSize��ŭ ������ �����ֱ�
		}

		// refMain�� refSide ����
		// diagonal ��忡���d refMain�� refAbove�� ����
		refMain = bIsModeVer ? refAbove + blkSize : refLeft + blkSize; // refMain[0] == (i)�� ������ֱ�
		refSide = bIsModeVer ? refLeft + blkSize : refAbove + blkSize;

		int sideSize = blkSize;
		int sideStart = blkSize;
		// refMain�� -8���� -1���� refleft���� �� �� �ְ�
		for (int k = -sideSize; k <= -1; k++) {
			refMain[k] = refSide[k * (-1)];
		}
	}


	Pel* pDsty = predBuf; // �ӽ� ���� ȭ�� ���ۿ� ���� ȭ�� �ֱ� 
	if (mode == VERTICAL || mode == HORIZONTAL)
	{
		for (int y = 0; y < blkSize; y++) {
			for (int x = 0; x < blkSize; x++) {
				pDsty[x] = refMain[x + 1];
			}
			pDsty += blkSize;
		}
	}
	else if (mode == DIAGONAL)
	{
		// Diagonal ������� - �̷��� �ϸ� �ڵ��� ������ ��� �Ǵ��� �� ����!! (�׸� ����)
		for (int y = 0, deltapos = intraPredAngle; y < blkSize;y++, deltapos += intraPredAngle, pDsty +=blkSize) {
			const int deltaint = deltapos >> 5; // scale down

			for (int x = 0; x <blkSize; x++) {
				pDsty[x] = refMain[x + deltaint + 1];
			}
		}
	}
	else
	{
		// DC ������� - ���
		int DC_mean, DC_sum = 0;
		for (int i = 1; i <= blkSize; i++)
			DC_sum += refMain[i];
		for (int i = 1; i <= blkSize; i++)
			DC_sum += refSide[i];
		DC_mean = DC_sum / (blkSize * 2); // ���

		for (int y = 0; y < blkSize; y++) {
			for (int x = 0; x < blkSize; x++) {
				pDsty[x] = DC_mean;
			}
			pDsty += blkSize;
		}
	}

	// ���� ��� ����
	for (int y = 0; y < blkSize; y++)
		for (int x = 0; x < blkSize; x++)
			predBlk[y * blkStride + x] = bIsModeVer ? predBuf[y * blkStride + x] : predBuf[x *blkStride + y]; // horizontal�̸� transpose�ǰ�
}