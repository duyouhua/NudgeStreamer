/*
 * 视频编码模块的实现类，开始线程时将会获取
 * 最新的网络质量参数，根据最新的网络质量参
 * 数在编码池中获取对应码率的编码对象；该线
 * 程在隔一段时间就会重新获取最新的网络质量
 * 参数，对比网络质量是有偏差较大，获取编码
 * 对象编号之后与当前的手持的编码对象编号是
 * 否一致，不一致则在编码池中获取相应码率的
 * 编码对象进行编码
 */

#ifndef _AV_ENCODE_H
#define _AV_ENCODE_H

#include "VideoEncode.h"

namespace Encode
{
    class AvEncode : public VideoEncode
    {
    public:
        AvEncode(const int& width, const int& height);
        ~AvEncode();

    public:
        bool Initial();

    protected:
        void run();

    private:
        int GetEncoderID();// 获取编码ID

    private:
        // DEFAULT_TIME毫秒之后从消息队列中取出网络质量参数，对比是否需要修改编码码率
        const int DEFAULT_TIME;
        const int DEFAULT_WIDTH;
        const int DEFAULT_HEIGHT;

    private:
        // 当前手持的编码类编号，编号范围为0-3，数字越小代表网络质量越好，故码率也越大
        int m_current_encoder_id;
    };
}

#endif // _AV_ENCODE_H
