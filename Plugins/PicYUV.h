/*
 * 自定义YUV图片格式的结构体，截屏后将基本信
 * 息存入该结构体，提供给编码的对象进行操作，
 * 可存放几种不同格式
 */

#ifndef _PIC_YUV_H
#define _PIC_YUV_H

#include <cstdint>

struct PicYUV
{
    static const int NUM_DATA_POINTERS = 3;// 图像裸数据数组数，默认为3
    int width;// 图像的宽
    int height;// 图像的高
    int linesize[NUM_DATA_POINTERS];// 行的长度
    uint8_t *data[NUM_DATA_POINTERS];// 指向图像数据

    PicYUV()
    {
        for (int i = 0; i < NUM_DATA_POINTERS; ++i)
            data[i] = NULL;
    }

    PicYUV(const int& width, const int& height)
    {
        this->width = width;
        this->height = height;

        int size = width * height;
        data[0] = new uint8_t[size];
        data[1] = new uint8_t[size / 4];
        data[2] = new uint8_t[size / 4];
    }

    ~PicYUV()
    {
        for (int i = 0; i < NUM_DATA_POINTERS; ++i)
        {
            delete data[i];
            data[i] = NULL;
        }
    }
};

#endif // _PIC_YUV_H
