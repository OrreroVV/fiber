#ifndef __NONCOPYABLE_H__
#define __NONCOPYABLE_H__

/***
 * @Author: Orrero
 * @Date: 2024-12-16
 * @Description: 
 * @
 * @Copyright (c) 2024 by Orrero, All Rights Reserved.
 */

namespace hzh{

class Noncopyable {
public:
    Noncopyable() = default;
    ~Noncopyable() = default;
    Noncopyable(const Noncopyable&) = delete;
    Noncopyable& operator=(const Noncopyable&) = delete;
};


}

#endif // __NONCOPYABLE_H__