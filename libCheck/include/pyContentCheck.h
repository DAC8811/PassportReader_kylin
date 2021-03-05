#pragma onc
#include "ContentCheck.h"


//extern"C" CHECKPOINT_API int pySourceSeperate(char* ImageLoc, char* nation, char* type, int mode);

long long pySourceSeperate_WI_UV_IR(char* WIImageLoc, char* NIRImageLoc, char* UVImageLoc, char* nation,
                                    char* CodePath);

int pySourceSeperate_New(char* WIImageLoc, char* NIRImageLoc, char* UVImageLoc, char* nation, char* CodePath, int(&RCode)[14]);

int pyUVImageCheck(char* UVImageLoc);