#include "pyContentCheck.h"

 /*int pySourceSeperate(char* ImageLoc, char* nation, char* type, int mode)
{
	 ContentCheck M;
	 return M.SourceSeperate(ImageLoc, nation, type, mode);
}*/

long long pySourceSeperate_WI_UV_IR(char* WIImageLoc, char* NIRImageLoc, char* UVImageLoc, char* nation, char* CodePath)
 {
	 ContentCheck M;
	 int Code[14];
	 for (int m = 0; m < 14; ++m)
		 Code[m] = 5;

	 M.SourceSeperate_New(WIImageLoc, NIRImageLoc, UVImageLoc, nation, CodePath, Code);

	 long long result = 0;

	 for (int m = 13; m >= 0; --m)
		 result = result * 10 + Code[m];
	 return result;
 }

int pySourceSeperate_New(char* WIImageLoc, char* NIRImageLoc, char* UVImageLoc, char* nation, char* CodePath, int(&RCode)[14])
{
	ContentCheck M;
	return M.SourceSeperate_New(WIImageLoc, NIRImageLoc, UVImageLoc, nation, CodePath, RCode);
}

 int pyUVImageCheck(char* UVImageLoc)
 {
	 ContentCheck M;
	 return M.UVImageCheck(UVImageLoc);
 }
