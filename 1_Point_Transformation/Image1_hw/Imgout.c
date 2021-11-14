#include "Imgout.h"
#include <stdio.h>
void ImageOutput(UChar* Data, Int wid, Int hei, Char String[])
{
	char Name_Hist[50] = "Arith_";
	char Name_extension[10] = ".raw";

	FILE *fp;

	strcat(Name_Hist, String);
	strcat(Name_Hist, Name_extension);

	fopen_s(&fp, Name_Hist, "wb"); //원본 영상 열기
	fwrite(Data, sizeof(UChar), wid * hei, fp);
	fclose(fp);
}