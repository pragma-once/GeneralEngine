#include "../../Engine/Engine.h"
#include <iostream>

#include <cstdlib>
#include <ctime>

using namespace std::literals::chrono_literals;

#define print(context) (std::cout << context << '\n')
#define input(var) (std::cin >> var)

Engine::Utilities::SmartMutex m;
Engine::Utilities::Shared<int, true> NextThreadID = 0;

int GetID()
{
    NextThreadID.Mutex.Lock();
    int id = NextThreadID;
    NextThreadID = NextThreadID + 1;
    NextThreadID.Mutex.Unlock();
    return id;
}

const char * space = "                        ";

void Lock(int id)
{
    print(id << space << id << ": pre: lock");
    m.Lock();
    print(id << ": post: lock");
}

void SharedLock(int id)
{
    print(id << space << id << ": pre: shared-lock");
    m.SharedLock();
    print(id << ": post: shared-lock");
}

void Unlock(int id)
{
    print(id << space << id << ": pre: unlock");
    m.Unlock();
    print(id << ": post: unlock");
}

void SharedUnlock(int id)
{
    print(id << space << id << ": pre: shared-unlock");
    m.SharedUnlock();
    print(id << ": post: shared-unlock");
}

void thread1()
{
    std::srand(std::time(nullptr));
    int id = GetID();
    SharedLock(id);
    std::this_thread::sleep_for(100ms);
    Lock(id);
    std::this_thread::sleep_for(100ms);
    Unlock(id);
    std::this_thread::sleep_for((std::rand() % 1000) * 1ms);
    SharedUnlock(id);
}

int main()
{
    char dummy[256];
    while (true)
    {
        std::thread threads[4];
        for (int i = 0; i < 4; i++)
            threads[i] = std::thread(thread1);
        for (int i = 0; i < 4; i++)
            threads[i].join();
        input(dummy);
    }
}
