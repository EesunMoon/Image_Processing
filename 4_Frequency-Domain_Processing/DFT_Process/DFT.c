#include "main.h"
#if flagLPF
void LPF(Double* blkReal, Double* blkImag, Int wid, Int hei)
{
	// 버터워스 LPF 구현 

	int x, y;
	int tempx, tempy;
	int halfcols, halfrows;
	double butterworth, coordinate;
	halfcols = hei / 2;
	halfrows = wid / 2;

	for (y = 0; y < hei; y++)
	{
		if (y >= halfcols)
			tempy = y - halfcols;
		else
			tempy = y + halfcols;

		for (x = 0; x < wid; x++)
		{

			if (x >= halfrows)
				tempx = x - halfrows;
			else
				tempx = x + halfrows;

			coordinate = sqrt(pow((double)(tempx - halfcols), 2.0) + pow((double)(tempy - halfrows), 2.0));
			butterworth = 1.0 / (1.0 + pow(coordinate / D0, 2 * N0));
			
			
			blkReal[y * wid + x] *= butterworth;
			blkImag[y * wid + x] *= butterworth;
			
			
		}

	}
}
#endif
void DFT_Func(UChar* curBlk, Int blkSize, Int blkRow, Int blkCol, DFT_Val* DFT)
{
	Double PI = pi;
	Int stride = WIDTH;

	// 블록 단위로 값을 저장하기 위해
	Double* blkReal;
	Double* blkImag;
	Double* blkMag;
	Double* blkPha;

	blkReal = (Double*)calloc(blkSize * blkSize, sizeof(Double));
	blkImag = (Double*)calloc(blkSize * blkSize, sizeof(Double));
	blkMag = (Double*)calloc(blkSize * blkSize, sizeof(Double));
	blkPha = (Double*)calloc(blkSize * blkSize, sizeof(Double));

	// 512x512 에서 쓰임
#if TransType
	Int count = 0;
	Double* cosTable, * sinTable;
	FILE* fp, * op;

	cosTable = (Double*)calloc(WIDTH * HEIGHT * 2, sizeof(Double));
	sinTable = (Double*)calloc(WIDTH * HEIGHT * 2, sizeof(Double));


	fopen_s(&fp, "cosTable.txt", "rb");
	fopen_s(&op, "sinTable.txt", "rb");

	fread(cosTable, sizeof(double), WIDTH * HEIGHT * 2, fp);
	fread(sinTable, sizeof(double), WIDTH * HEIGHT * 2, op);

	fclose(fp);
	fclose(op);
#endif

	// i, j가 time domain(x, y), k, l이 frequency domain(u, v)
	for (Int i = 0; i < blkSize; i++)
	{
#if TransType 
		printf("DFT : %.2f %%\n", (++count) / (double)(WIDTH / blkSize * HEIGHT) * 100);
#endif
		for (Int j = 0; j < blkSize; j++)
		{
			for (Int k = 0; k < blkSize; k++)
			{
				for (Int l = 0; l < blkSize; l++)
				{ // 내가 작성한 것
#if TransType // 512 x 512 일 때 - 시간을 단축하고자 cosTable, sinTable 사용
					// 2*pi*(ux/m+vx/n) :: 2, pi, m, n은 같음 i*k+j*l만 변함
					blkReal[i * blkSize + j] += curBlk[k * blkSize + l] * cosTable[i * k + j * l];
					blkImag[i * blkSize + j] -= curBlk[k * blkSize + l] * sinTable[i * k + j * l];
#else // 8x8 일 때
					blkReal[i * blkSize + j] += curBlk[k * blkSize + l] * cos(2 * pi * (i * k + j * l) / blkSize); // 실수부
					blkImag[i * blkSize + j] -= curBlk[k * blkSize + l] * sin(2 * pi * (i * k + j * l) / blkSize); // 허수부
#endif
				}
			}
			// 1/(MxN) 수식 
			// => blkReal = blkReal/(M*N) :: M, N - block size
			// => blkImag = blkImag/(M*N)
			blkReal[i * blkSize + j] = blkReal[i * blkSize + j] / (blkSize * blkSize);
			blkImag[i * blkSize + j] = blkImag[i * blkSize + j] / (blkSize * blkSize);
		}
	} // 내가 작성한 것


#if TransType

#if flagLPF // 분석해보기
	LPF(blkReal, blkImag, WIDTH, HEIGHT); // Low pass filter
#endif
#endif

	for (Int i = 0; i < blkSize; i++)
	{
		for (Int j = 0; j < blkSize; j++)
		{
			// 내가 적은 것
			blkMag[i * blkSize + j] = sqrt(pow(blkReal[i * blkSize + j], 2) + pow(blkImag[i * blkSize + j], 2)); // Magnitude : sqrt(a^2+b^2)
			blkPha[i * blkSize + j] = atan2(blkImag[i * blkSize + j], blkReal[i * blkSize + j]); // Phase : atan2(b/a)
			// 내가 적은 것
		}
	}

	for (Int i = 0; i < blkSize; i++)      //블록을 원래 있던 픽쳐 위치에 저장 - 떼 온 위치로 저장
	{
		for (Int j = 0; j < blkSize; j++)
		{
			DFT->picReal     [(blkRow * blkSize + i) * stride + (blkCol * blkSize + j)] = blkReal[i * blkSize + j];
			DFT->picImag     [(blkRow * blkSize + i) * stride + (blkCol * blkSize + j)] = blkImag[i * blkSize + j];
			DFT->picMagnitude[(blkRow * blkSize + i) * stride + (blkCol * blkSize + j)] = blkMag[i * blkSize + j];
			DFT->picPhase    [(blkRow * blkSize + i) * stride + (blkCol * blkSize + j)] = blkPha[i * blkSize + j];
		}
	}

	// SPECTRUM	
#if TransType

	Int wid = WIDTH;
	Int hei = HEIGHT;
	
	Double C, Temp, Spec;
	Double* SpecTmp;
	UChar* Shuffling;

	SpecTmp   = (Double*)calloc(blkSize * blkSize, sizeof(Double));
	Shuffling = (UChar*)calloc(blkSize * blkSize, sizeof(UChar));

	if (blkSize == wid && blkSize == hei)
	{
		// Log를 이용해서 scale down
		C = hypot(DFT->picReal[(blkRow * blkSize) + (blkCol * (wid + (wid % blkSize)) * blkSize)], DFT->picImag[(blkRow * blkSize) 
			+ (blkCol * (wid + (wid % blkSize)) * blkSize)]);
		for (Int i = 0; i < blkSize; i++)
		{
			Spec = 0;
			for (Int j = 0; j < blkSize; j++)
			{
				Temp = hypot(DFT->picReal[(blkRow * blkSize) + (blkCol * (wid + (wid % blkSize)) * blkSize) + ((wid + (wid % blkSize)) * i) + j], 
					DFT->picImag[(blkRow * blkSize) + (blkCol * (wid + (wid % blkSize)) * blkSize) + ((wid + (wid % blkSize)) * i) + j]);
				Spec = (C * log(1.0 + abs(Temp))) < 0.0 ? 0.0 : (C * log(1.0 + abs(Temp))) > 255.0 ? 255.0 : (C * log(1.0 + abs(Temp)));
				SpecTmp[i * blkSize + j] = Spec;
			}
		}

		// 셔플링 - 대칭을 이용해서 에너지를 가운데로 모아줌
		for (Int i = 0; i < blkSize; i += (blkSize / 2))
		{
			for (Int j = 0; j < blkSize; j += (blkSize / 2))
			{
				for (Int k = 0; k < (blkSize / 2); k++)
				{
					for (Int l = 0; l < (blkSize / 2); l++)
					{
						Shuffling[wid * (k + i) + (l + j)] = (UChar)SpecTmp[wid * (255 - k + i) + (255 - l + j)];
					}
				}
			}
		}
		FILE* cp;
		fopen_s(&cp, "DFT_Spectrum.raw", "wb");
		
		fwrite(Shuffling, sizeof(UChar), blkSize * blkSize, cp);

		free(SpecTmp);
		free(Shuffling);

		fclose(cp);
	}

	
#endif



	free(blkReal);
	free(blkImag);
	free(blkMag);
	free(blkPha);

#if TransType
	free(cosTable);
	free(sinTable);
#endif
	
}

void DFT_Process(Img_Buf* img, DFT_Val* DFT)
{
	Int blkSize = BLOCK_SIZE; // 현재 block size
	Int wid = WIDTH; Int hei = HEIGHT; // 영상의 size
	Int min = minVal; Int max = maxVal;
	Int picSize = wid * hei; //영상 사이즈
	Int stride = wid;

	UChar* TL_curBlk; // 현재 블록의 왼쪽위 지점 좌표
	UChar* TMP; 
	UChar* curBlk; // 블록 만큼 떼와서 변환을 수행하기 위한 변수

	DFT->picReal      = (Double*)calloc(picSize, sizeof(Double));
	DFT->picImag      = (Double*)calloc(picSize, sizeof(Double));
	DFT->picMagnitude = (Double*)calloc(picSize, sizeof(Double));
	DFT->picPhase     = (Double*)calloc(picSize, sizeof(Double));

#if TransType == 0
	Int count = 0;
#endif
	//DFT - block size가 512일 때는 한 번만 수행됨
	curBlk = (UChar*)calloc(blkSize * blkSize, sizeof(UChar));
	// 현재 block의 왼쪽 위 좌표를 찾음 - 왼쪽 위를 기준으로 블록을 가져옴
	// 원본영상의 size는 같지만 block의 사이즈에 따라서 BlkRow와 BlkCol의 개수가 달라짐
	for (Int blkRow = 0; blkRow < hei / blkSize; blkRow++)    //블록의 왼쪽 위 좌표를 찾기 위한 반복문
	{
		for (Int blkCol = 0; blkCol < wid / blkSize; blkCol++)//블록의 왼쪽 위 좌표를 찾기 위한 반복문
		{
			memset(curBlk, 0, sizeof(UChar) * blkSize * blkSize);
			///////////////////////////////////////////////////////////////////////////////////////////
			// img->ori :: ori[0] 원본 영상의 첫번째 공간. 즉, 0번째를 기준으로 얼마나 떨어져 있는 좌표를 구할 것인가?
			// - 2차원 좌표계를 1차원 좌표계로 블록 단위로 바꾸기 :: 원래는 y*stride+x (stride : 영상의 가로길이 -> 화소단위임)
			// 블록단위라면 (blkRow*_)*stride+(blkCol*_)
			TL_curBlk = img->ori + (blkRow*blkSize*stride + blkCol*blkSize);                          //블록의 왼쪽 위 좌표 
			for (int k = 0; k < blkSize; k++)         //블록의 왼쪽 위 좌표를 기준으로 한 blkSize x blkSize 추출
				for (int l = 0; l < blkSize; l++)
				{
					TMP = TL_curBlk + (k * stride + l);
					curBlk[k * blkSize + l] = TMP[0]; // curBlk : blkSize x blkSize의 화소값이 들어가 있음
				}

			DFT_Func(curBlk, blkSize, blkRow, blkCol, DFT); //curBlk: 현재 블록(blkSize x blkSize)

			// 현재 DFT가 얼마나 진행됐는지
#if TransType == 0
			count += (blkSize * blkSize);
			printf("DFT : %.2f %%\n", (double)count/(wid*hei)*100);
#endif
		}
	}
	free(curBlk);
}