#include "main.h"

initCurrentBlock(UChar *curBlk, UChar *oriLuma, UInt ctbX, UInt ctbY, Int m_picWidthInCtu, Int blkSize)
{
	UChar* oriPos, * curPos;
	
	oriPos = oriLuma + ((ctbY * blkSize) * (m_picWidthInCtu * blkSize) + (ctbX * blkSize));         //픽쳐 내 현재 블록의 왼쪽 위 좌표
	curPos = curBlk;

	for (Int line = 0; line < blkSize; line++)
	{
		memcpy(curPos, oriPos, blkSize);

		oriPos += (m_picWidthInCtu * blkSize);
		curPos += blkSize;
	}
}


// xFillRegerenceSamples :: 분석해야함!!!!!!!!!!!
refBuf xFillReferenceSamples(UChar* recPos, UInt ctbX, UInt ctbY, Int stride)
{
	refBuf ref;
	recPos -= stride;
	recPos -= 1;

	// 영상의 경계인 경우에는 128을 넣기
	if (!ctbX || !ctbY)
		ref.refPix[0][0] = ref.refPix[1][0] = 128;
	else
		ref.refPix[0][0] = ref.refPix[1][0] = recPos[0];

	// 영상의 경계가 아닌 경우에는 해당되는 복원 화소 값 넣기
	// refPix[0] 이면 상단에 존재하는 복원 화소
	for (Int i = 0; i < BLOCK_SIZE; i++)
		ref.refPix[0][i + 1] = !ctbY ? 128 : recPos[i+1];
	// refPix[1] 이면 좌측에 존재하는 복원 화소
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
	Int startX = 0, startY = 0; // block이 시작되는 값
	Int stopX = m_picWidthInCtu, stopY = m_picHeightInCtu; // block의 끝 값 - 가로방향으로 몇 개 있는지, 세로 방향으로 몇 개 있는지

	UChar* recPos;
	refBuf ref;

	Int bistreamCandidate[4][(BLOCK_SIZE / 2) * (BLOCK_SIZE / 2)]; // 각 mode별로 양자화된 변환블록 값 - 이미지
	Int candiRecBlk[4][BLOCK_SIZE * BLOCK_SIZE]; // 예측 mode별로 복원된 값

	UChar* curBlk, * predBlk;
	Int* resiBlk, *quantBlk, *invResiBlk;

	curBlk = (UChar*)calloc(blkSize * blkSize, sizeof(UChar)); // 현재 블록
	predBlk = (UChar*)calloc(blkSize * blkSize, sizeof(UChar)); // 예측 블록
	resiBlk = (Int*)calloc(blkSize * blkSize, sizeof(Int)); // 잔차 블록
	invResiBlk = (Int*)calloc(blkSize * blkSize, sizeof(Int)); // 복원된 잔차 블록
	quantBlk = (Int*)calloc((blkSize / 2) * (blkSize / 2), sizeof(Int)); // 양자화된 블록 - 처음에 여기서 저장
	img->Quant_blk = (Int*)calloc((blkSize / 2) * (blkSize / 2), sizeof(Int)); // 양자화 블록 - entropy 과정에서 quantBlk 복사해서 쓰임

	// 블록 만큼 돌기
	for (Int ctbY = startY; ctbY < stopY; ctbY++)
	{
		for (Int ctbX = startX; ctbX < stopX; ctbX++)
		{
			Int Sad[modeNum] = { 0 };
			Int Sort[modeNum] = { 0 };
			Int Best_Sad, tmp, BestMode;

			memset(curBlk, 0, sizeof(UChar) * blkSize * blkSize);
			initCurrentBlock(curBlk, oriLuma, ctbX, ctbY, m_picWidthInCtu, blkSize); //8x8 원본 블록 추출
			
			// 원본 블록을 떼올 때 - 원본 블록의 위치와 복원 블록의 위치는 같아야 함
			// 원본 영상과 같은 복원 영상의 위치
			recPos = recLuma + ((ctbY * blkSize) * (m_picWidthInCtu * blkSize) + (ctbX * blkSize));
			// 참조 화소(복원 화소)값 채워 넣기 - 현재 블록 화소에서 예측에 사용되는 주변된 복원된 화소
			ref = xFillReferenceSamples(recPos, ctbX, ctbY, m_picWidthInCtu * blkSize);
			
			// 예측 수행 - 예측 모드 0-3번
			for (UInt mode = 0; mode < modeNum; mode++)
			{
				memset(predBlk, 0, sizeof(UChar) * blkSize * blkSize);
				memset(resiBlk,  0, sizeof(Int) * blkSize * blkSize);
				
				// 예측 블록이 predBlk에 저장 - 예측 블록 4개
				PredictionFunc(predBlk, ref, blkSize, blkSize, mode);
				// 잔차 블록 생성
				for (Int pos = 0; pos < blkSize * blkSize; pos++)
					resiBlk[pos] = curBlk[pos] - predBlk[pos];
				// 변환 양자화 과정 - 잔차블록(resBlk)이 입력, 양자화블록(quantBlk)가 출력
				TransQuantFunc(resiBlk, blkSize, quantBlk);
				// 각 예측 모드 별로 양자화 과정 수행
				for (Int pos = 0; pos < (blkSize / 2) * (blkSize / 2); pos++)
					bistreamCandidate[mode][pos] = quantBlk[pos];
				
				// 역양자화, 역변환 - 출력 : 최종 복원 블록(예측 모드 별로 각 값이 저장됨)
				// 인코더 내의 decoding하는 부분
				InvTransQuantFunc(quantBlk, blkSize, invResiBlk);
				for (Int pos = 0; pos < blkSize * blkSize; pos++)
					candiRecBlk[mode][pos] = invResiBlk[pos] + predBlk[pos];
			}

			// 최적의 예측 모드를 찾기 - 분석하기
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
			// 여기까지 최적 예측 모드 찾기
			// 
			///////////////////////////////////////////////////////////////////////////////////////////////
			// 최종 예측 블록 저장
			UChar* recPosSave; // 복원영상

			//픽쳐 내 현재 블록의 왼쪽 위 좌표
			recPosSave = recLuma + ((ctbY * blkSize) * (m_picWidthInCtu * blkSize) + (ctbX * blkSize));
			// 복원 블록을 영상 단위로 저장하기
			for (int i = 0; i < blkSize; i++)
				for (int j = 0; j < blkSize; j++)
				{
					// 현재 블록이 해당하는 위치의 영상좌표 cf) m_picWidthInCtu * blkSize == width
					// candiRecBlk는 블록단위!!
					recLuma[(ctbY * blkSize + i) * (m_picWidthInCtu * blkSize) + (ctbX * blkSize + j)] =
						candiRecBlk[BestMode][(blkSize * i + j)];
				}
			///////////////////////////////////////////////////////////////////////////////////////////////

			img->info.Best_Mode = BestMode; // best 모드에 해당되는 양자화블록을 entropy에 넘겨주기
			for (Int i = 0; i < blkSize / 2; i++)
				for (Int j = 0; j < blkSize / 2; j++)
					img->Quant_blk[i * (blkSize / 2) + j] = bistreamCandidate[BestMode][i * (blkSize / 2) + j]; // best모드에 해당하는 양자화된 블록의 계수값들이 들어감

			// entropy로 넘어가서 bitstream으로 만들면 - 현재 블록에 대한 양자화 과정이 끝남(모든 블록 해야함)
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