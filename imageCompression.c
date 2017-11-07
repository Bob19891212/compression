#include "imagecompression.h"

int findMinFromArr(float srcArr[],int length)
{
    float min = srcArr[0];            //将数组的第一个元素赋值给最小值
    for (int i = 1; i < length; ++i)  //轮询数组中元素,分别将数组中每个元素与最小值比较
    {
        if( min > srcArr[i])          //如果数组中的元素比最小值小
            min = srcArr[i];          //将此元素的值赋值给最小值
    }
    return (int)min;                  //将浮点型最小值转换为int型返回
}

int findMaxFromArr(float srcArr[],int length)
{
    float max = srcArr[0];            //将数组的第一个元素赋值给最大值
    for (int i = 1; i < length; ++i)  //轮询数组中元素,分别将数组中每个元素与最大值比较
    {
        if( max < srcArr[i])          //如果数组中的元素比最小值大
            max = srcArr[i];          //将此元素的值赋值给最大值
    }
    return (int)max;                  //将浮点型最大值转换为int型返回
}

void mapFloatArrToByteArr(int max,
                          int min,
                          float *srcArr,
                          unsigned char destArr[],
                          int arrayLen)
{
    int tempMapValue;                       //临时映射值,将数组中浮点型数据转换成整型

    //判断最大值与最小值是否相等,如果相等则程序错误
    if ( max == min )
    {
        printf("数组的最大值与最小值相等,无法压缩数据!");
    }
    else
    {
        //轮询数组中每个元素,将其映射到Byte型数组中
        for (int i = 0; i < arrayLen; ++i)
        {
            tempMapValue = (int)(srcArr[i]);    //将数组中的浮点型数据转换成整型
            //通过公式计算出数据映射后的结果,存入目标数组中
            destArr[i] = (unsigned char)
                         (BYTE_MAX_VALUE * (tempMapValue -min) / (max - min));
        }
    }

}


void compressArray(float srcArr[],
                   unsigned char destArr[],
                   int length)
{
    int max = findMaxFromArr(srcArr,length);  //获取数组中的最大值
    int min = findMinFromArr(srcArr,length);  //获取数组中的最小值

    //将数组的长度除以压缩系数取值(压缩系数:100,使结果在0-255范围内,下同),存入Byte数组中
    destArr[0] = (unsigned char)(length / RATIO_FACTOR);
    //将数组的长度除以压缩系数取余数,存入Byte数组中
    destArr[1] = (unsigned char)(length % RATIO_FACTOR);
    destArr[2] = (unsigned char)(max / RATIO_FACTOR); //将最大值压缩至Byte数组
    destArr[3] = (unsigned char)(max % RATIO_FACTOR);
    destArr[4] = (unsigned char)(min / RATIO_FACTOR); //将最小值压缩至Byte数组
    destArr[5] = (unsigned char)(min % RATIO_FACTOR);

    //根据最大值,最小值,将浮点型数组元素映射到Byte数组中
    mapFloatArrToByteArr(max,min,srcArr,&destArr[HEADER_LEN],length);
}


void deleteFile(char * dir)
{
    char filePath[NAME_CHAR_CNT];            //存放文件路径的byte数组

    DIR * pDir = opendir(dir);              //文件目录指针(获取文件目录打开状态)
    struct dirent * file;                   //定义结构体文件指针(获取文件名字)

    while ( (file=readdir(pDir)) != NULL )  //判断文件名不为NULL,则删除文件
    {
        // 忽略 目录下的"." 和 ".."影藏目录
        if ( 0 == strcmp(file->d_name,".")|| 0 == strcmp(file->d_name,"..") )
        {
            continue;
        }

        sprintf(filePath, "%s%s", dir, file->d_name);   //拼接文件的路径

        remove(filePath);                               // 删除文件
    }
    closedir(pDir);                                     //关闭文件目录
}

void compressDataAndExportToFile(float ** pSrcArr,
                                 int arrayLenArr[],
                                 char * dir,
                                 int arrayCnt,
                                 char * pPrefix)
{
    if(access(dir, 0) == 0)    //判断文件目录是否存在,如果存在,删除目录中的所有文件
    {
        deleteFile(dir);
    }
    else                                                //如果目录不存在则创建目录
    {
        mkdir( dir, 0777);                              //生成存放压缩数据的目录
    }

    char filePath[NAME_CHAR_CNT];                       //生成压缩数据的路径
    char fileName[NAME_CHAR_CNT];                       //输出数据至文件的文件名

    unsigned char cacheSingleArr[LENGTH];               //缓存压缩单个数组的数组
    unsigned char cacheMultipleArrs[LENGTH];            //缓存压缩多个数组的数组

    char cacheArrIdx[4];                                //记录存放多个数组的索引号
    int cacheArrCnt = 0;                                //记录缓存数组的数量
    int cacheArrLen = 0;                                //缓存数组的长度
    int multipleArrsLen = 0;                            //多个数组的总长度

    clock_t startTime, finishTime;                  //声明变量程序开始时间与结束时间
    double compressionTime;                         //声明变量程序压缩时间

    startTime = clock();                            //获取压缩开始时间

    //依次轮询每个数组,如果数组长度大于等于4096,则直接压缩
    //如果数组长度小于4096,则将多个数组拼接至长度大于等于4096后,再进行压缩
    for (int i = 0; i < arrayCnt; ++i)
    {
        //>>>----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        //step1
        //如果长度大于等于 blockSize(单个区块大小为4096)
        //将数组数据压缩到存放单个数组的Byte数组中,将数据写入指定路径的文件
        if ( arrayLenArr[i] >= BLOCK_SIZE)
        {
            cacheSingleArr[0] = 1;                 //缓存数组的数量
            //调用压缩数组函数,将浮点型数据压缩至存放单个数组的 Byte 数组中(包括表头)
            compressArray( pSrcArr[i],&cacheSingleArr[1], arrayLenArr[i]);
            //获取输出至文件的文件路径
            sprintf(filePath,"%s%s%03d",dir,pPrefix,i);
            //获取输出至文件的数组的长度
            cacheArrLen = arrayLenArr[i] + HEADER_LEN + 1;
            //将数据输出至文件
            exportByteToFile( filePath, cacheSingleArr,cacheArrLen);
        }
        //<<<----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

        //>>>----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        //step2
        //如果长度小于 blockSize,先将数组数据压缩到存放多个数组的Byte数组中
        //将压缩到Byte数组中的索引号拼接到fileName的数组中
        //如果存放多个数组的 Byte 数组的长度大于等于 blockSize,将数据写入指定路径的文件
        else
        {       
            //调用压缩数组函数,将浮点型数据压缩至存放多个数组的 Byte 数组中(包括表头)
            compressArray(pSrcArr[i],
            &cacheMultipleArrs[1 + multipleArrsLen + HEADER_LEN * cacheArrCnt],
                         arrayLenArr[i]);

            //记录文件名,如果存在多个,则将文件名字拼接,如"chipxxx_chipxxx"
            if (cacheArrCnt == 0)  //如果是缓存多个数组的第一个数组,则文件名为"chipxxx"
            {
                //获取缓存数组的索引号,存放在缓存数组索引字符串的数组中
                sprintf(cacheArrIdx,"%s%03d",pPrefix,i);
                //将字符串 cacheArrIdx 复制给字符数组fileName
                strcpy(fileName,cacheArrIdx);
            }
            else  //否则将文件名前加"_",再拼接到之前的文件名中,如"chipxxx_chipxxx"
            {
                sprintf(cacheArrIdx,"_%s%03d",pPrefix,i);
                //将字符数组 cacheArrIdx 追加到字符数组 fileName 字符串的结尾
                strcat(fileName,cacheArrIdx);
            }

            multipleArrsLen += arrayLenArr[i];  //统计缓存数组的总长度
            ++cacheArrCnt;                      //统计缓存在数组中的数量
            cacheMultipleArrs[0] = cacheArrCnt; //将缓存数组的数量存放在压缩数组的首位

            //如果存放多个数组的累计长度大于等于 blockSize,则直接进行压缩
            if(multipleArrsLen >= BLOCK_SIZE)
            {
                //更新输出文件路径的字符串
                sprintf(filePath,"%s%s",dir,fileName);
                //记录输出至数组的长度
                cacheArrLen =  multipleArrsLen + cacheArrCnt * HEADER_LEN + 1;
                //调用向文件写入数据函数
                exportByteToFile(filePath, cacheMultipleArrs,cacheArrLen);

                multipleArrsLen = 0;            //将存放多个数组长度重置为0
                cacheArrCnt = 0;                //将缓存文件的数量重置为0
            }
        }
        //<<<----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

        //>>>----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        //step3
        //如果轮询至数组最后一个,且仍有被压缩的数组没写入文件,则直接将数据写入文件
        if (i == (arrayCnt - 1) && multipleArrsLen != 0)
        {
            //更新输出文件路径的字符串
            sprintf(filePath,"%s%s",dir,fileName);
            //记录输出至数组的长度
            cacheArrLen = multipleArrsLen + HEADER_LEN + 1;
            //调用向文件写入数据函数
            exportByteToFile( filePath, cacheMultipleArrs, cacheArrLen);
        }

        //<<<----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    }

    finishTime = clock();                   //获取压缩结束时间
    //计算压缩时间,并在终端上显示
    compressionTime = (double)(finishTime - startTime) / CLOCKS_PER_SEC;
    printf("数据压缩的时间为：%fs\n",compressionTime);
}

