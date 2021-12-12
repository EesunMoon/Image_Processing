#include "main.h"

refBuf xFillReferenceSamples(UChar* recPos, UInt ctbX, UInt ctbY, Int stride)
{
	refBuf ref;
	recPos -= stride;
	recPos -= 1;

	if (!ctbX || !ctbY)
		ref.refPix[0][0] = ref.refPix[1][0] = 128;
	else
		ref.refPix[0][0] = ref.refPix[1][0] = recPos[0];

	for (Int i = 0; i < BLOCK_SIZE; i++)
		ref.refPix[0][i + 1] = !ctbY ? 128 : recPos[i + 1];

	for (Int i = 0; i < BLOCK_SIZE; i++)
	{
		recPos += stride;
		ref.refPix[1][i + 1] = !ctbX ? 128 : recPos[0];
	}

	return ref;
}


Decoding_Process(UChar* recLuma, Int m_picWidthInCtu, Int m_picHeightInCtu, Img_Buf* img)
{
	Int blkSize = BLOCK_SIZE;
	Int startX = 0, startY = 0;
	Int stopX = m_picWidthInCtu, stopY = m_picHeightInCtu;

	UChar* recPos;
	refBuf ref; // ����ȭ��

	UChar* predBlk;
	Int * quantBlk, * invResiBlk;

	predBlk = (UChar*)calloc(blkSize * blkSize, sizeof(UChar)); // ���� ���
	invResiBlk = (Int*)calloc(blkSize * blkSize, sizeof(Int)); // ������ ���� ���
	quantBlk = (Int*)calloc((blkSize / 2) * (blkSize / 2), sizeof(Int)); // ����ȭ�� ���
	img->Quant_blk = (Int*)calloc((blkSize / 2) * (blkSize / 2), sizeof(Int)); // ����ȭ�� ��� - entropy�������� quantBlk�����ؼ� ���

	Int RecBlk[BLOCK_SIZE * BLOCK_SIZE]; // ���� ���� ���

	for (Int ctbY = startY; ctbY < stopY; ctbY++)
	{
		for (Int ctbX = startX; ctbX < stopX; ctbX++)
		{
			memset(img->Quant_blk, 0, sizeof(Int) * (blkSize / 2) * (blkSize / 2));
			
			Entropy(blkSize / 2, img);

			// ���� ����� ���� �� - ���� ����� ��ġ�� ���� ����� ��ġ�� ���ƾ� ��
			// ���� ����� ���� ���� ������ ��ġ
			recPos = recLuma + ((ctbY * blkSize) * (m_picWidthInCtu * blkSize) + (ctbX * blkSize));
			// ���� ȭ��(���� ȭ��)�� ä�� �ֱ�
			ref = xFillReferenceSamples(recPos, ctbX, ctbY, m_picWidthInCtu * blkSize);
			memset(predBlk, 0, sizeof(UChar) * blkSize * blkSize);

			for (Int pos = 0; pos < (blkSize / 2) * (blkSize / 2); pos++)
				quantBlk[pos] = img->Quant_blk[pos]; //��Ʈ��Ʈ�����κ��� ������ ����ȭ�� �� ����

			/////////////////////////////////////////////////////////////////////////////////////////////
			//���� �Լ� ȣ�� �κ�
			// ���� ����� predBlk�� ���� - ���� ��� 4��
			PredictionFunc(predBlk, ref, blkSize, blkSize, img->info.Best_Mode); // ���� �ۼ�
			/////////////////////////////////////////////////////////////////////////////////////////////
			
			/////////////////////////////////////////////////////////////////////////////////////////////
			//������ȭ, ����ȯ �Լ� ȣ�� �κ�
			// ������ȭ, ����ȯ - ��� : ���� ���� ���
			InvTransQuantFunc(quantBlk, blkSize, invResiBlk); // ���� �ۼ�
			/////////////////////////////////////////////////////////////////////////////////////////////

			for (Int pos = 0; pos < blkSize * blkSize; pos++)
				RecBlk[pos] = invResiBlk[pos] + predBlk[pos]; //���� ��� ����

			//���� ����� ���� ���� ���ۿ� ����
			for (Int i = 0; i < blkSize; i++)
				for (Int j = 0; j < blkSize; j++)
					recLuma[(ctbY * blkSize + i) * (m_picWidthInCtu * blkSize) + (ctbX * blkSize + j)] = RecBlk[(blkSize * i + j)];


		}
	}
	free(predBlk);
	free(invResiBlk);
	free(quantBlk);
	free(img->Quant_blk);



}

