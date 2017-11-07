#ifndef IMAGECOMPRESSION_H
#define IMAGECOMPRESSION_H

#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

//>>>----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//定义全局变量
//表头的长度,表头记录了数组长度(2个字节),数组元素的最大值(2个字节)与最小值(2个字节)
#define HEADER_LEN  6
//数组的长度,最大值,最小值与此系数取值和取余,使得值与余数在0-255范围内,
//使得数组的长度,最大值,最小值用两个字节就能完成存储
#define RATIO_FACTOR  100
#define BYTE_MAX_VALUE  255                         //byte类型最大值为255
//单个区块的大小,硬盘的最小单位为扇区,主要有512Byte和4K两种规格(4k是目前市面上的主流)
//如果磁盘扇区为4k,即使存储数据没有4k,但是实际硬盘占用空间也以4k计算
#define BLOCK_SIZE  4096
//缓存数组的长度,即将数据缓存至内存的总长度
#define LENGTH  10000
#define NAME_CHAR_CNT 80      //文件名及文件路径的字符数量
#define PRECISION    0.01     //计算还原数据的精度(单位:um,精确到小数点后两位)

//<<<----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//>>>----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// 定义宏函数,
// 随机获取数据精度(精度范围在0.00 - 1.00范围内,单位:um)
// 还原数据,数据整数部分直接映射,小数部分通过随机生成获取
#define GET_ACCURACY ((float)( rand() % RATIO_FACTOR  * PRECISION ))

//<<<----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//>>>----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//数据的对比与计算
/*
*  @brief  获取浮点型数组中最大值,并将其转换为整型返回
*  @param  srcArr: 需要统计所有元素最大值的数组
*          length: 数组的长度
*  @return 数组中的最大值(将最大值由浮点型转换成整型返回)
*/
int findMaxFromArr(float srcArr[],int length);

/*
*  @brief  获取浮点型数组中最小值,并将其转换为整型返回
*  @param  srcArr: 需要统计所有元素最小值的数组
*          length: 数组的长度
*  @return 数组中的最小值(将最小值由浮点型转换成整型返回)
*/
int findMinFromArr(float srcArr[],int length);

//<<<----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//>>>----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//压缩,解压数组数据
/*
*  @brief   将数组中的浮点型数据压缩到目标数组中,包括数据的表头
*           (表头信息: 数组的长度,数组元素的最大值,数组元素的最小值)
*  @param   srcArr: 需要压缩的浮点型数组
*           destArr: 将压缩数据压缩后存放在Byte型数组中
*           length : 需要压缩的浮点型数组的长度
*  @return  N/A
*/
void compressArray(float srcArr[],
                   unsigned char destArr[],
                   int length);

/*
*  @brief   根据文件名称获取数组的索引号,并根据文件中的数据解压至数组
*           (即将文件中Byte数据解压至float型数组中)
*  @param   file: 结构体指针(根据文件名获取数组的索引号)
*           srcArr: 需要解压的Byte型数组
*           pDestArr:  双重浮点型指针数组，指向数据还原后存放数组的头指针
*  @return  N/A
*/
void decompressArray(struct dirent *file,
                     unsigned char srcArr[],
                     float **pDestArr);

//>>>----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//>>>----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//数组数据映射(根据数组的最大值与最小值,将float型数组映射为Byte,或将Byte型数组映射为float)
/*
*  @brief 根据数组的最大值与最小值,将数组中的float型数组映射到Byte型的数组中
*         (数组中的最小值映射后的值为0,数组中的最大值映射后为255)
*  @param max: 数组中的最大值
*         min: 数组中的最小值
*         srcArr: 需要映射的源数组
*         destArr: 将数组映射后存放的目标数组
*         arrayLen: 需要映射的源数组的长度
*  @return  N/A
*/
void mapFloatArrToByteArr(int max,
                          int min,
                          float *srcArr,
                          unsigned char destArr[],
                          int arrayLen);

/*
*  @brief 根据数组的最大值与最小值,将数组中的Byte型数组映射到float型的数组中
*  @param destArr: 将映射后的数据存放到此目标float型数组中
*         max: 数组中的最大值(整型)
*         min: 数组中的最小值(整型)
*         srcArr: 需要映射的Byte型数组
*         arrayLen: 需要映射的源数组的长度
*  @return  N/A
*/
void mapByteArrToFloatArr(int max,
                          int min,
                          unsigned char srcArr[],
                          float * destArr,
                          int arrayLen);

//<<<----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//>>>----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//将文件I/O操作(包括文件的添加与删除)
/*
*  @brief   删除指定目录下的所有文件
*           (当存放文件的目录下文件已存在,则先删除目录下的所有文件,再进行文件存储)
*  @param   dir:文件目录名(要删除文件所在的目录)
*  @return  N/A
*/
void deleteFile(char * dir);

//<<<----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//>>>----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//将文件I/O操作(包括文件的添加与删除)
/*
*  @brief   将Byte型数组数据写入指定路径的文件
*  @param   pFilePath　：　（字符型数组）文件路径的头指针
*           srcArr ：要输出至文件的（unsigned char）源数组
*           length : 输出至文件的（unsigned char）数组的长度
*  @return  N/A
*/
void exportByteToFile(char *pFilePath,
                      unsigned char srcArr[],
                      int length);

/*
*  @brief  读取指定目录的文件,将文件数据中 Byte 数据存入到数组中
*  @param  file: 结构体指针(取出目录下的文件名称字符串)
*          destArr: 双重浮点型指针数组，指向数据还原后存放数组的头指针
*          dir :  文件目录 char 型指针,指向需要读取数据的文件目录
*  @return N/A
*/
void importByteFromFile(struct dirent * file,
                        unsigned char destArr[],
                        char * dir);

//<<<----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//>>>----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//文件数据的导入导出
/*
*  @brief   轮询数组中数据,将大于等于4096的数组直接压缩,然后将压缩数据输出至文件
*           如果数组的长度小于4096,则将数组压缩放置在缓存数组中,当缓存数组长度大于等于4096,
*           再将压缩数据输出至文件
*           如果当数组轮询完成,且仍有数据未被写入文件,则直接将数据输出
*  @param   pSrcArr ：双重指针数组，指向需要压缩数据数组的头指针
*           arrayLenArr：　数组长度的数组
*           dir:   存储文件目录的指针
*           ａrrayCnt: 需要进行压缩的数组的数量
*           blockSize: 写入文件数组的最小区块大小
*           pPrefix : 文件类型的指针（元件类型如＂chip＂）
*  @return  N/A
*/
//压缩数据,并将压缩的数据写入文件
void compressDataAndExportToFile(float ** pSrcArr,
                                 int arrayLenArr[],
                                 char * dir,
                                 int arrayCnt,
                                 char * pPrefix);

/*
*  @brief  读取指定目录的文件,将文件数据存入到内存中,再通过表头数据解压数据
*  @param  pDestArr: 双重浮点型指针数组，指向数据解压后存放数组的头指针
*          dir :  文件目录 char 型指针,指向需要读取数据的文件目录
*  @return N/A
*/
void importDataFromFileAndDecompress(float ** pDestArr, char * dir);

//<<<----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // IMAGECOMPRESSION_H

