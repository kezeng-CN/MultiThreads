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
    thread th1(Funtion);
    th1.join();
    cout << "1.2 Create by Functor" << endl;
    thread th2(Functor(12));
    th2.join();
    cout << "1.3 Create by Lambda" << endl;
    auto LambdaTH = [] { cout << "1.3 Lambda Start" << endl; };
    thread th3(LambdaTH);
    th3.join();
    cout << "1.4 Create by MemberFunction" << endl;
    Functor objFunctor(0);
    thread th4(&Functor::MemberFunction, objFunctor, 14);
    th4.join();
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
        thread th1(FunctionPassbyFalseRef, nNum, cStr); //1 ①引用传递参数(VS会进行一次拷贝 GNU C会进行两次拷贝) ②引用传递指针（）
        th1.join();
    }

    cout << "2.2 Pass Arguement by Parameterized Constructor" << endl;
    {
        CArgMutable objArgMutable(22);
        thread th2(FunctionPassbyObj, 22); //2 不要用隐式类型转换
        cout << "2.2 Main Thread\t Addr\t" << &objArgMutable << "\tData\t" << objArgMutable.nData << "\tId " << get_id() << endl;
        th2.join();
    }

    cout << "2.3 Pass Arguement by Reference" << endl;
    {
        CArgUnMut objArgUnMut(23);
        thread th3(FunctionPassbyRef, std::ref(objArgUnMut)); //3 通过引用传递的参数使用std::ref函数，线程接收参数是实际地址
        cout << "2.3 Main Thread\t Addr\t" << &objArgUnMut << "\tData\t" << objArgUnMut.nData << "\tId " << get_id() << endl;
        th3.join();
    }

    cout << "2.4 Pass Smart Pointer as Arguement" << endl;
    {
        unique_ptr<int> pNum(new int(24));
        cout << "2.4 Main Thread\t Addr\t" << &pNum << "\tData\t" << *pNum << "\tId " << get_id() << endl;
        thread th4(FunctionPassbySmartPtr, std::move(pNum));
        th4.join();
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

class CNumList
{
  private:
    static list<int> NumList;
    static mutex mutexNumList1;
    static mutex mutexNumList2;
    static long nSum;

  public:
    CNumList(){};
    ~CNumList(){};
    void push()
    {
        for (size_t i = 0; i < 100000; i++)
        {
            mutexNumList1.lock();
            mutexNumList2.lock();
            // std::chrono::milliseconds sec5(5000);
            // std::this_thread::sleep_for(sec5);
            cout << "4.2 PushDone\t Addr\t" << this << "\tData\t" << nSum++ << "\tId " << get_id() << endl;
            NumList.push_back(i);
            mutexNumList1.unlock();
            mutexNumList2.unlock();
        }
    }
    void pop()
    {
        for (size_t i = 0; i < 100000; i++)
        {
            int nData = 0;
            // mutexNumList1.lock();
            // mutexNumList2.lock();
            if (pop_delegation(nData) == true)
                cout << "4.2 PopDone \t Addr\t" << this << "\tData\t" << nSum << "\tId " << get_id() << endl;
            else
                cout << "4.2 PopEmpty \t Addr\t" << this << "\tData\t" << nSum << "\tId " << get_id() << endl;
            // mutexNumList1.unlock();
            // mutexNumList2.unlock();
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
    bool pop_delegation(int &n)
    {
#if Z_MULTI_TEST_LOCK_TYPE == 1
        std::lock_guard<mutex> lock_guardNumList1(mutexNumList1);
        std::lock_guard<mutex> lock_guardNumList2(mutexNumList2);
#elif Z_MULTI_TEST_LOCK_TYPE == 2
        lock(mutexNumList1, mutexNumList2);
        // std::lock访问互斥资源在请求不成功情况下不会保持请求成功的资源，请求不成功主动释放
        std::lock_guard<mutex>(mutexNumList1, std::adopt_lock);
        std::lock_guard<mutex>(mutexNumList2, std::adopt_lock);
#elif Z_MULTI_TEST_LOCK_TYPE == 3
        std::unique_lock<mutex> unique_lockNumList1(mutexNumList1);
        std::unique_lock<mutex> unique_lockNumList2(mutexNumList2);
#elif Z_MULTI_TEST_LOCK_TYPE == 4
        lock(mutexNumList1, mutexNumList2);
        std::unique_lock<mutex> unique_lockNumList1(mutexNumList1, std::adopt_lock);
        std::unique_lock<mutex> unique_lockNumList2(mutexNumList2, std::adopt_lock);
#elif Z_MULTI_TEST_LOCK_TYPE == 5
        std::unique_lock<mutex> unique_lockNumList1(mutexNumList1, std::try_to_lock);
        std::unique_lock<mutex> unique_lockNumList2(mutexNumList2, std::try_to_lock);
        if (!(unique_lockNumList1.owns_lock() == true && unique_lockNumList2.owns_lock() == true))
        {
            cout << "4.2 PopLck\t Addr\t" << this << "\tData\tERROR"
                 << "\tId " << get_id() << endl;
            return false;
        }
#elif Z_MULTI_TEST_LOCK_TYPE == 6
        std::unique_lock<mutex> defer_lockNumList1(mutexNumList1, std::defer_lock);
        std::unique_lock<mutex> defer_lockNumList2(mutexNumList1, std::defer_lock);
        defer_lockNumList1.lock();
        if (true == defer_lockNumList2.try_lock()) // 类似std::try_to_lock
        {
            cout << "4.2 PopLck\t Addr\t" << this << "\tData\tERROR"
                 << "\tId " << get_id() << endl;
            defer_lockNumList1.unlock();
        }
#elif Z_MULTI_TEST_LOCK_TYPE == 7
        std::unique_lock<mutex> unique_lockNumList1 = move_mutex(mutexNumList1);
        std::unique_lock<mutex> unique_lockNumList2(std::move(unique_lockNumList1));
        mutex *pMutexNumList2 = unique_lockNumList2.release();
#endif
        nSum++;
        bool bEmpty = NumList.empty();
        if (bEmpty != true)
        {
            n = NumList.front();
            NumList.pop_front();
        }
#if Z_MULTI_TEST_LOCK_TYPE == 7
        pMutexNumList2->unlock();
#endif
        return !bEmpty;
    }
    std::unique_lock<mutex> move_mutex(mutex &m)
    {
        // 生成临时unique_lock对象，并调用unique_lock的移动构造函数
        return std::unique_lock<mutex>(m);
    }
};
list<int> CNumList::NumList;
mutex CNumList::mutexNumList1;
mutex CNumList::mutexNumList2;
long CNumList::nSum = 0;

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
        CNumList objNumList;
        thread thRecv(&CNumList::push, objNumList);
        thread thSend(&CNumList::pop, objNumList);
        thRecv.join();
        thSend.join();
        cout << "4.2 Main Thread\t Id " << get_id() << endl;
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

class CSingleton
{
  private:
    CSingleton() { cout << "5.1 Singleton \t Addr\t" << this << "\tObject\t" << pSingleton << "\tId " << get_id() << endl; }
    static CSingleton *pSingleton;
    static mutex mutexCreation;
    static std::once_flag once_flagCreation;
    class CFixMemLeak
    {
      public:
        ~CFixMemLeak()
        {
            if (nullptr != pSingleton)
            {
                delete (pSingleton);
            }
        }
    };

  public:
    static CSingleton *getInstance()
    {
        if (nullptr == pSingleton)
        {
            std::unique_lock<mutex> unique_lockCreation(mutexCreation);
            if (nullptr == pSingleton)
            {
                pSingleton = new CSingleton();
                static CFixMemLeak objFixMemLeak;
            }
        }
        return pSingleton;
    }
    static void Creation()
    {
        if (nullptr == pSingleton)
        {
            pSingleton = new CSingleton();
            static CFixMemLeak objFixMemLeak;
        }
    }
    static CSingleton *getInstanceCallOnce()
    {
        std::call_once(once_flagCreation, Creation);
        return pSingleton;
    }
};
CSingleton *CSingleton::pSingleton = nullptr;
mutex CSingleton::mutexCreation;
std::once_flag CSingleton::once_flagCreation;

void function_singleton()
{
    // CSingleton *pSingleton = CSingleton::getInstance();
    CSingleton *pSingleton = CSingleton::getInstanceCallOnce();
}

void zthread::test_Singleton()
{
    cout << "5.1 Singleton\t Id " << get_id() << endl;
    thread objThd1(function_singleton);
    thread objThd2(function_singleton);
    objThd1.join();
    objThd2.join();
    return;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

#include <condition_variable>
#include <queue>

using std::queue;
class CNumQueue
{
  private:
    static queue<int> NumQueue;
    static mutex mtxNumQueue;
    static std::condition_variable cvNumQueue;

  public:
    void push_notify_one()
    {
        for (size_t i = 0; i < 10000; i++)
        {
            std::unique_lock<mutex> uNumQueue(mtxNumQueue);
            NumQueue.push(i);
            cout << "6.1 PushDone \t Addr\t" << this << "\tData\t" << i << "\tId " << get_id() << endl;
            cvNumQueue.notify_all();
        }
    }
    void pop_wait()
    {
        for (;;)
        {
            std::unique_lock<mutex> uNumQueue(mtxNumQueue);
            cvNumQueue.wait(uNumQueue, [this] { return !NumQueue.empty(); });
            cout << "6.1 PopDone \t Addr\t" << this << "\tData\t" << NumQueue.front() << "\tId " << get_id() << endl;
            NumQueue.pop();
        }
    }
};
queue<int> CNumQueue::NumQueue;
mutex CNumQueue::mtxNumQueue;
std::condition_variable CNumQueue::cvNumQueue;

void zthread::test_Condition_Variable()
{
    CNumQueue objNumQueue;
    thread thPush(CNumQueue::push_notify_one, objNumQueue);
    thread thPop1(CNumQueue::pop_wait, objNumQueue);
    thread thPop2(CNumQueue::pop_wait, objNumQueue);
    thPush.join();
    thPop1.join();
    thPop2.join();
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

#include <future>

int future(int n)
{
    cout << "7.X Future Beg\t Data\t" << n << "\tId " << get_id() << endl;
    std::chrono::milliseconds sec5(5000);
    // std::this_thread::sleep_for(sec5);
    cout << "7.X Future End\t Data\t" << n << "\tId " << get_id() << endl;
    return n;
}

void PromiseSet(std::promise<int> &pms, int n)
{
    cout << "7.4 Future Set\t Data\t" << n << "\tId " << get_id() << endl;
    pms.set_value(n);
}

void PromiseGet(std::promise<int> &pms)
{
    auto fPromise = pms.get_future();
    int n = fPromise.get();
    cout << "7.4 Future Get\t Data\t" << n << "\tId " << get_id() << endl;
}

void zthread::test_Future()
{
    cout << "7.1 Get Retired Value by Future Object" << endl;
    {
        std::future<int> fAsync = std::async(std::launch::async, future, 0);
        int nAsync = fAsync.get(); // 等待线程执行结束并返回值，不能多次调用
        std::future<int> fDeferred = std::async(std::launch::deferred, future, 0);
        int nDeferred = fDeferred.get(); // future::get仅有一次取得到结果
        cout << "7.1 Main Thread\t Data\t" << nAsync << "\tId " << get_id() << endl;
        cout << "7.1 Main Thread\t Data\t" << nDeferred << "\tId " << get_id() << endl;
    }

    cout << "7.2 Package Task" << endl;
    {
        // std::packaged_task 包装任务方便用作线程入口函数使用
        std::packaged_task<int(int)> ptFunction(future);
        std::packaged_task<int(int)> ptLambda([](int n) { cout << "7.2 Lambda\t Data\t" << n << "\tId " << get_id() << endl; return ++n; });
        std::packaged_task<int(int)> ptPackage(future);
        thread thFunction(std::ref(ptFunction), 1);
        thread thLambda(std::ref(ptLambda), 2);
        ptPackage(3);
        thFunction.join();
        thLambda.join();
        std::future<int> fFuture = ptFunction.get_future();
        std::future<int> fLambda = ptLambda.get_future();
        std::future<int> fPackage = ptPackage.get_future();
        cout << "7.2 Function\t Data\t" << fFuture.get() << "\tId " << get_id() << endl;
        cout << "7.2 Lambda\t Data\t" << fLambda.get() << "\tId " << get_id() << endl;
        cout << "7.2 Package\t Data\t" << fPackage.get() << "\tId " << get_id() << endl;
    }

    cout << "7.3 Package Move" << endl;
    {
        std::packaged_task<int(int)> ptFunction(future);
        vector<std::packaged_task<int(int)>> vPackage;
        vPackage.push_back(std::move(ptFunction));
        auto itPackage = vPackage.begin();
        std::packaged_task<int(int)> ptFunctionCopied = std::move(*itPackage);
        vPackage.erase(itPackage);
        ptFunctionCopied(4);
        std::future<int> fPackage = ptFunctionCopied.get_future();
        cout << "7.3 Pkg Moved\t Data\t" << fPackage.get() << "\tId " << get_id() << endl;
    }

    cout << "7.4 Promise" << endl;
    {
        std::promise<int> pms;
        thread thPromiseSet(PromiseSet, std::ref(pms), 5);
        thread thPromiseGet(PromiseGet, std::ref(pms));
        thPromiseSet.join();
        thPromiseGet.join();
    }

    cout << "7.5 Wait For & Future Status" << endl;
    {
        std::future<int> fStatus = std::async(std::launch::deferred, future, 6);
        std::future_status fsStatus = fStatus.wait_for(std::chrono::microseconds(1000));
        if (fsStatus == std::future_status::timeout)
        {
            cout << "7.5 Time Out\t Data\t" << "ERROR" << "\tId " << get_id() << endl;
        }
        else if(fsStatus == std::future_status::ready)
        {
            cout << "7.5 Time On\t Data\t" << fStatus.get() << "\tId " << get_id() << endl;
        }
        else if(fsStatus == std::future_status::deferred)
        {
            // 如果std::async状态设置成std::deferred
            auto n = fStatus.get();
            cout << "7.5 Deferred\t Data\t" << n << "\tId " << get_id() << endl;
        }
    }

    cout << "7.6 Shared Future" << endl;
    {

    }
}
