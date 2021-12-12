#include "main.h"

Int wid = WIDTH;
Int hei = HEIGHT;
Int frame = FRAME;

void main()
{
	Img_Buf image_bitstream_info;

	FILE* op;

	UChar* recLuma;
	Int picSize = wid * hei; //���� ������

	Int m_picWidthInCtu = wid / BLOCK_SIZE; //������� ����� ��
	Int m_picHeightInCtu = hei / BLOCK_SIZE; //�������� ����� ��
	
	fopen_s(&image_bitstream_info.Input_Bitstream, "Bitstream.raw", "rb");

	recLuma = (UChar*)malloc(sizeof(UChar) * picSize);

	fopen_s(&op, "REC_DEC_BasketballDrill_832x480_50.y", "wb");
	for (Int i = 0; i < 1; i++)
	{
		Decoding_Process(recLuma, m_picWidthInCtu, m_picHeightInCtu, &image_bitstream_info);

		fwrite(recLuma, sizeof(UChar), picSize, op);
		printf("Frame: %d\n", i);
	}

	fclose(image_bitstream_info.Input_Bitstream);
	fclose(op);

	
}


