
/*****************************************
* 功能： 获取照片
* 参数：								 
*	dlpath： [IN] so路径, 例如："../lib/libwltdecode.so"
*	pBmpfilepath： [IN] 照片路径, 例如："../list/zp.bmp"
* 返回值：								 
*	成功返回0
* 包含证件类型：0、1、2
*****************************************/
extern "C" {
#ifdef WITH_EST100
// int GetBmpFile(const char* dlpath, char* pBmpfilepath);
/*
 * 函数功能：照片解码。
 * 参数说明：pwlt - 原始数据；pbmp - 解码后的照片数据
 * 返回值：0 - 表示成功；其他 - 表示失败
 */
    int GetBmpFile(const char* dlpath, char* pBmpfilepath);
#elif WITH_IDR220S
/*
 * 函数功能：照片解码。
 * 参数说明：pwlt - 原始数据；pbmp - 解码后的照片数据
 * 返回值：1 - 表示成功；其他 - 表示失败
 */ 
    int iDR220S_Dewlt(unsigned char* pwlt, unsigned char* pbmp);
#endif
}
