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
	UChar* refMain; // 복원 화소 저장 버퍼
	UChar* refSide; // 복원 화소 저장 버퍼

	UChar refAbove[2*BLOCK_SIZE+3]; // 위 복원 화소(i,a,b,c,d)
	UChar refLeft[2*BLOCK_SIZE+3]; // 왼쪽 복원화소(i,h,g,f,e)

	UInt bIsModeVer = (mode == HORIZONTAL) ? 0 : 1; // 현재 예측 모드가 수평이냐, 수직이냐
	Int intraPredAngle = mode == DIAGONAL ? -32 : 0; // 현재 예측 모드가 DIAGONAL 모드냐 아니냐
	UChar predBuf[BLOCK_SIZE * BLOCK_SIZE]; // 임시로 예측 블록 저장

	refSam pSrc;
	pSrc.at = refSample;

	// 8x8 블록
	if (intraPredAngle == 0) {
		// vertical, horizontal, DC 일 때의 refMain, refSide의 선정 방법
		for (int i = 0; i <= blkSize; i++) {
			// refAbove
			refAbove[i] = pSrc.at(ref, 0, i); // 0이면 상단, 1이면 좌측
		}
		for (int i = 0; i <= blkSize; i++) {
			// refLeft
			refLeft[i] = pSrc.at(ref, 1, i); // 0이면 상단, 1이면 좌측
		}

		// refMain과 refSide 연결
		refMain = bIsModeVer ? refAbove : refLeft; // 예측모드가 수직이면 Above가 main, Left가 side
		refSide = bIsModeVer ? refLeft : refAbove; // 예측모드가 수평이면 Above가 side, Left가 main
	}
	else {
		// diagonal
		for (int i = 0; i <= blkSize; i++) {
			// refAbove
			refAbove[i + blkSize] = pSrc.at(ref, 0, i); // 0이면 상단, 1이면 좌측 
			// blkSize만큼 공간을 떼어주기
		}
		for (int i = 0; i <= blkSize; i++) {
			// refLeft
			refLeft[i + blkSize] = pSrc.at(ref, 1, i); // 0이면 상단, 1이면 좌측
			// blkSize만큼 공간을 떼어주기
		}

		// refMain과 refSide 연결
		// diagonal 모드에서늕 refMain에 refAbove가 들어옴
		refMain = bIsModeVer ? refAbove + blkSize : refLeft + blkSize; // refMain[0] == (i)로 만들어주기
		refSide = bIsModeVer ? refLeft + blkSize : refAbove + blkSize;

		int sideSize = blkSize;
		int sideStart = blkSize;
		// refMain의 -8부터 -1까지 refleft값이 들어갈 수 있게
		for (int k = -sideSize; k <= -1; k++) {
			refMain[k] = refSide[k * (-1)];
		}
	}


	Pel* pDsty = predBuf; // 임시 예측 화소 버퍼에 복원 화소 넣기 
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
		// Diagonal 예측모드 - 이렇게 하면 코드의 동작이 어떻게 되는지 꼭 쓰기!! (그림 참고)
		for (int y = 0, deltapos = intraPredAngle; y < blkSize;y++, deltapos += intraPredAngle, pDsty +=blkSize) {
			const int deltaint = deltapos >> 5; // scale down

			for (int x = 0; x <blkSize; x++) {
				pDsty[x] = refMain[x + deltaint + 1];
			}
		}
	}
	else
	{
		// DC 예측모드 - 평균
		int DC_mean, DC_sum = 0;
		for (int i = 1; i <= blkSize; i++)
			DC_sum += refMain[i];
		for (int i = 1; i <= blkSize; i++)
			DC_sum += refSide[i];
		DC_mean = DC_sum / (blkSize * 2); // 평균

		for (int y = 0; y < blkSize; y++) {
			for (int x = 0; x < blkSize; x++) {
				pDsty[x] = DC_mean;
			}
			pDsty += blkSize;
		}
	}

	// 예측 블록 저장
	for (int y = 0; y < blkSize; y++)
		for (int x = 0; x < blkSize; x++)
			predBlk[y * blkStride + x] = bIsModeVer ? predBuf[y * blkStride + x] : predBuf[x *blkStride + y]; // horizontal이면 transpose되게
}