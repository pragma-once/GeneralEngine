// ----------------------------------------------------------------

// Example test inputs:

// dead-lock and live-lock exceptions:
// c n sl 0 s 500 l 0 s 500 d n sl 0 s 500 tl 0 s 500 d n sl 0 s 500 l 0 s 500 d s

// ----------------------------------------------------------------

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
    GlobalSharedUnlock,
    TryLock,
    TrySharedLock,
    GlobalTryLock,
    GlobalTrySharedLock
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
    else if (name == "tl") return TryLock;
    else if (name == "tsl") return TrySharedLock;
    else if (name == "gtl") return GlobalTryLock;
    else if (name == "gtsl") return GlobalTrySharedLock;
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
        std::string expanded_guard_id = "local" + std::to_string(ID) + "-" + guard_id;
        try
        {
            LockGuards.SetValue(guard_id, new SmartMutex::LockGuard(m.GetLock()));
            print(ID << ": locked: " << expanded_guard_id);
        }
        catch (std::exception& e)
        {
            print(ID << ": exception on lock attempt, " << expanded_guard_id << ": " << e.what());
        }
    }
    void Unlock(std::string guard_id)
    {
        std::string expanded_guard_id = "local" + std::to_string(ID) + "-" + guard_id;
        delete LockGuards.GetValue(guard_id);
        LockGuards.Remove(guard_id);
        print(ID << ": unlocked: " << expanded_guard_id);
    }
    void SharedLock(std::string guard_id)
    {
        std::string expanded_guard_id = "local" + std::to_string(ID) + "-shared-" + guard_id;
        SharedLockGuards.SetValue(guard_id, new SmartMutex::SharedLockGuard(m.GetSharedLock()));
        print(ID << ": shared-locked: " << expanded_guard_id);
    }
    void SharedUnlock(std::string guard_id)
    {
        std::string expanded_guard_id = "local" + std::to_string(ID) + "-shared-" + guard_id;
        delete SharedLockGuards.GetValue(guard_id);
        SharedLockGuards.Remove(guard_id);
        print(ID << ": shared-unlocked: " << expanded_guard_id);
    }
    void TryLock(std::string guard_id)
    {
        std::string expanded_guard_id = "local" + std::to_string(ID) + "-" + guard_id;
        try
        {
            SmartMutex::LockGuard guard;
            if (m.TryGetLock(guard))
            {
                LockGuards.SetValue(guard_id, new SmartMutex::LockGuard(guard));
                print(ID << ": try-lock successful: " << expanded_guard_id);
            }
            else print(ID << ": try-lock failed: " << expanded_guard_id);
        }
        catch (std::exception& e)
        {
            print(ID << ": exception on try-lock attempt, " << expanded_guard_id << ": " << e.what());
        }
    }
    void TrySharedLock(std::string guard_id)
    {
        std::string expanded_guard_id = "local" + std::to_string(ID) + "-shared-" + guard_id;
        SmartMutex::SharedLockGuard guard;
        if (m.TryGetSharedLock(guard))
        {
            SharedLockGuards.SetValue(guard_id, new SmartMutex::SharedLockGuard(guard));
            print(ID << ": try-shared-lock successful: " << expanded_guard_id);
        }
        else print(ID << ": try-shared-lock failed: " << expanded_guard_id);
    }

    void GlobalLock(std::string guard_id)
    {
        std::string expanded_guard_id = "global-" + guard_id;
        try
        {
            GlobalLockGuards.SetValue(guard_id, new SmartMutex::LockGuard(m.GetLock()));
            print(ID << ": locked: " << expanded_guard_id);
        }
        catch (std::exception& e)
        {
            print(ID << ": exception on lock attempt, " << expanded_guard_id << ": " << e.what());
        }
    }
    void GlobalUnlock(std::string guard_id)
    {
        std::string expanded_guard_id = "global-" + guard_id;
        delete GlobalLockGuards.GetValue(guard_id);
        GlobalLockGuards.Remove(guard_id);
        print(ID << ": unlocked: " << expanded_guard_id);
    }
    void GlobalSharedLock(std::string guard_id)
    {
        std::string expanded_guard_id = "global-shared-" + guard_id;
        GlobalSharedLockGuards.SetValue(guard_id, new SmartMutex::SharedLockGuard(m.GetSharedLock()));
        print(ID << ": shared-locked: " << expanded_guard_id);
    }
    void GlobalSharedUnlock(std::string guard_id)
    {
        std::string expanded_guard_id = "global-shared-" + guard_id;
        delete GlobalSharedLockGuards.GetValue(guard_id);
        GlobalSharedLockGuards.Remove(guard_id);
        print(ID << ": shared-unlocked: " << expanded_guard_id);
    }
    void GlobalTryLock(std::string guard_id)
    {
        std::string expanded_guard_id = "global-" + guard_id;
        try
        {
            SmartMutex::LockGuard guard;
            if (m.TryGetLock(guard))
            {
                GlobalLockGuards.SetValue(guard_id, new SmartMutex::LockGuard(guard));
                print(ID << ": try-lock successful: " << expanded_guard_id);
            }
            else print(ID << ": try-lock failed: " << expanded_guard_id);
        }
        catch (std::exception& e)
        {
            print(ID << ": exception on try-lock attempt, " << expanded_guard_id << ": " << e.what());
        }
    }
    void GlobalTrySharedLock(std::string guard_id)
    {
        std::string expanded_guard_id = "global-shared-" + guard_id;
        SmartMutex::SharedLockGuard guard;
        if (m.TryGetSharedLock(guard))
        {
            GlobalSharedLockGuards.SetValue(guard_id, new SmartMutex::SharedLockGuard(guard));
            print(ID << ": try-shared-lock successful: " << expanded_guard_id);
        }
        else print(ID << ": try-shared-lock failed: " << expanded_guard_id);
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
            case CommandType::TryLock: TryLock(cmd.GuardId); break;
            case CommandType::TrySharedLock: TrySharedLock(cmd.GuardId); break;
            case CommandType::GlobalTryLock: GlobalTryLock(cmd.GuardId); break;
            case CommandType::GlobalTrySharedLock: GlobalTrySharedLock(cmd.GuardId); break;
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
            print("Command:");
            print("  s <milliseconds>          => Sleep");
            print("  <mutex_command> <guard_id> => Mutex commands");
            print("Local guard mutex commands:");
            print("  l:  Lock,    u: Unlock, sl:  SharedLock,    su: SharedUnlock");
            print("  tl: TryLock,            tsl: TrySharedLock");
            print("Global guard mutex commands");
            print("  gl:  Lock,    gu: Unlock, gsl:  SharedLock,    gsu: SharedUnlock");
            print("  gtl: TryLock,             gtsl: TrySharedLock");
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
            if (str == "c") { NextThreadID = 0; Threads.clear(); }
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
