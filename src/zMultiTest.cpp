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
    {
        thread thFunction(Funtion);
        thFunction.join();
    }
    cout << "1.2 Create by Functor" << endl;
    {
        thread thFunctor(Functor(12));
        thFunctor.join();
    }
    cout << "1.3 Create by Lambda" << endl;
    {
        auto la = [] { cout << "1.3 Lambda Start" << endl; };
        thread thLambda(la);
        thLambda.join();
    }
    cout << "1.4 Create by MemberFunction" << endl;
    {
        Functor objMemFunctor(0);
        thread th4(&Functor::MemberFunction, objMemFunctor, 14);
        th4.join();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

using std::unique_ptr;

class CArgMutable
{
  public:
    mutable int nData;
    CArgMutable() : nData(0) { cout << "2.2 Ctor\t Addr\t" << this << "\tData\t" << nData << "\tId " << get_id() << endl; };
    CArgMutable(int n) : nData(n) { cout << "2.2 Param Ctor\t Addr\t" << this << "\tData\t" << nData << "\tId " << get_id() << endl; };
    CArgMutable(const CArgMutable &a) : nData(a.nData) { cout << "2.2 Copy Ctor\t Addr\t" << this << "\tData\t" << nData << "\tId " << get_id() << endl; };
    ~CArgMutable() { cout << "2.2 Dctor\t Addr\t" << this << "\tData\t" << nData << "\tId " << get_id() << endl; };
};
class CArgUnMut
{
  public:
    int nData;
    CArgUnMut() : nData(0) { cout << "2.3 Ctor\t Addr\t" << this << "\tData\t" << nData << "\tId " << get_id() << endl; };
    CArgUnMut(int n) : nData(n) { cout << "2.3 Param Ctor\t Addr\t" << this << "\tData\t" << nData << "\tId " << get_id() << endl; };
    CArgUnMut(const CArgUnMut &a) : nData(a.nData) { cout << "2.3 Copy Ctor\t Addr\t" << this << "\tData\t" << nData << "\tId " << get_id() << endl; };
    ~CArgUnMut() { cout << "2.3 Dctor\t Addr\t" << this << "\tData\t" << nData << "\tId " << get_id() << endl; };
};
void zthread::test_BugsOfDetach()
{
    cout << "2.1 Pass by False Reference" << endl;
    {
        int nNum = 0;
        char cStr[] = {"2.1"};
        auto laFalseRef = [](const int &nNum, char *cStr) {
            cout << "2.1 ThreadNum\t Addr\t" << &nNum << "\tData\t" << nNum << "\tId " << get_id() << endl;
            cout << "2.1 ThreadArry\t Addr\t" << &cStr << "\tData\t" << cStr << "\tId " << get_id() << endl;
        };
        cout << "2.1 MainNum\t Addr\t" << &nNum << "\tData\t" << nNum << "\tId " << get_id() << endl;
        cout << "2.1 MainArry\t Addr\t" << &cStr << "\tData\t" << cStr << "\tId " << get_id() << endl;
        thread thFalseRef(laFalseRef, nNum, cStr); //1 ①引用传递参数(VS会进行一次拷贝 GNU C会进行两次拷贝) ②引用传递指针（）
        thFalseRef.join();
    }

    cout << "2.2 Pass Arguement by Parameterized Constructor" << endl;
    {
        CArgMutable objArgMutable(22);
        auto laPassbyObj = [](const CArgMutable &c) {
            cout << "2.2 ConstObj\t Addr\t" << &c << "\tData\t" << ++c.nData << "\tId " << get_id() << endl;
        };
        thread th2(laPassbyObj, 22); //2 不要用隐式类型转换
        cout << "2.2 Main Thread\t Addr\t" << &objArgMutable << "\tData\t" << objArgMutable.nData << "\tId " << get_id() << endl;
        th2.join();
    }

    cout << "2.3 Pass Arguement by Reference" << endl;
    {
        CArgUnMut objArgUnMut(23);
        auto laPassbyRef = [](CArgUnMut &c) {
            c.nData = 231;
            cout << "2.3 RefObj\t Addr\t" << &c << "\tData\t" << c.nData << "\tId " << get_id() << endl;
        };
        thread thPassbyRef(laPassbyRef, std::ref(objArgUnMut)); //3 通过引用传递的参数使用std::ref函数，线程接收参数是实际地址
        cout << "2.3 Main Thread\t Addr\t" << &objArgUnMut << "\tData\t" << objArgUnMut.nData << "\tId " << get_id() << endl;
        thPassbyRef.join();
    }

    cout << "2.4 Pass Smart Pointer as Arguement" << endl;
    {
        unique_ptr<int> pNum(new int(24));
        auto laPassbySmartPtr = [](unique_ptr<int> p) {
            *p = 241;
            cout << "2.4 Unique Ptr\t Addr\t" << &p << "\tData\t" << *p << "\tId " << get_id() << endl;
        };
        cout << "2.4 Main Thread\t Addr\t" << &pNum << "\tData\t" << *pNum << "\tId " << get_id() << endl;
        thread thPassbySmartPtr(laPassbySmartPtr, std::move(pNum));
        thPassbySmartPtr.join();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

#include <ctime>
#include <list>
#include <mutex>
#include <vector>

#include <pthread.h> // 临界区调用

using std::list;
using std::lock;
using std::mutex;
using std::vector;

void zthread::test_Threads()
{
    cout << "3.1 Write Threads to Vector" << endl;
    {
#define N_LOOP_3_1 10
        vector<thread> vThreads;

        auto laFunc = [](int tid) {
            cout << "3.1 Threads\t Id " << get_id() << "\tThreadId " << tid << endl;
        };
        for (size_t i = 0; i < N_LOOP_3_1; i++)
        {
            vThreads.push_back(thread(laFunc, i));
        }
        for (auto itThread = vThreads.begin(); itThread != vThreads.end(); itThread++)
        {
            itThread->join();
        }
        cout << "3.1 Main Thread\t Id " << get_id() << endl;
    }
    cout << "3.2 False Sharing" << endl;
    {
#define N_LOOP_3_2 1000000
        int aFalseSharing[100];
        auto laFalseSharing = [](int a[], int n) {for(size_t i =0; i< N_LOOP_3_2; i++)a[n] = i; };
        {
            size_t clkStart = clock();
            thread thFalseSharing1(laFalseSharing, aFalseSharing, 0);
            thread thFalseSharing2(laFalseSharing, aFalseSharing, 1);
            thFalseSharing1.join();
            thFalseSharing2.join();
            cout << "3.2 False Shr\t Loop\t" << N_LOOP_3_2 << "\tTaked " << clock() - clkStart << endl;
        }
        {
            size_t clkStart = clock();
            thread thFalseSharing1(laFalseSharing, aFalseSharing, 0);
            thread thFalseSharing2(laFalseSharing, aFalseSharing, 99);
            thFalseSharing1.join();
            thFalseSharing2.join();
            cout << "3.2 True Shr\t Loop\t" << N_LOOP_3_2 << "\tTaked " << clock() - clkStart << endl;
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

#define Z_MULTI_TEST_LOCK_TYPE 7
// 0 mutex直接加锁
// 1 lock_guard单独加锁(加索区域是其对象生命周期,由于加索顺序可能死锁)
// 2 lock加锁,再由lock_guard接管(lock请求不成功释放已获得的资源)
// 3 unique_lock默认使用跟lock_guard一致
// 4 unique_lock使用std::adopt，mutex需要lock，跟lock_guard一致
// 5 unique_lock使用std::try_to_lock，mutex不能lock
// 6 unique_lock使用std::defer_lock，mutex不能lock，初始化了一个unlock的mutex，可通过try_lock尝试加锁
// 7 unique_lock使用std::release，解绑unique_lock和mutex绑定关系，解绑unique_lock的mutex后有责任接管该mutex的lock和unlock操作
// 8 临界区
class CSharedList
{
  private:
    static list<int> NumList;
    static mutex mtxSharedList1;
    static mutex mtxSharedList2;
    static long nSum;
#if Z_MULTI_TEST_LOCK_TYPE == 8 && (defined _MSC_VER)
    CRITICAL_SECTION csSharedList;
#elif Z_MULTI_TEST_LOCK_TYPE == 8 && (defined __GNUC__)
    pthread_mutex_t csSharedList;
#endif
#define N_LOOP_4_2 10
  public:
    CSharedList()
    {
#if Z_MULTI_TEST_LOCK_TYPE == 8 && (defined _MSC_VER)
        InitializeCriticalSection(&csSharedList);
#elif Z_MULTI_TEST_LOCK_TYPE == 8 && (defined __GNUC__)
        pthread_mutex_init(&csSharedList, NULL);
#endif
    };
    ~CSharedList(){};
#if Z_MULTI_TEST_LOCK_TYPE == 8
    class CCriticalSection
    {
      public:
#if defined _MSC_VER
        CCriticalSection(CRITICAL_SECTION *p) : pCS(p)
        {
            EnterCriticalSection(pCS);
        }
#elif (defined __GNUC__)
        CCriticalSection(pthread_mutex_t *p) : pCS(p)
        {
            pthread_mutex_lock(pCS);
        }
#endif
        ~CCriticalSection()
        {
#if defined _MSC_VER
            LeaveCriticalSection(pCS);
#elif (defined __GNUC__)
            pthread_mutex_unlock(pCS);
#endif
        }

      private:
#if defined _MSC_VER
        CRITICAL_SECTION *pCS;
#elif (defined __GNUC__)
        pthread_mutex_t *pCS;
#endif
    };
#endif
    void write()
    {
        for (size_t i = 0; i < N_LOOP_4_2; i++)
        {
#if Z_MULTI_TEST_LOCK_TYPE == 0
            mtxSharedList1.lock();
            mtxSharedList2.lock();
#elif Z_MULTI_TEST_LOCK_TYPE == 8
            CCriticalSection csLock(&csSharedList);
#endif
            // std::chrono::milliseconds sec5(5000);
            // std::this_thread::sleep_for(sec5);
            cout << "4.2 WriteDone\t Addr\t" << this << "\tData\t" << i << "\tId " << get_id() << endl;
            NumList.push_back(i);
#if Z_MULTI_TEST_LOCK_TYPE == 0
            mtxSharedList1.unlock();
            mtxSharedList2.unlock();
#endif
        }
    }
    void read()
    {
        for (size_t i = 0; i < N_LOOP_4_2; i++)
        {
            int nData = 0;
            // mtxSharedList1.lock();
            // mtxSharedList2.lock();
            if (read_delegation(nData) == true)
                cout << "4.2 ReadDone \t Addr\t" << this << "\tData\t" << nData << "\tId " << get_id() << endl;
            else
                cout << "4.2 ReadEmpty \t Addr\t" << this << "\tData\t" << nData << "\tId " << get_id() << endl;
            // mtxSharedList1.unlock();
            // mtxSharedList2.unlock();
        }
    }
    bool read_delegation(int &n)
    {
#if Z_MULTI_TEST_LOCK_TYPE == 1
        std::lock_guard<mutex> lgSharedList1(mtxSharedList1);
        std::lock_guard<mutex> lgSharedList2(mtxSharedList2);
#elif Z_MULTI_TEST_LOCK_TYPE == 2
        lock(mtxSharedList1, mtxSharedList2);
        // std::lock访问互斥资源在请求不成功情况下不会保持请求成功的资源，请求不成功主动释放
        std::lock_guard<mutex>(mtxSharedList1, std::adopt_lock);
        std::lock_guard<mutex>(mtxSharedList2, std::adopt_lock);
#elif Z_MULTI_TEST_LOCK_TYPE == 3
        std::unique_lock<mutex> ulSharedList1(mtxSharedList1);
        std::unique_lock<mutex> ulSharedList2(mtxSharedList2);
#elif Z_MULTI_TEST_LOCK_TYPE == 4
        lock(mtxSharedList1, mtxSharedList2);
        std::unique_lock<mutex> ulSharedList1(mtxSharedList1, std::adopt_lock);
        std::unique_lock<mutex> ulSharedList2(mtxSharedList2, std::adopt_lock);
#elif Z_MULTI_TEST_LOCK_TYPE == 5
        std::unique_lock<mutex> ulSharedList1(mtxSharedList1, std::try_to_lock);
        std::unique_lock<mutex> ulSharedList2(mtxSharedList2, std::try_to_lock);
        if (!(ulSharedList1.owns_lock() == true && ulSharedList2.owns_lock() == true))
        {
            cout << "4.2 ReadLck\t Addr\t" << this << "\tData\tError"
                 << "\tId " << get_id() << endl;
            return false;
        }
#elif Z_MULTI_TEST_LOCK_TYPE == 6
        std::unique_lock<mutex> dlSharedList1(mtxSharedList1, std::defer_lock);
        std::unique_lock<mutex> dlSharedList2(mtxSharedList1, std::defer_lock);
        dlSharedList1.lock();
        if (true == dlSharedList2.try_lock()) // 类似std::try_to_lock
        {
            cout << "4.2 ReadLck\t Addr\t" << this << "\tData\tError"
                 << "\tId " << get_id() << endl;
            dlSharedList1.unlock();
        }
#elif Z_MULTI_TEST_LOCK_TYPE == 7
        auto laMoveMutex = [](std::mutex &mtx) { return std::unique_lock<mutex>(mtx); };
        std::unique_lock<mutex> ulSharedList1 = laMoveMutex(mtxSharedList1);
        std::unique_lock<mutex> ulSharedList2(std::move(ulSharedList1));
        mutex *pmtxSharedList2 = ulSharedList2.release();
#elif Z_MULTI_TEST_LOCK_TYPE == 8 && (defined _MSC_VER)
        EnterCriticalSection(&mutex_lock);
#elif Z_MULTI_TEST_LOCK_TYPE == 8 && (defined __GNUC__)
        pthread_mutex_lock(&csSharedList);
#elif Z_MULTI_TEST_LOCK_TYPE == 9

#endif
        nSum++;
        bool bEmpty = NumList.empty();
        if (bEmpty != true)
        {
            n = NumList.front();
            NumList.pop_front();
        }
#if Z_MULTI_TEST_LOCK_TYPE == 7
        pmtxSharedList2->unlock();
#elif Z_MULTI_TEST_LOCK_TYPE == 8 && (defined _MSC_VER)
        LeaveCriticalSection(&csSharedList);
#elif Z_MULTI_TEST_LOCK_TYPE == 8 && (defined __GNUC__)
        pthread_mutex_unlock(&csSharedList);
#endif
        return !bEmpty;
    }
};
list<int> CSharedList::NumList;
mutex CSharedList::mtxSharedList1;
mutex CSharedList::mtxSharedList2;
long CSharedList::nSum = 0;

void zthread::test_DataSharing()
{
    cout << "4.1 Read Only" << endl;
    {
#define N_LOOP_4_1 10
        vector<thread> vThreads;
        static int nData = 1;
        auto laReadOnly = [](int n) {
            cout << "4.1 Threads\t Read\t" << n << "\tId " << get_id() << endl;
        };
        for (size_t i = 0; i < N_LOOP_4_1; i++)
        {
            vThreads.push_back(thread(laReadOnly, nData));
        }
        for (auto itThread = vThreads.begin(); itThread != vThreads.end(); itThread++)
        {
            itThread->join();
        }
        cout << "4.1 Main Thread\t Id " << get_id() << endl;
    }
    cout << "4.2 Read and Write with Lock" << endl;
    {
        CSharedList objNumList;
        thread thRecv(&CSharedList::write, objNumList);
        thread thSend(&CSharedList::read, objNumList);
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
            Creation();
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

void zthread::test_Singleton()
{
#define N_LOOP_5_1 1000
    cout << "5.1 Singleton\t Id " << get_id() << endl;
    vector<thread> vThreads;
    auto laGet = [] { CSingleton *pSingleton = CSingleton::getInstance(); };
    auto laCallOnce = [] { CSingleton *pSingleton = CSingleton::getInstanceCallOnce(); };
    for (size_t i = 0; i < N_LOOP_5_1; i++)
    {
        vThreads.push_back(thread(laGet));
        thread p(laCallOnce);
        vThreads.push_back(std::move(p));
        thread p(laCallOnce);
        vThreads.emplace_back(p);
    }
    for (auto it = vThreads.begin(); it != vThreads.end(); it++)
    {
        it->join();
    }
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
    void write_notify_one()
    {
        for (size_t i = 0; i < 10000; i++)
        {
            std::unique_lock<mutex> uNumQueue(mtxNumQueue);
            NumQueue.push(i);
            cout << "6.1 WriteDone \t Addr\t" << this << "\tData\t" << i << "\tId " << get_id() << endl;
            cvNumQueue.notify_all();
        }
    }
    void read_wait()
    {
        for (;;)
        {
            std::unique_lock<mutex> uNumQueue(mtxNumQueue);
            auto laEmpty = [] { return !NumQueue.empty(); };
            cvNumQueue.wait(uNumQueue, laEmpty);
            cout << "6.1 ReadDone \t Addr\t" << this << "\tData\t" << NumQueue.front() << "\tId " << get_id() << endl;
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
    thread thWrite(&CNumQueue::write_notify_one, objNumQueue);
    thread thRead1(&CNumQueue::read_wait, objNumQueue);
    thread thRead2(&CNumQueue::read_wait, objNumQueue);
    thWrite.join();
    thRead1.join();
    thRead2.join();
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

#include <ctime>
#include <future>

int FutureTest(int n)
{
    cout << "7.X Future Beg\t Data\t" << n << "\tId " << get_id() << endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
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
        {
            std::future<int> fAsync = std::async(std::launch::async, FutureTest, 0);
            int nAsync = fAsync.valid() == true ? fAsync.get() : -1;
            cout << "7.1 Async\t Data\t" << nAsync << "\tId " << get_id() << endl;
        }
        {
            std::future<int> fDeferred = std::async(std::launch::deferred, FutureTest, 0);
            int nDeferred = fDeferred.valid() == true ? fDeferred.get() : -1; // future::get仅可取一次结果
            cout << "7.1 Deferred\t Data\t" << nDeferred << "\tId " << get_id() << endl;
        }
        {
            // std::async第一参数不传则为std::launch::async | std::launch::deferred默认值，由系统自行决定是否启用异步任务
            std::future<int> fDefault = std::async(std::launch::async | std::launch::deferred, FutureTest, 0);
            auto nDefault = fDefault.get();
            cout << "7.1 Default\t Data\t" << nDefault << "\tId " << get_id() << endl;
        }
    }

    cout << "7.2 Package Task" << endl;
    {
        // std::packaged_task 包装任务方便用作线程入口函数使用
        std::packaged_task<int(int)> ptFunction(FutureTest);
        std::packaged_task<int(int)> ptLambda([](int n) { cout << "7.2 Lambda\t Data\t" << n << "\tId " << get_id() << endl; return ++n; });
        std::packaged_task<int(int)> ptPackage(FutureTest);
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
        std::packaged_task<int(int)> ptFunction(FutureTest);
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
#define Z_MULTI_TEST_FUTURE_STATUS 3
// 1 std::future_status::wait_for
// 2 std::future_ststus::ready
// 3 std::future_status::deferred
#if Z_MULTI_TEST_FUTURE_STATUS == 1
        std::future<int> fStatus = std::async(std::launch::async, FutureTest, 6);
        std::future_status fsStatus = fStatus.wait_for(std::chrono::seconds(0));
#elif Z_MULTI_TEST_FUTURE_STATUS == 2
        std::future<int> fStatus = std::async(std::launch::async, FutureTest, 6);
        std::future_status fsStatus = fStatus.wait_for(std::chrono::seconds(10));
#elif Z_MULTI_TEST_FUTURE_STATUS == 3
        std::future<int> fStatus = std::async(std::launch::deferred, FutureTest, 6);
        std::future_status fsStatus = fStatus.wait_for(std::chrono::seconds(0));
#endif
        if (fsStatus == std::future_status::timeout)
        {
            cout << "7.5 Time Out\t Data\t" << -1 << "\tId " << get_id() << endl;
        }
        else if (fsStatus == std::future_status::ready)
        {
            cout << "7.5 Time On\t Data\t" << fStatus.get() << "\tId " << get_id() << endl;
        }
        else if (fsStatus == std::future_status::deferred)
        {
            auto n = fStatus.get();
            cout << "7.5 Deferred\t Data\t" << n << "\tId " << get_id() << endl;
        }
    }

#define Z_MULTI_TEST_SHARED_FUTURE 2
    // 1 assign std::shared_future by get_future
    // 2 init std::shared_future by future object
    // 3 init std::shared_future by future object.share()
    cout << "7.6 Shared Future" << endl;
    {
        std::packaged_task<int(int)> ptShared(FutureTest);
        thread thSet(std::ref(ptShared), 7);
        thSet.join();
#if Z_MULTI_TEST_SHARED_FUTURE == 1
        std::shared_future<int> sfShared = ptShared.get_future();
#elif Z_MULTI_TEST_SHARED_FUTURE == 2
        std::shared_future<int> sfShared(ptShared.get_future());
#elif Z_MULTI_TEST_SHARED_FUTURE == 3
        std::future<int> fShared = ptShared.get_future();
        std::shared_future<int> sfShared(fShared.share());
#endif
        int n = sfShared.valid() == true ? sfShared.get() : -1;
        cout << "7.6 Main Get\t Data\t" << n << "\tId " << get_id() << endl;
        // 从另一个线程中取值
        auto laGet = [](std::shared_future<int> f) {auto n = f.get();cout << "7.6 Lambda Get\t Data\t" << n << "\tId " << get_id() << endl; };
        laGet(sfShared); // Call Lambda Directly
        thread thGet(laGet, sfShared);
        thGet.join();
    }

    cout << "7.7 Atomic Variable" << endl;
    {
        static std::atomic<int> atmData;
        static mutex mtxAtomic;
        static int nData;
#define N_LOOP_7_7 1000000
        auto laMutexless = [] { for(size_t i = 0; i < N_LOOP_7_7; i++){nData++;} };
        auto laAtomic = [] { for(size_t i = 0; i < N_LOOP_7_7; i++){atmData++;} };
        auto laAtomicFailed = [] { for(size_t i = 0; i < N_LOOP_7_7; i++){atmData = atmData + 1;} };
        auto laMutex = [] { for(size_t i = 0; i < N_LOOP_7_7; i++){mtxAtomic.lock(); nData++; mtxAtomic.unlock();} };
        size_t clkStart;

        nData = 0;
        {
            clkStart = clock();
            thread thMutexless1(laMutexless);
            thread thMutexless2(laMutexless);
            thMutexless1.join();
            thMutexless2.join();
            cout << "7.7 Do Mtxless\t Data\t" << nData << "\tTaked " << clock() - clkStart << endl;
        }
        atmData = 0;
        {
            clkStart = clock();
            thread thAtomic1(laAtomic);
            thread thAtomic2(laAtomic);
            thAtomic1.join();
            thAtomic2.join();
            cout << "7.7 Do Atomic\t Data\t" << atmData << "\tTaked " << clock() - clkStart << endl;
        }
        nData = 0;
        {
            clkStart = clock();
            thread thMutex1(laMutex);
            thread thMutex2(laMutex);
            thMutex1.join();
            thMutex2.join();
            cout << "7.7 Do Mutex\t Data\t" << nData << "\tTaked " << clock() - clkStart << endl;
        }
        atmData = 0;
        {
            clkStart = clock();
            thread thAtomicFailed1(laAtomicFailed);
            thread thAtomicFailed2(laAtomicFailed);
            thAtomicFailed1.join();
            thAtomicFailed2.join();
            cout << "7.7 Non Atomic\t Data\t" << atmData << "\tTaked " << clock() - clkStart << endl;
            // atomic 对++ -- += -=
        }
    }
}
