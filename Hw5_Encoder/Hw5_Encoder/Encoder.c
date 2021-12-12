#include "main.h"

initCurrentBlock(UChar *curBlk, UChar *oriLuma, UInt ctbX, UInt ctbY, Int m_picWidthInCtu, Int blkSize)
{
	UChar* oriPos, * curPos;
	
	oriPos = oriLuma + ((ctbY * blkSize) * (m_picWidthInCtu * blkSize) + (ctbX * blkSize));         //���� �� ���� ����� ���� �� ��ǥ
	curPos = curBlk;

	for (Int line = 0; line < blkSize; line++)
	{
		memcpy(curPos, oriPos, blkSize);

		oriPos += (m_picWidthInCtu * blkSize);
		curPos += blkSize;
	}
}


// xFillRegerenceSamples :: �м��ؾ���!!!!!!!!!!!
refBuf xFillReferenceSamples(UChar* recPos, UInt ctbX, UInt ctbY, Int stride)
{
	refBuf ref;
	recPos -= stride;
	recPos -= 1;

	// ������ ����� ��쿡�� 128�� �ֱ�
	if (!ctbX || !ctbY)
		ref.refPix[0][0] = ref.refPix[1][0] = 128;
	else
		ref.refPix[0][0] = ref.refPix[1][0] = recPos[0];

	// ������ ��谡 �ƴ� ��쿡�� �ش�Ǵ� ���� ȭ�� �� �ֱ�
	// refPix[0] �̸� ��ܿ� �����ϴ� ���� ȭ��
	for (Int i = 0; i < BLOCK_SIZE; i++)
		ref.refPix[0][i + 1] = !ctbY ? 128 : recPos[i+1];
	// refPix[1] �̸� ������ �����ϴ� ���� ȭ��
	for (Int i = 0; i < BLOCK_SIZE; i++)
	{
		recPos += stride;
		ref.refPix[1][i + 1] = !ctbX ? 128 : recPos[0];
	}

	return ref;
}

Encoding_Process(UChar* oriLuma, UChar* recLuma, Int m_picWidthInCtu, Int m_picHeightInCtu, Img_Buf* img)
{
	fopen_s(&img->info.stream, "Bitstream.raw", "wb");
	Int blkSize = BLOCK_SIZE;
	Int startX = 0, startY = 0; // block�� ���۵Ǵ� ��
	Int stopX = m_picWidthInCtu, stopY = m_picHeightInCtu; // block�� �� �� - ���ι������� �� �� �ִ���, ���� �������� �� �� �ִ���

	UChar* recPos;
	refBuf ref;

	Int bistreamCandidate[4][(BLOCK_SIZE / 2) * (BLOCK_SIZE / 2)]; // �� mode���� ����ȭ�� ��ȯ��� �� - �̹���
	Int candiRecBlk[4][BLOCK_SIZE * BLOCK_SIZE]; // ���� mode���� ������ ��

	UChar* curBlk, * predBlk;
	Int* resiBlk, *quantBlk, *invResiBlk;

	curBlk = (UChar*)calloc(blkSize * blkSize, sizeof(UChar)); // ���� ���
	predBlk = (UChar*)calloc(blkSize * blkSize, sizeof(UChar)); // ���� ���
	resiBlk = (Int*)calloc(blkSize * blkSize, sizeof(Int)); // ���� ���
	invResiBlk = (Int*)calloc(blkSize * blkSize, sizeof(Int)); // ������ ���� ���
	quantBlk = (Int*)calloc((blkSize / 2) * (blkSize / 2), sizeof(Int)); // ����ȭ�� ��� - ó���� ���⼭ ����
	img->Quant_blk = (Int*)calloc((blkSize / 2) * (blkSize / 2), sizeof(Int)); // ����ȭ ��� - entropy �������� quantBlk �����ؼ� ����

	// ��� ��ŭ ����
	for (Int ctbY = startY; ctbY < stopY; ctbY++)
	{
		for (Int ctbX = startX; ctbX < stopX; ctbX++)
		{
			Int Sad[modeNum] = { 0 };
			Int Sort[modeNum] = { 0 };
			Int Best_Sad, tmp, BestMode;

			memset(curBlk, 0, sizeof(UChar) * blkSize * blkSize);
			initCurrentBlock(curBlk, oriLuma, ctbX, ctbY, m_picWidthInCtu, blkSize); //8x8 ���� ��� ����
			
			// ���� ����� ���� �� - ���� ����� ��ġ�� ���� ����� ��ġ�� ���ƾ� ��
			// ���� ����� ���� ���� ������ ��ġ
			recPos = recLuma + ((ctbY * blkSize) * (m_picWidthInCtu * blkSize) + (ctbX * blkSize));
			// ���� ȭ��(���� ȭ��)�� ä�� �ֱ� - ���� ��� ȭ�ҿ��� ������ ���Ǵ� �ֺ��� ������ ȭ��
			ref = xFillReferenceSamples(recPos, ctbX, ctbY, m_picWidthInCtu * blkSize);
			
			// ���� ���� - ���� ��� 0-3��
			for (UInt mode = 0; mode < modeNum; mode++)
			{
				memset(predBlk, 0, sizeof(UChar) * blkSize * blkSize);
				memset(resiBlk,  0, sizeof(Int) * blkSize * blkSize);
				
				// ���� ����� predBlk�� ���� - ���� ��� 4��
				PredictionFunc(predBlk, ref, blkSize, blkSize, mode);
				// ���� ��� ����
				for (Int pos = 0; pos < blkSize * blkSize; pos++)
					resiBlk[pos] = curBlk[pos] - predBlk[pos];
				// ��ȯ ����ȭ ���� - �������(resBlk)�� �Է�, ����ȭ���(quantBlk)�� ���
				TransQuantFunc(resiBlk, blkSize, quantBlk);
				// �� ���� ��� ���� ����ȭ ���� ����
				for (Int pos = 0; pos < (blkSize / 2) * (blkSize / 2); pos++)
					bistreamCandidate[mode][pos] = quantBlk[pos];
				
				// ������ȭ, ����ȯ - ��� : ���� ���� ���(���� ��� ���� �� ���� �����)
				// ���ڴ� ���� decoding�ϴ� �κ�
				InvTransQuantFunc(quantBlk, blkSize, invResiBlk);
				for (Int pos = 0; pos < blkSize * blkSize; pos++)
					candiRecBlk[mode][pos] = invResiBlk[pos] + predBlk[pos];
			}

			// ������ ���� ��带 ã�� - �м��ϱ�
			for (Int i = 0; i < blkSize * blkSize; i++)
			{
				Sad[0] += abs(curBlk[i] - candiRecBlk[0][i]);
				Sad[1] += abs(curBlk[i] - candiRecBlk[1][i]);
				Sad[2] += abs(curBlk[i] - candiRecBlk[2][i]);
				Sad[3] += abs(curBlk[i] - candiRecBlk[3][i]);
			}

			for (Int i = 0; i < modeNum; i++)
				Sort[i] = Sad[i];

			for (Int i = 1; i < modeNum; i++)
			{
				for (tmp = Sort[i], Best_Sad = i; Best_Sad > 0; Best_Sad--)
				{
					if (Sort[Best_Sad - 1] > tmp)
						Sort[Best_Sad] = Sort[Best_Sad - 1];
					else break;
				}
				Sort[Best_Sad] = tmp;
			}

			for (Int i = 0; i < modeNum; i++)
				if (Sort[0] == Sad[i])
				{
					BestMode = i;
					break;
				}
			// ������� ���� ���� ��� ã��
			// 
			///////////////////////////////////////////////////////////////////////////////////////////////
			// ���� ���� ��� ����
			UChar* recPosSave; // ��������

			//���� �� ���� ����� ���� �� ��ǥ
			recPosSave = recLuma + ((ctbY * blkSize) * (m_picWidthInCtu * blkSize) + (ctbX * blkSize));
			// ���� ����� ���� ������ �����ϱ�
			for (int i = 0; i < blkSize; i++)
				for (int j = 0; j < blkSize; j++)
				{
					// ���� ����� �ش��ϴ� ��ġ�� ������ǥ cf) m_picWidthInCtu * blkSize == width
					// candiRecBlk�� ��ϴ���!!
					recLuma[(ctbY * blkSize + i) * (m_picWidthInCtu * blkSize) + (ctbX * blkSize + j)] =
						candiRecBlk[BestMode][(blkSize * i + j)];
				}
			///////////////////////////////////////////////////////////////////////////////////////////////

			img->info.Best_Mode = BestMode; // best ��忡 �ش�Ǵ� ����ȭ����� entropy�� �Ѱ��ֱ�
			for (Int i = 0; i < blkSize / 2; i++)
				for (Int j = 0; j < blkSize / 2; j++)
					img->Quant_blk[i * (blkSize / 2) + j] = bistreamCandidate[BestMode][i * (blkSize / 2) + j]; // best��忡 �ش��ϴ� ����ȭ�� ����� ��������� ��

			// entropy�� �Ѿ�� bitstream���� ����� - ���� ��Ͽ� ���� ����ȭ ������ ����(��� ��� �ؾ���)
			Entropy(blkSize / 2, img);
		}
	}
	free(curBlk);
	free(predBlk);
	free(resiBlk);
	free(invResiBlk);
	free(quantBlk);
	free(img->Quant_blk);
	fclose(img->info.stream);
}