#include "zMultiTest.h"

#include <iostream>
#include <thread>

using std::cout;
using std::endl;
using std::thread;
using std::this_thread::get_id;

class Functor
{
  public:
    int nData;
    Functor() : nData(0) { cout << "1.X Ctor\t Data\t" << nData << "\tId " << get_id() << endl; };
    Functor(const int &n) : nData(n) { cout << "1.X Trans Ctor\t Data\t" << nData << "\tId " << get_id() << endl; };
    Functor(const Functor &c) : nData(c.nData) { cout << "1.X Copy Ctor\t Data\t" << nData << "\tId " << get_id() << endl; };
    ~Functor() { cout << "1.X Dctor\t Data\t" << nData << "\tId " << get_id() << endl; };
    void operator()() { cout << "1.2 Functor\t Data\t" << nData << "\tId " << get_id() << endl; }
    void MemberFunction(const int &n)
    {
        nData = n;
        cout << "1.4 MemFunc\t Data\t" << nData << "\tId " << get_id() << endl;
    }
};
void Funtion()
{
    cout << "1.1 Function" << endl;
}
void zthread::test_Creation()
{
    cout << "1.1 Create by Function" << endl;
    thread objThred1(Funtion);
    objThred1.join();
    cout << "1.2 Create by Functor" << endl;
    thread objThred2(Functor(12));
    objThred2.join();
    cout << "1.3 Create by Lambda" << endl;
    auto LambdaTH = [] {
        cout << "1.3 Lambda Start" << endl;
    };
    thread objThred3(LambdaTH);
    objThred3.join();
    cout << "1.4 Create by MemberFunction" << endl;
    Functor objFunctor(0);
    thread objThred4(&Functor::MemberFunction, objFunctor, 14);
    objThred4.join();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------

using std::unique_ptr;

void FunctionPassbyFalseRef(const int &nNum, char *cStr)
{
    cout << "2.1 ThreadNum\t Addr\t" << &nNum << "\tData\t" << nNum << "\tId " << get_id() << endl;
    cout << "2.1 ThreadArry\t Addr\t" << &cStr << "\tData\t" << cStr << "\tId " << get_id() << endl;
}
class CArgMutable
{
  public:
    mutable int nData;
    CArgMutable() : nData(0) { cout << "2.2 Ctor\t Addr\t" << this << "\tData\t" << nData << "\tId " << get_id() << endl; };
    CArgMutable(int n) : nData(n) { cout << "2.2 Param Ctor\t Addr\t" << this << "\tData\t" << nData << "\tId " << get_id() << endl; };
    CArgMutable(const CArgMutable &a) : nData(a.nData) { cout << "2.2 Copy Ctor\t Addr\t" << this << "\tData\t" << nData << "\tId " << get_id() << endl; };
    ~CArgMutable() { cout << "2.2 Dctor\t Addr\t" << this << "\tData\t" << nData << "\tId " << get_id() << endl; };
};
void FunctionPassbyObj(const CArgMutable &c)
{
    cout << "2.2 ConstObj\t Addr\t" << &c << "\tData\t" << ++c.nData << "\tId " << get_id() << endl;
}
class CArgUnMut
{
  public:
    int nData;
    CArgUnMut() : nData(0) { cout << "2.3 Ctor\t Addr\t" << this << "\tData\t" << nData << "\tId " << get_id() << endl; };
    CArgUnMut(int n) : nData(n) { cout << "2.3 Param Ctor\t Addr\t" << this << "\tData\t" << nData << "\tId " << get_id() << endl; };
    CArgUnMut(const CArgUnMut &a) : nData(a.nData) { cout << "2.3 Copy Ctor\t Addr\t" << this << "\tData\t" << nData << "\tId " << get_id() << endl; };
    ~CArgUnMut() { cout << "2.3 Dctor\t Addr\t" << this << "\tData\t" << nData << "\tId " << get_id() << endl; };
};
void FunctionPassbyRef(CArgUnMut &c)
{
    c.nData = 231;
    cout << "2.3 RefObj\t Addr\t" << &c << "\tData\t" << c.nData << "\tId " << get_id() << endl;
}
void FunctionPassbySmartPtr(unique_ptr<int> p)
{
    *p = 241;
    cout << "2.4 Unique Ptr\t Addr\t" << &p << "\tData\t" << *p << "\tId " << get_id() << endl;
}
void zthread::test_BugsOfDetach()
{
    cout << "2.1 Pass by False Reference" << endl;
    {
        int nNum = 0;
        char cStr[] = {"2.1"};
        cout << "2.1 MainNum\t Addr\t" << &nNum << "\tData\t" << nNum << "\tId " << get_id() << endl;
        cout << "2.1 MainArry\t Addr\t" << &cStr << "\tData\t" << cStr << "\tId " << get_id() << endl;
        thread objThred1(FunctionPassbyFalseRef, nNum, cStr); //1 ①引用传递参数(VS会进行一次拷贝 GNU C会进行两次拷贝) ②引用传递指针（）
        objThred1.join();
    }

    cout << "2.2 Pass Arguement by Parameterized Constructor" << endl;
    {
        CArgMutable objArgMutable(22);
        thread objThred2(FunctionPassbyObj, 22); //2 不要用隐式类型转换
        cout << "2.2 Main Thread\t Addr\t" << &objArgMutable << "\tData\t" << objArgMutable.nData << "\tId " << get_id() << endl;
        objThred2.join();
    }

    cout << "2.3 Pass Arguement by Reference" << endl;
    {
        CArgUnMut objArgUnMut(23);
        thread objThred3(FunctionPassbyRef, std::ref(objArgUnMut)); //3 通过引用传递的参数使用std::ref函数，线程接收参数是实际地址
        cout << "2.3 Main Thread\t Addr\t" << &objArgUnMut << "\tData\t" << objArgUnMut.nData << "\tId " << get_id() << endl;
        objThred3.join();
    }

    cout << "2.4 Pass Smart Pointer as Arguement" << endl;
    {
        unique_ptr<int> pNum(new int(24));
        cout << "2.4 Main Thread\t Addr\t" << &pNum << "\tData\t" << *pNum << "\tId " << get_id() << endl;
        thread objThred4(FunctionPassbySmartPtr, std::move(pNum));
        objThred4.join();
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------

#include <list>
#include <mutex>
#include <vector>

using std::list;
using std::lock;
using std::mutex;
using std::vector;

void FunctionThreads(int tid)
{
    cout << "3.1 Threads\t Id " << get_id() << "\tThreadId " << tid << endl;
}

void zthread::test_Threads()
{
    cout << "3.1 Push Threads to Vector" << endl;
    {
        size_t nNum = 10;
        vector<thread> vThreads;
        for (size_t i = 0; i < nNum; i++)
        {
            vThreads.push_back(thread(FunctionThreads, i));
        }
        for (auto itThread = vThreads.begin(); itThread != vThreads.end(); itThread++)
        {
            itThread->join();
        }
        cout << "3.1 Main Thread\t Id " << get_id() << endl;
    }
}

int nReadOnly = 1;
void FunctionDataR()
{
    cout << "4.1 Threads\t Read\t" << nReadOnly << "\tId " << get_id() << endl;
}

class CMsgQueue
{
  private:
    static list<int> MsgQueue;
    static mutex mutexMsgQueue1;
    static mutex mutexMsgQueue2;
    static long nSum;

  public:
    CMsgQueue(){};
    ~CMsgQueue(){};
    void Recv()
    {
        for (size_t i = 0; i < 100000; i++)
        {
            mutexMsgQueue1.lock();
            mutexMsgQueue2.lock();
            // std::chrono::milliseconds sleep5s(5000);
            // std::this_thread::sleep_for(sleep5s);
            cout << "4.2 PushDone\t Addr\t" << this << "\tData\t" << nSum++ << "\tId " << get_id() << endl;
            MsgQueue.push_back(i);
            mutexMsgQueue1.unlock();
            mutexMsgQueue2.unlock();
        }
    }
    void Ans()
    {
        for (size_t i = 0; i < 100000; i++)
        {
            int nData = 0;
            // mutexMsgQueue1.lock();
            // mutexMsgQueue2.lock();
            if (Pop_mutex(nData) == true)
                cout << "4.2 PopDone \t Addr\t" << this << "\tData\t" << nSum << "\tId " << get_id() << endl;
            else
                cout << "4.2 PopEmpty \t Addr\t" << this << "\tData\t" << nSum << "\tId " << get_id() << endl;
            // mutexMsgQueue1.unlock();
            // mutexMsgQueue2.unlock();
        }
    }
#define Z_MULTI_TEST_LOCK_TYPE 7
    // 1.lock_guard单独加锁(加索区域是其对象生命周期,由于加索顺序可能死锁)
    // 2.lock加锁,再由lock_guard接管(lock请求不成功释放已获得的资源)
    // 3.unique_lock默认使用跟lock_guard一致
    // 4.unique_lock使用std::adopt，mutex需要lock，跟lock_guard一致
    // 5.unique_lock使用std::try_to_lock，mutex不能lock
    // 6.unique_lock使用std::defer_lock，mutex不能lock，初始化了一个unlock的mutex，可通过try_lock尝试加锁
    // 7.unique_lock使用std::release，解绑unique_lock和mutex绑定关系，解绑unique_lock的mutex后有责任接管该mutex的lock和unlock操作
    bool Pop_mutex(int &n)
    {
#if Z_MULTI_TEST_LOCK_TYPE == 1
        std::lock_guard<mutex> lock_guardMsgQueue1(mutexMsgQueue1);
        std::lock_guard<mutex> lock_guardMsgQueue2(mutexMsgQueue2);
#elif Z_MULTI_TEST_LOCK_TYPE == 2
        lock(mutexMsgQueue1, mutexMsgQueue2);
        // std::lock访问互斥资源在请求不成功情况下不会保持请求成功的资源，请求不成功主动释放
        std::lock_guard<mutex>(mutexMsgQueue1, std::adopt_lock);
        std::lock_guard<mutex>(mutexMsgQueue2, std::adopt_lock);
#elif Z_MULTI_TEST_LOCK_TYPE == 3
        std::unique_lock<mutex> unique_lockMsgQueue1(mutexMsgQueue1);
        std::unique_lock<mutex> unique_lockMsgQueue2(mutexMsgQueue2);
#elif Z_MULTI_TEST_LOCK_TYPE == 4
        lock(mutexMsgQueue1, mutexMsgQueue2);
        std::unique_lock<mutex> unique_lockMsgQueue1(mutexMsgQueue1, std::adopt_lock);
        std::unique_lock<mutex> unique_lockMsgQueue2(mutexMsgQueue2, std::adopt_lock);
#elif Z_MULTI_TEST_LOCK_TYPE == 5
        std::unique_lock<mutex> unique_lockMsgQueue1(mutexMsgQueue1, std::try_to_lock);
        std::unique_lock<mutex> unique_lockMsgQueue2(mutexMsgQueue2, std::try_to_lock);
        if (!(unique_lockMsgQueue1.owns_lock() == true && unique_lockMsgQueue2.owns_lock() == true))
        {
            cout << "4.2 PopLck\t Addr\t" << this << "\tData\tERROR"
                 << "\tId " << get_id() << endl;
            return false;
        }
#elif Z_MULTI_TEST_LOCK_TYPE == 6
        std::unique_lock<mutex> defer_lockMsgQueue1(mutexMsgQueue1, std::defer_lock);
        std::unique_lock<mutex> defer_lockMsgQueue2(mutexMsgQueue1, std::defer_lock);
        defer_lockMsgQueue1.lock();
        if (true == defer_lockMsgQueue2.try_lock()) // 类似std::try_to_lock
        {
            cout << "4.2 PopLck\t Addr\t" << this << "\tData\tERROR"
                 << "\tId " << get_id() << endl;
            defer_lockMsgQueue1.unlock();
        }
#elif Z_MULTI_TEST_LOCK_TYPE == 7
        std::unique_lock<mutex> unique_lockMsgQueue1 = move_mutex(mutexMsgQueue1);
        std::unique_lock<mutex> unique_lockMsgQueue2(std::move(unique_lockMsgQueue1));
        mutex *pMutexMsgQueue2 = unique_lockMsgQueue2.release();
#endif
        nSum++;
        bool bRet = MsgQueue.empty();
        if (bRet != true)
        {
            n = MsgQueue.front();
            MsgQueue.pop_front();
        }
#if Z_MULTI_TEST_LOCK_TYPE == 7
        pMutexMsgQueue2->unlock();
#endif
        return !bRet;
    }
    std::unique_lock<mutex> move_mutex(mutex &m)
    {
        // 生成临时unique_lock对象，并调用unique_lock的移动构造函数
        return std::unique_lock<mutex>(m);
    }
};
list<int> CMsgQueue::MsgQueue;
mutex CMsgQueue::mutexMsgQueue1;
mutex CMsgQueue::mutexMsgQueue2;
long CMsgQueue::nSum = 0;

void zthread::test_DataSharing()
{
    cout << "4.1 Read Only" << endl;
    {
        size_t nNum = 10;
        vector<thread> vThreads;
        for (size_t i = 0; i < nNum; i++)
        {
            vThreads.push_back(thread(FunctionDataR));
        }
        for (auto itThread = vThreads.begin(); itThread != vThreads.end(); itThread++)
        {
            itThread->join();
        }
        cout << "4.1 Main Thread\t Id " << get_id() << endl;
    }
    cout << "4.2 Read and Write with Lock" << endl;
    {
        CMsgQueue objMsgQueue;
        thread thRecv(&CMsgQueue::Recv, objMsgQueue);
        thread thAns(&CMsgQueue::Ans, objMsgQueue);
        thRecv.join();
        thAns.join();
        cout << "4.2 Main Thread\t Id " << get_id() << endl;
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

class CSingleton
{
  private:
    CSingleton() {}
    static CSingleton *pSingleton;
  public:
    static CSingleton *getInstance()
    {
        if (nullptr == pSingleton) {
            pSingleton = new CSingleton();
        }
        return pSingleton;
    }
};
CSingleton* CSingleton::pSingleton = nullptr;

void zthread::test_Singleton()
{
    CSingleton* pSingleton = CSingleton::getInstance();
    return;
}
