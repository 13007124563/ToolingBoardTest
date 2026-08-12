/*
 *  ByteOrderUtil.h
 *
 *  Created on: Nov 15, 20xx
 *  Author: wanshi
 */

#ifndef BYTEORDERUTIL_H_
#define BYTEORDERUTIL_H_

#include <stdint.h>

namespace ByteOrder
{
const uint16_t us_flag = 1;
// little_end_flag 表示主机字节序是否小端字节序
const bool little_end_flag = *((uint8_t *)&us_flag) == 1;

//小端到主机
template <typename T>
T le_to_host(T &from)
{
    T to;
    uint8_t byteLen = sizeof(T);

    if (little_end_flag)
    {
        return from;
    }
    else
    {
        char *to_char = (char *)&to;
        char *from_char = (char *)&from;
        for (int i = 0; i < byteLen; i++)
        {
            to_char[i] = from_char[byteLen - i - 1];
            //此处也可用移位操作来实现
        }
        return to;
    }
}

//主机到小端
template <typename T>
T host_to_le(T &from)
{
    return le_to_host(from);
}

//大端到主机
template <typename T>
T be_to_host(T &from)
{
    T to;
    uint8_t byteLen = sizeof(T);
    if (!little_end_flag)
    {
        return from;
    }
    else
    {
        char *to_char = (char *)&to;
        char *from_char = (char *)&from;
        for (int i = 0; i < byteLen; i++)
        {
            to_char[i] = from_char[byteLen - i - 1];
            //此处也可用移位操作来实现
        }
        return to;
    }
}

//主机到大端
template <typename T>
T host_to_be(T &from)
{
    return be_to_host(from);
}

} // namespace ByteOrder

#endif /* BYTEORDERUTIL_H_ */