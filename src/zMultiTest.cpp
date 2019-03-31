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
#include <vector>

using std::list;
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
    list<int> MsgQueue;

  public:
    CMsgQueue(/* args */){};
    ~CMsgQueue(){};
    void Push()
    {
        for (size_t i = 0; i < 10; i++)
        {
            cout << "4.2 Push\t Data\t" << i << "\tId " << get_id() << endl;
            MsgQueue.push_back(i);
        }
    }
    void Pop()
    { 
        for (size_t i = 0; i < 10000; i++)
        {
            if (!MsgQueue.empty())
            {
                cout << "4.2 Pop\t Data\t" << MsgQueue.front() << "\tId " << get_id() << endl;
                MsgQueue.pop_front();
            }
        }
    }
};
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

    cout << "4.2 Read and Write" << endl;
    {
        CMsgQueue objMsgQueue;
        thread thRecv(&CMsgQueue::Push, objMsgQueue);
        thRecv.join();
        thread thAns(&CMsgQueue::Pop, objMsgQueue);
        thAns.join();
    }
}
