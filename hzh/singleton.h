#ifndef __SINGLETON_H__
#define __SINGLETON_H__

/***
 * @author: Orrero
 * @date: 2024-12-27
 * @brief: 
 * 
 * @copyright (c) 2024 by Orrero, All Rights Reserved.
 */

#include <memory>

namespace hzh {

template<class T, class X = void, int N = 0>
class Singleton {
public:
    static T* getInstance() {
        static T a;
        return &a;
    }
};

template<class T, class X = void, int N = 0>
class SingletonPtr {
public:
    static std::shared_ptr<T> getInstance() {
        static std::shared_ptr<T> v(new T);
        return v;
    }

};

}

#endif // __SINGLETON_H__