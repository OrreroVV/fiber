#include "hzh/iomanager.h"
#include <memory.h>
#include <iostream>
#include <functional>

class A : public std::enable_shared_from_this<A>{
public:

public:
    virtual void test() {
        std::cerr << "A TEST" << std::endl;
    };

public:
    void func() {
        std::function<void()> s = [this]() {
            A::test();
        };
        s();
    }
};

class B : public A, public std::enable_shared_from_this<B> {

public:
    void test() override {
        std::cerr << "B TEST" << std::endl;

    }

    void f1() {
        std::function<void()> s = [this]() {
            A::test();
        };
        s();
    }

};

int main() {
    B* b = new B;
    b->func();

    return 0;
}
