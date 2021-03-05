#ifndef PASSPORT_H_
#define PASSPORT_H_

/*对外接口说明
 * EchipOneCardRead: 读取一行机读码的护照
 * EchipTwoCardRead: 读取两行机读码的护照
 * EchipThreeCardRead: 读取三行机读码的护照
 * 对于DG1,DG11,Path都需要预先设置内存,Path设置完整的路径包括文件名,以.bmp为后缀
 * 	入参:
 * 		code: 机读码
 * 		path : 存储DG2文件的生成路径,也就是护照电子芯片中的头像文件的路径 (由于DG2文件有20K bytes左右,不方便传递)
 * 		Len1,Len11,PathLen: 作为入参时,表示DG1,DG11,path的内存大小,DG1数据大小在100byte以内,Path路径数据大小在1024byte以内
 *  出参:
 *  	DG1 : 返回存储DG1文件的数据,也即是护照电子芯片中的机读码
 *  	DG11 : 返回存储DG11文件文件信息,包括本国际语言的姓名等信息
 * 		Len1,Len11: 作为出参时,表示DG1,DG11的数据大小
 *
 * IDCardRead:身份证读取
 * 	入参:
 * 		IDInfo : 存储身份证信息,如姓名,身份证号,出生日期等等信息
 * 		IDPic  : 存储身份证所有者图片数据,数据量不少于1K
 * 		Len1,Len2: 作为入参时,表示DIDInfo,IDPic的内存大小,DIDInfo数据大小约为300byte,IDPic路径数据大小不少于1K bytes
 *  出参:
 * 		Len1,Len2: 作为出参时,表示DIDInfo,IDPic的数据大小
 * 返回值说明:
 * 		以上的函数,返回0表示失败,返回1表示成功
 * */


extern int EchipOneCardRead(const char *code,
                            unsigned char *DG1, unsigned int *Len1,
                            unsigned char *DG11, unsigned int *Len11,
                            unsigned char *path, unsigned int PathLen);

extern int EchipTwoCardRead(const char *code,
                            unsigned char *DG1, unsigned int *Len1,
                            unsigned char *DG11, unsigned int *Len11,
                            unsigned char *path, unsigned int PathLen);

extern int EchipThreeCardRead(const char *code,
                              unsigned char *DG1, unsigned int *Len1,
                              unsigned char *DG11, unsigned int *Len11,
                              unsigned char *path, unsigned int PathLen);

extern int IDCardRead(unsigned char *IDInfo, unsigned int *Len1,
                      unsigned char *IDPic, unsigned int *Len2);

#endif /* PASSPORT_H_ */
