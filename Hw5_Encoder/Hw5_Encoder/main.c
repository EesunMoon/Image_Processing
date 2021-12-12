#include "main.h"

Int wid = WIDTH;
Int hei = HEIGHT;
Int frame = FRAME;

void main()
{
	FILE* fp;
	FILE* op;
	Img_Buf image_bitstream_info;

	UChar* oriLuma, *recLuma;
	Int picSize = wid * hei; //영상 사이즈

	// block size : 8 x 8
	Int m_picWidthInCtu  = wid / BLOCK_SIZE; //수평방향 블록의 수
	Int m_picHeightInCtu = hei / BLOCK_SIZE; //수직방향 블록의 수

	oriLuma = (UChar*)malloc(sizeof(UChar) * picSize);
	recLuma = (UChar*)malloc(sizeof(UChar) * picSize);

	fopen_s(&fp, "BasketballDrill_832x480_50.y", "rb"); // 원본영상
	fopen_s(&op, "REC_BasketballDrill_832x480_50.y", "wb"); // 복원영상

	for (Int i = 0; i < 1; i++)
	{
		memset(oriLuma, minVal, sizeof(UChar) * picSize);
		memset(recLuma, minVal, sizeof(UChar) * picSize);
		fread(oriLuma, sizeof(UChar), picSize, fp);

		// 원본영상과 복원영상 둘 다 encoding process에 넣음
		Encoding_Process(oriLuma, recLuma, m_picWidthInCtu, m_picHeightInCtu, &image_bitstream_info);

		fwrite(recLuma, sizeof(UChar), picSize, op);
		printf("Frame: %d\n", i);
	}

	free(oriLuma);
	free(recLuma);
	
	fclose(fp);
	fclose(op);
}