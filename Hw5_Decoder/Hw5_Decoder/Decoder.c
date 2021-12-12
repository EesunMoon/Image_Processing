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
	refBuf ref; // 복원화소

	UChar* predBlk;
	Int * quantBlk, * invResiBlk;

	predBlk = (UChar*)calloc(blkSize * blkSize, sizeof(UChar)); // 예측 블록
	invResiBlk = (Int*)calloc(blkSize * blkSize, sizeof(Int)); // 복원된 잔차 블록
	quantBlk = (Int*)calloc((blkSize / 2) * (blkSize / 2), sizeof(Int)); // 양자화된 블록
	img->Quant_blk = (Int*)calloc((blkSize / 2) * (blkSize / 2), sizeof(Int)); // 양자화된 블록 - entropy과정에서 quantBlk복사해서 사용

	Int RecBlk[BLOCK_SIZE * BLOCK_SIZE]; // 최종 복원 블록

	for (Int ctbY = startY; ctbY < stopY; ctbY++)
	{
		for (Int ctbX = startX; ctbX < stopX; ctbX++)
		{
			memset(img->Quant_blk, 0, sizeof(Int) * (blkSize / 2) * (blkSize / 2));
			
			Entropy(blkSize / 2, img);

			// 원본 블록을 떼올 때 - 원본 블록의 위치와 복원 블록의 위치는 같아야 함
			// 원본 영상과 같은 복원 영상의 위치
			recPos = recLuma + ((ctbY * blkSize) * (m_picWidthInCtu * blkSize) + (ctbX * blkSize));
			// 참조 화소(복원 화소)값 채워 넣기
			ref = xFillReferenceSamples(recPos, ctbX, ctbY, m_picWidthInCtu * blkSize);
			memset(predBlk, 0, sizeof(UChar) * blkSize * blkSize);

			for (Int pos = 0; pos < (blkSize / 2) * (blkSize / 2); pos++)
				quantBlk[pos] = img->Quant_blk[pos]; //비트스트림으로부터 복원한 양자화된 값 저장

			/////////////////////////////////////////////////////////////////////////////////////////////
			//예측 함수 호출 부분
			// 예측 블록이 predBlk에 저장 - 예측 블록 4개
			PredictionFunc(predBlk, ref, blkSize, blkSize, img->info.Best_Mode); // 내가 작성
			/////////////////////////////////////////////////////////////////////////////////////////////
			
			/////////////////////////////////////////////////////////////////////////////////////////////
			//역양자화, 역변환 함수 호출 부분
			// 역양자화, 역변환 - 출력 : 최종 복원 블록
			InvTransQuantFunc(quantBlk, blkSize, invResiBlk); // 내가 작성
			/////////////////////////////////////////////////////////////////////////////////////////////

			for (Int pos = 0; pos < blkSize * blkSize; pos++)
				RecBlk[pos] = invResiBlk[pos] + predBlk[pos]; //복원 블록 생성

			//복원 블록을 복원 영상 버퍼에 저장
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

