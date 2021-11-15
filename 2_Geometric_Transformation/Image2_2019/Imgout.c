#include "GEO.h"

void ImageOutput(UChar* Data, Int wid, Int hei, Char String[])
{
	char Name_Hist[50] = "out_";
	char Name_extension[10] = ".raw";

	FILE *fp;

	

	strcat_s(Name_Hist, 20, String);
	strcat_s(Name_Hist, 20, Name_extension);


	fopen_s(&fp, Name_Hist, "wb"); //원본 영상 열기
	fwrite(Data, sizeof(UChar), wid * hei, fp);
	fclose(fp);
}