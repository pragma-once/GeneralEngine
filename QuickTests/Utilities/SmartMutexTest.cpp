#include "../../Engine/Engine.h"
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace std::literals::chrono_literals;
using namespace Engine::Utilities;

#define print(context) (std::cout << context << '\n')
#define input(var) (std::cin >> var)

SmartMutex m;

Shared<int, true> NextThreadID = 0;
int GetID()
{
    auto guard = NextThreadID.Mutex.GetLock();
    int id = NextThreadID;
    NextThreadID = NextThreadID + 1;
    return id;
}

Collections::Dictionary<std::string, SmartMutex::LockGuard*> GlobalLockGuards;
Collections::Dictionary<std::string, SmartMutex::SharedLockGuard*> GlobalSharedLockGuards;

enum CommandType
{
    Sleep,
    Lock,
    Unlock,
    SharedLock,
    SharedUnlock,
    GlobalLock,
    GlobalUnlock,
    GlobalSharedLock,
    GlobalSharedUnlock
};
CommandType NameToCommandType(std::string name)
{
    if (name == "s") return Sleep;
    else if (name == "l") return Lock;
    else if (name == "u") return Unlock;
    else if (name == "sl") return SharedLock;
    else if (name == "su") return SharedUnlock;
    else if (name == "gl") return GlobalLock;
    else if (name == "gu") return GlobalUnlock;
    else if (name == "gsl") return GlobalSharedLock;
    else if (name == "gsu") return GlobalSharedUnlock;
    else throw std::domain_error("Undefined command");
}

struct Command
{
    CommandType Type;
    std::string GuardId;
    double SleepDuration;
    Command() {}
    Command(double SleepDuration) : Type(CommandType::Sleep), SleepDuration(SleepDuration) {}
    Command(CommandType Type, std::string GuardId) : Type(Type), GuardId(GuardId) {}
};

class TestThread
{
private:
    int ID;
    Collections::Dictionary<std::string, SmartMutex::LockGuard*> LockGuards;
    Collections::Dictionary<std::string, SmartMutex::SharedLockGuard*> SharedLockGuards;
    Collections::List<Command> Commands;
    std::thread Thread;

    void Lock(std::string guard_id)
    {
        try
        {
            LockGuards.SetValue(guard_id, new SmartMutex::LockGuard(m.GetLock()));
            print(ID << ": locked " << guard_id << " locally");
        }
        catch (std::exception& e)
        {
            print(ID << ": exception on locking " << guard_id << " locally: " << e.what());
        }
    }
    void Unlock(std::string guard_id)
    {
        delete LockGuards.GetValue(guard_id);
        LockGuards.Remove(guard_id);
        print(ID << ": unlocked " << guard_id << " locally");
    }
    void SharedLock(std::string guard_id)
    {
        SharedLockGuards.SetValue(guard_id, new SmartMutex::SharedLockGuard(m.GetSharedLock()));
        print(ID << ": shared-locked " << guard_id << " locally");
    }
    void SharedUnlock(std::string guard_id)
    {
        delete SharedLockGuards.GetValue(guard_id);
        SharedLockGuards.Remove(guard_id);
        print(ID << ": shared-unlocked " << guard_id << " locally");
    }

    void GlobalLock(std::string guard_id)
    {
        try
        {
            GlobalLockGuards.SetValue(guard_id, new SmartMutex::LockGuard(m.GetLock()));
            print(ID << ": locked " << guard_id << " globally");
        }
        catch (std::exception& e)
        {
            print(ID << ": exception on locking " << guard_id << " globally: " << e.what());
        }
    }
    void GlobalUnlock(std::string guard_id)
    {
        delete GlobalLockGuards.GetValue(guard_id);
        GlobalLockGuards.Remove(guard_id);
        print(ID << ": unlocked " << guard_id << " globally");
    }
    void GlobalSharedLock(std::string guard_id)
    {
        GlobalSharedLockGuards.SetValue(guard_id, new SmartMutex::SharedLockGuard(m.GetSharedLock()));
        print(ID << ": shared-locked " << guard_id << " globally");
    }
    void GlobalSharedUnlock(std::string guard_id)
    {

        delete GlobalSharedLockGuards.GetValue(guard_id);
        GlobalSharedLockGuards.Remove(guard_id);
        print(ID << ": shared-unlocked " << guard_id << " globally");
    }

    void ExecuteCommand(Command cmd)
    {
        switch (cmd.Type)
        {
            case CommandType::Sleep: std::this_thread::sleep_for(cmd.SleepDuration * 1ms); break;
            case CommandType::Lock: Lock(cmd.GuardId); break;
            case CommandType::Unlock: Unlock(cmd.GuardId); break;
            case CommandType::SharedLock: SharedLock(cmd.GuardId); break;
            case CommandType::SharedUnlock: SharedUnlock(cmd.GuardId); break;
            case CommandType::GlobalLock: GlobalLock(cmd.GuardId); break;
            case CommandType::GlobalUnlock: GlobalUnlock(cmd.GuardId); break;
            case CommandType::GlobalSharedLock: GlobalSharedLock(cmd.GuardId); break;
            case CommandType::GlobalSharedUnlock: GlobalSharedUnlock(cmd.GuardId); break;
        }
    }

public:
    TestThread()
    {
        ID = GetID();
        while (true)
        {
            std::string str;
            double number;
            std::string guard_id;
            print("Commands:");
            print("  s <milliseconds> => Sleep");
            print("  l <guard_id>     => Lock");
            print("  u <guard_id>     => Unlock");
            print("  sl <guard_id>    => SharedLock");
            print("  su <guard_id>    => SharedUnlock");
            print("  gl <guard_id>    => GlobalLock");
            print("  gu <guard_id>    => GlobalUnlock");
            print("  gsl <guard_id>   => GlobalSharedLock");
            print("  gsu <guard_id>   => GlobalSharedUnlock");
            print("Enter a command to add to thread " << ID << " or d (or q or e) to finish:");
            input(str);
            if (str == "d" || str == "q" || str == "e")
                break;
            if (str == "s")
            {
                input(number);
                Commands.Add(Command(number));
            }
            else
            {
                try
                {
                    CommandType Type = NameToCommandType(str);
                    input(str);
                    Commands.Add(Command(Type, str));
                }
                catch (std::domain_error&)
                {
                    print("Invalid input.");
                }
            }
        }
    }
    void Start()
    {
        Thread = std::thread([&] {
            for (int i = 0; i < Commands.GetCount(); i++)
                ExecuteCommand(Commands.GetItem(i));
            print(ID << ": done, destroying all local guards...");
            LockGuards.ForEach([](std::string key, SmartMutex::LockGuard * value) { delete value; });
            SharedLockGuards.ForEach([](std::string key, SmartMutex::SharedLockGuard * value) { delete value; });
        });
    }
    void Join()
    {
        Thread.join();
    }
};

int main()
{
    std::vector<std::shared_ptr<TestThread>> Threads;
    while (true)
    {
        while (true)
        {
            std::string str;
            print("Enter c to clear, n to create a new thread, s to start, or q (or e) to quit:");
            input(str);
            if (str == "c") Threads.clear();
            else if (str == "n") Threads.push_back(std::shared_ptr<TestThread>(new TestThread()));
            else if (str == "s") break;
            else if (str == "q" || str == "e") return 0;
            else print("Invalid input.");
        }
        for (auto t : Threads) t->Start();
        for (auto t : Threads) t->Join();
        GlobalLockGuards.ForEach([](std::string key, SmartMutex::LockGuard * value) { delete value; });
        GlobalSharedLockGuards.ForEach([](std::string key, SmartMutex::SharedLockGuard * value) { delete value; });
    }
}
