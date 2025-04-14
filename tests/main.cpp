#include <bits/stdc++.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <utility>
#include "hzh/ThreadTool.h"

using namespace std;

int get(double x) {
    int y = x;
    return y;
}

void cset(int cmd, ...) {
    va_list args;
    va_start(args, cmd);

    char* str = va_arg(args, char*); 
    double x = va_arg(args, double);
    printf("%s\n", str);
    va_end(args);
    int tmp = get(x);
}

template<typename _type>
struct _reference {
    typedef _type type;
};

template<typename _type>
struct _reference<_type&> {
    typedef _type type;
};
template<typename _type>
struct _reference<_type&&> {
    typedef _type type;
};



// 左值, 左值引用
template<typename T>
T&& __forward(typename _reference<T>::type& t) {
    return static_cast<T&&>(t);
}

template<typename T>
T&& __forward(typename _reference<T>::type&& t) {
    return static_cast<T&&>(t);
}

template<typename T>
typename _reference<T>::type&& __move(T&& t) {
    return static_cast<typename _reference<T>::type&&>(t);
}

#include<stdio.h>
 


class MyClass {
public:
    std::string data;
    int x;

    MyClass(const std::string& str, int x) : data(str), x(x) {
        std::cout << "Constructor: " << data << std::endl;
    }

    MyClass(const MyClass& other) : data(other.data) {
        std::cout << "Copy Constructor: " << data << std::endl;
    }

    ~MyClass() {
        std::cout << "Destructor: " << data << std::endl;
    }
};
    
MyClass createObject() {
    MyClass temp("Temp Object", 2);
    std::cout << &temp << std::endl;
    return temp;
}







template<typename S, typename T>
class LexicalCast {

public:
    T operator()(const S& v) {
        return static_cast<T>(v);
    }

};

template<typename T>
class LexicalCast<std::string, std::set<T>>{
public:

    std::set<T> operator()(const std::string& input) {
        std::set<T> result;
        std::istringstream iss(input);
        T value;
        
        std::string token;
        while (std::getline(iss, token, ',')) {
            std::istringstream token_stream(token);
            if (token_stream >> value) {
                result.insert(value);
            }
        }
        return result;
    }
};


template<typename T, typename From=LexicalCast<std::string, std::set<T>>, typename To=LexicalCast<std::set<T>, std::string>>
class Config {
public:
    void fromString(std::string x) {
        std::set<T> res = From()(x);
        std::cout << "to set: ";
        for (auto i : res) {
            cout << i;
        }
        cout << endl;
    }
};


template<typename, typename...>
class Function;


template<typename... Args>
class Function<void, Args...> {

public:

    Function(std::function<void(Args...)> t) {
        a = t;
    }

    void cb(Args... s) {
        a(s...);
    }

    std::function<void(Args...)> a;
};

template<typename R, typename... Args>
class Function {

public:

    Function(std::function<R(Args...)> t) {
        a = t;
    }

    R cb(Args... s) {
        R tmp = a(s...);
        return tmp;
    }

    R operator()(Args&&... args) {
        // R tmp = a(std::forward<Args>(args)...);
        R tmp = a(args...);
        return tmp;
    }

    std::function<R(Args...)> a;
};



struct Multiplier {
    int factor;
    Multiplier(int f) : factor(f) {
        std::cout << "构造" << std::endl;
    }

    Multiplier (const Multiplier& s) {
        factor = s.factor;
        std::cout << "拷贝构造" << std::endl;
    }

    ~Multiplier() {
        std::cout << "析构" << std::endl;
    }

    int operator()(int x) const { return x * factor; }
};



/*

给你一个下标从 0 开始、长度为 n 的二进制字符串 s ，你可以对其执行两种操作：

选中一个下标 i 并且反转从下标 0 到下标 i（包括下标 0 和下标 i ）的所有字符，成本为 i + 1 。
选中一个下标 i 并且反转从下标 i 到下标 n - 1（包括下标 i 和下标 n - 1 ）的所有字符，成本为 n - i 。
返回使字符串内所有字符 相等 需要的 最小成本 。

反转 字符意味着：如果原来的值是 '0' ，则反转后值变为 '1' ，反之亦然。

 
1001




示例 1：
1 + 2
2 + 1
输入：s = "1011"
输出：2
解释：执行第二种操作，选中下标 i = 2 ，可以得到 s = "0000" ，成本为 2 。可以证明 2 是使所有字符相等的最小成本。
示例 2：


111010111
111011111
6 + 5 + 4 + 3

4 + 4 + 3 + 3

2 + 1 + 3 + 2 + 1
101010

输入：s = "010101"
输出：9
解释：执行第一种操作，选中下标 i = 2 ，可以得到 s = "101  101" ，成本为 3 。
执行第一种操作，选中下标 i = 1 ，可以得到 s = "011101" ，成本为 2 。
执行第一种操作，选中下标 i = 0 ，可以得到 s = "111101" ，成本为 1 。
执行第二种操作，选中下标 i = 4 ，可以得到 s = "111110" ，成本为 2 。
执行第二种操作，选中下标 i = 5 ，可以得到 s = "111111" ，成本为 1 。
使所有字符相等的总成本等于 9 。可以证明 9 是使所有字符相等的最小成本。 
 

提示：

1 <= s.length == n <= 105
s[i] 为 '0' 或 '1'


10001001
4 + 
*/

class Solution {
public:
    long long minimumCost(string s) {
        int mid = s.size() / 2;
        int cnt = 0;
        for (int i = mid; i >= 0;) {
            int j = i;
            while (j >= 0 && s[j] == s[i]) {
                j--;
            }
            cnt += j + 1;
            i = j;
        }

        for (int i = mid; i < s.size();) {
            int j = i;
            while (j < s.size() && s[j] == s[i]) {
                j++;
            }
            cnt += s.size() - j;
            i = j;
        }
        return cnt;
    }
};



void mem_() {
    const char* name = "/my_shm";
    size_t size = 1024;

    int fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    ftruncate(fd, size);

    void* ptr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    
    strcpy((char*)ptr, "Hello, Shared Memory!");

    std::cout << "Written to shared memory: " << (char*)ptr << std::endl;

    munmap(ptr, size);
    close(fd);
    shm_unlink(name);
}

class A {

public:
    A() {}

    static int get(int x) { return x; }

};


class B {

public:
    B() {}

    static std::string get(std::string x) { return x; }

};


template<typename T, typename C>
class Func;

template<typename C>
class Func<int, C> {
public:
    static void get(int x) {
        std::cout << "i am a int: " << C::get(x) << std::endl;
    }
};

template<typename C>
class Func<std::string, C> {
public:
    static void get(std::string x) {
        std::cout << C::get(x) << std::endl;
    }
};


template<typename T>
class Ve {

public:
    void func(T& container) {
        m_it = container.begin();
        std::cout << *m_it << std::endl;
    }

    decltype(T().begin()) m_it;
};

typedef void (*func_t)(int, int);


void f(int a, int b) {
    std::cout << a + b << std::endl;
}

void test() {

    void(*t)(int,int);
    t = &*f;

    t(1, 2);

    Func<std::string, B>::get("sss");
    Func<int, A>::get(1);
    if (std::vector<int>().begin() == std::vector<int>().end()) {
        std::cout << "null" << std::endl;
    }
    decltype(std::vector<int>().begin()) m;
    std::vector<int>a = { 1, 2 };

    Ve<std::vector<int>> v1;
    v1.func(a);

    const std::vector<int> vec = a;
    Ve<const std::vector<int>> v;
    v.func(vec);

    m = a.begin();
    std::cout << *m << std::endl;
}

/* test1 */


int get() {
    return 0;
}

template<typename T, typename... S>
int get(T t, S... s) {
    std::cout << t << std::endl;
    return get(s...) + 1;
}

template<typename T, typename... D>
class TMP {
public:

    int fun(D... s) {
        int cnt = get(s...);
        return cnt;
    }

    TMP(T t, D... s) {
        y = std::bind(&TMP<int, D...>::fun, this, s...);
        int tmp = y(std::forward<D>(s)...);
        std::cout << "cnt: " << tmp << std::endl;
    }

    typedef T(*func)(D...);
    func x;
    std::function<int(D...)> y;
};

template<typename... D>
class TMP<void, D...> {
public:



    TMP(D... s) {
        y = std::bind(&TMP<void, D...>::fun, this, std::forward<D>(s)...);
        std::cout << "cnt: " << y(std::forward<D>(s)...) << std::endl;
    }
    
    int fun(D... s) {
        int cnt = get(s...);
        return cnt;
    }

    typedef void(*func)(D...);
    func x;
    std::function<int(D...)> y;
};

void test1() {
    int n = 10;
    TMP<void, int, int, int&> t(2, n, n);
}


/* test2 */


class Foo
{
public:
    Foo() {
        std::cout << "default construct" << std::endl;
    }
    Foo(int) {
        
        std::cout << "1 param construct" << std::endl;
    }
    Foo& operator=(const Foo& x) {
        std::cout << "operator = " << std::endl;
        y = x.y;
        return *this;
    }

    Foo(const Foo &) {
        
        std::cout << "copy construct" << std::endl;
    };
private:
    
    int y;
};


template<typename _Tp>
constexpr _Tp&&
_forward(_Tp& __t) noexcept
{
  return static_cast<_Tp&&>(__t);
}

template<typename _Tp>
constexpr _Tp&&
_forward(typename std::remove_reference<_Tp>::type& __t) noexcept {
    return static_cast<_Tp&&>(__t);
}

template<typename _Tp>
constexpr _Tp&&
_forward(typename std::remove_reference<_Tp>::type&& __t) noexcept
{
  return static_cast<_Tp&&>(__t);
}

void otherdef(int & t) {
    cout << "lvalue\n";
}
void otherdef(const int & t) {
    cout << "rvalue\n";
}

template <typename T>
void ff(T&& t) {
    // otherdef(std::forward<T>(t));
    otherdef(_forward<T&>(t));

    // otherdef(_forward<T&&>(10));
    // otherdef(t);
}





void test2() {
    int n = 10;
    int& num = n;
    int&& x = std::forward<int>(10);
    ff<int>(std::move(num));
    int&& num2 = 11;
    ff<int&&>(std::move(num2));

    Foo a1(123);
    Foo a2;  //error: 'Foo::Foo(const Foo &)' is private
    a2 = a1;
    Foo a3 = { 123 };
    Foo a4 { 123 };
    int a5 = { 3 };
    int a6 { 3 };
}


// test4


class Demo{
public:
    Demo():num(new int(0)){
        cout<<"construct!"<<endl;
    }

    Demo(Demo &d):num(new int(*d.num)){
        cout<<"lvalue construct!"<<endl;
    }
    //添加移动构造函数
    // Demo(Demo &&d):num(d.num){
    //     d.num = nullptr;
    //     cout<<"move construct!"<<endl;
    // }
    ~Demo(){
        if (num) {
            delete num;
        }
        cout<<"class destruct!"<<endl;
    }
private:
    int *num;
};

Demo get_demo() {
    Demo demo;
    return demo;
}

void test_func(int& x) {
    std::cout << "ff: " << x << std::endl;
}

// Demo get_demo() {
//     return Demo();
// }

void func(const int& x) {
    std::cout << "const lvalue" << x << std::endl;
}

void func(const int* x) {
    int num = 10;
    x = &num;
    num = 5;
    std::cout << "const int* " << &num << std::endl;
}

void func(int* const x) {
    int* num = x;
    *x = 10;
    std::cout << "int* const " << num << " " << &num << " " << *num << std::endl;
}

void func(int&& x) {
    std::cout << "rvalue: " << x << std::endl;
}

// int getFunction(int(*a)(const char*, ...), int x) {
//     a("%d\n", 123);
// }

template<typename A, typename... Args>
A (*getFunction(A(*a)(Args...)))(Args...) {
    return a;
}

// typedef int(*funcType)(const char*, ...);

// funcType getFunction() {
//     return &printf;
// }

int tes(const char* y, int x) {
    std::cout << x << std::endl;
    return 1;
}

void ss() {
    std::cout << "ss" << std::endl;
}

void test4() {
    // int(*f)(const char*, ...) = printf;
    // f("%d\n", 123);
    getFunction<int, const char*, int>(tes)("%d\n", 111);
    getFunction<void>(ss)();
    // getFunction(getFunction(printf), 1);

    // test_func(3);
    get_demo();
    // Demo demo = get_demo();
    // std::cout << "---------" << std::endl;
    // Demo d = demo;
    // func(10);

    // const int* x;
    // func(x);


}

void testThead() {
    ThreadTool t;
    
}

int main() {
    // test4();
    return 0;
}