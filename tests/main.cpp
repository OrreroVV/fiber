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
        std::cout << "set<T>" << std::endl;
        
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

template<typename T>
class LexicalCast<std::string, T> {
public:
    T operator()(const std::string& input) {
        T result;
        std::cout << "T" << std::endl;
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


void test_lexical() {
    Config<std::string> a;

    a.fromString("abc");


}

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

给你一个整数数组 nums 和一个 正整数 k 。

请你统计有多少满足 「 nums 中的 最大 元素」至少出现 k 次的子数组，并返回满足这一条件的子数组的数目。

子数组是数组中的一个连续元素序列。

 

示例 1：

输入：nums = [1,3,2,3,3], k = 2
输出：6
解释：包含元素 3 至少 2 次的子数组为：[1,3,2,3]、[1,3,2,3,3]、[3,2,3]、[3,2,3,3]、[2,3,3] 和 [3,3] 。
示例 2：

输入：nums = [1,4,2,1], k = 3
输出：0
解释：没有子数组包含元素 4 至少 3 次。
 

提示：

1 <= nums.length <= 105
1 <= nums[i] <= 106
1 <= k <= 105
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




//test condition_variable

class CP {

public:

    void start() {
        m_threads.emplace_back(std::thread(std::bind(&CP::product, this)));
        m_threads.emplace_back(std::thread(std::bind(&CP::product, this)));
        m_threads.emplace_back(std::thread(std::bind(&CP::product, this)));
        m_threads.emplace_back(std::thread(std::bind(&CP::consumer, this)));
        m_threads.emplace_back(std::thread(std::bind(&CP::consumer, this)));
        m_threads.emplace_back(std::thread(std::bind(&CP::consumer, this)));
        m_threads.emplace_back(std::thread(std::bind(&CP::consumer, this)));
    }

    void consumer() {
        
        while (!m_stop) {
            {
                std::unique_lock<std::mutex>lock(m_qMutex);
                m_condition.wait(lock, [this]()->bool{ return q.size() || m_stop.load();});
            }

            if (m_stop.load()) {
                break;
            }
            std::unique_lock<std::mutex>lock(m_qMutex);
            auto t = q.front();
            q.pop();
            std::cout << "consumer: " << t << std::endl;
        }
    }

    void product() {
        while (!m_stop) {

            if (m_stop.load()) {
                break;
            }

            if (cnt > 1000) {
                break;
            }

            {
                std::unique_lock<std::mutex>lock(m_qMutex);
                std::cout << "product: " << ++cnt << std::endl;
                q.push(cnt);
                m_condition.notify_one();
            }
            usleep(100000);
        }
    }




private:
    int cnt = { 0 };
    std::atomic<bool>m_stop = { false };
    std::vector<std::thread> m_threads;
    std::mutex m_cMutex;
    std::mutex m_pMutex;
    std::mutex m_qMutex; //queue
    std::queue<int>q;
    std::condition_variable m_condition;
};

void test_cp() {
    CP cp;
    cp.start();
    while (true) {
        sleep(100);
    }
}

//test5

template<typename T> 
void func_demo(T&& param) {

}

void test5() {


    func_demo(10);
    int a = 5;
    func_demo(a);
    func_demo(&a);
    int* b = &a;
    func_demo(std::move(a));



    int* ptr = new int[10];
    std::shared_ptr<int> p1(ptr, [](int* p)->void{delete []p;});
    std::shared_ptr<int> p2(p1);
    std::cout << p1.use_count() << " " << p2.use_count() << std::endl;


    

}

//test5 end


// test6 start


void t_close(int fd) {
    std::cout << "close: " << fd << std::endl;
}


void xxx(int a, int b) {
    
}

void test6() {
    void(*a)(int, int) = xxx;
    a(1, 2);


    // ThreadTool t;
    // t.start();

    // int a = 10;
    // t.enqueue(t_close, a);


    // t.stop();



    // a(1, 2);

}

// test6 end



void test_pipe() {
    int fd[2] = {};

    if (pipe(fd) == -1) {
        perror("pipe");
        return;
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        return;
    }

    if (!pid) {
        close(fd[1]);
        char buf[128] = {};
        
        read(fd[0], buf, sizeof(buf) - 1);
        std::cout << buf << std::endl;
        close(fd[0]);

    } else {
        std::string buf = "hello world";
        close(fd[0]);

        write(fd[1], buf.c_str(), sizeof(buf));
        close(fd[1]);
    }

}

int main() {
    // test_cp();
    // test4();
    // test_lexical();
    test_pipe();
    return 0;
}