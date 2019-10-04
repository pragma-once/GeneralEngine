// ----------------------------------------------------------------

// Example test inputs:

// dead-lock and live-lock exceptions:
// c n sl 0 s 500 l 0 s 500 d n sl 0 s 500 tl 0 s 500 d n sl 0 s 500 l 0 s 500 d s

// ----------------------------------------------------------------

#include "../../Engine/Engine.h"
#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

using namespace std::literals::chrono_literals;
using namespace Engine::Utilities;

#define print(context) (std::cout << context << '\n')
#define input(var) (std::cin >> var)

/// @brief The mutex that is tested by TestThreads.
SmartMutex GlobalMutex;

/// @brief Used to assign IDs to new TestThreads.
Shared<int, true> NextThreadID = 0;
/// @brief Gets the next ID to be assigned to the newly created TestThreads.
int GetNextThreadID()
{
    auto guard = NextThreadID.Mutex.GetLock();
    int id = NextThreadID;
    NextThreadID = NextThreadID + 1;
    return id;
}

Collections::Dictionary<std::string, SmartMutex::LockGuard*> GlobalLockGuards;
Collections::Dictionary<std::string, SmartMutex::SharedLockGuard*> GlobalSharedLockGuards;

/// @brief The command types available to use in a TestThread.
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

/// @brief Converts a short command name from CLI to CommandType to use in TestThread.
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
    /// @brief Ignored if the CommandType is Sleep.
    std::string GuardId;
    /// @brief Sleep duration in milliseconds. Only used if the CommandType is Sleep.
    double SleepDuration;
    Command() {}
    /// @brief Constructs a Sleep type command.
    Command(double SleepDuration) : Type(CommandType::Sleep), SleepDuration(SleepDuration) {}
    Command(CommandType Type, std::string GuardId) : Type(Type), GuardId(GuardId) {}
};

/// @brief Used to create test threads containing command lists by user.
class TestThread
{
private:
    int ID;
    Collections::Dictionary<std::string, SmartMutex::LockGuard*> LockGuards;
    Collections::Dictionary<std::string, SmartMutex::SharedLockGuard*> SharedLockGuards;
    std::vector<Command> Commands;
    std::thread Thread;

    /// @brief Ensures that a guard dictionary contains a GuardID
    template <typename GuardType> void EnsureGuardExistence(
            Collections::Dictionary<std::string, GuardType*>& Guards,
            const std::string& GuardID
        )
    {
        if (!Guards.Contains(GuardID)) Guards.SetValue(GuardID, new GuardType());
    }

    /// @brief Implementation of commands of type CommandType::Lock
    void Lock(std::string guard_id)
    {
        std::string expanded_guard_id = "local" + std::to_string(ID) + "-" + guard_id;
        try
        {
            EnsureGuardExistence(LockGuards, guard_id);
            *LockGuards.GetValue(guard_id) = GlobalMutex.GetLock();
            print(ID << ": locked: " << expanded_guard_id);
        }
        catch (std::exception& e) // SmartMutex::DeadLockException
        {
            print(ID << ": exception on lock attempt, " << expanded_guard_id << ": " << e.what());
        }
    }

    /// @brief Implementation of commands of type CommandType::Unlock
    void Unlock(std::string guard_id)
    {
        std::string expanded_guard_id = "local" + std::to_string(ID) + "-" + guard_id;
        try
        {
            LockGuards.GetValue(guard_id)->Unlock();
            print(ID << ": unlocked: " << expanded_guard_id);
        }
        catch (std::exception& e) // std::domain_error("Key not found.") thrown by LockGuards
        {
            print(ID << ": exception on unlock attempt, " << expanded_guard_id << ": " << e.what());
            print(ID << ": hint: Are you unlocking an existing guard? "
                        "LockGuards are created when you lock (l or tl).");
        }
    }

    /// @brief Implementation of commands of type CommandType::SharedLock
    void SharedLock(std::string guard_id)
    {
        std::string expanded_guard_id = "local" + std::to_string(ID) + "-shared-" + guard_id;
        EnsureGuardExistence(SharedLockGuards, guard_id);
        *SharedLockGuards.GetValue(guard_id) = GlobalMutex.GetSharedLock();
        print(ID << ": shared-locked: " << expanded_guard_id);
    }

    /// @brief Implementation of commands of type CommandType::SharedUnlock
    void SharedUnlock(std::string guard_id)
    {
        std::string expanded_guard_id = "local" + std::to_string(ID) + "-shared-" + guard_id;
        try
        {
            SharedLockGuards.GetValue(guard_id)->Unlock();
            print(ID << ": shared-unlocked: " << expanded_guard_id);
        }
        catch (std::exception& e) // std::domain_error("Key not found.") thrown by SharedLockGuards
        {
            print(ID << ": exception on shared-unlock attempt, " << expanded_guard_id << ": " << e.what());
            print(ID << ": hint: Are you unlocking an existing guard? "
                        "SharedLockGuards are created when you shared-lock (sl or tsl).");
        }
    }

    /// @brief Implementation of commands of type CommandType::TryLock
    void TryLock(std::string guard_id)
    {
        std::string expanded_guard_id = "local" + std::to_string(ID) + "-" + guard_id;
        try
        {
            EnsureGuardExistence(LockGuards, guard_id);
            SmartMutex::LockGuard guard;
            if (GlobalMutex.TryGetLock(guard))
            {
                *LockGuards.GetValue(guard_id) = guard;
                print(ID << ": try-lock successful: " << expanded_guard_id);
            }
            else print(ID << ": try-lock failed: " << expanded_guard_id);
        }
        catch (std::exception& e) // SmartMutex::PossibleLivelockException
        {
            print(ID << ": exception on try-lock attempt, " << expanded_guard_id << ": " << e.what());
        }
    }

    /// @brief Implementation of commands of type CommandType::TrySharedLock
    void TrySharedLock(std::string guard_id)
    {
        std::string expanded_guard_id = "local" + std::to_string(ID) + "-shared-" + guard_id;
        EnsureGuardExistence(SharedLockGuards, guard_id);
        SmartMutex::SharedLockGuard guard;
        if (GlobalMutex.TryGetSharedLock(guard))
        {
            *SharedLockGuards.GetValue(guard_id) = guard;
            print(ID << ": try-shared-lock successful: " << expanded_guard_id);
        }
        else print(ID << ": try-shared-lock failed: " << expanded_guard_id);
    }

    /// @brief Implementation of commands of type CommandType::GlobalLock
    void GlobalLock(std::string guard_id)
    {
        std::string expanded_guard_id = "global-" + guard_id;
        try
        {
            EnsureGuardExistence(GlobalLockGuards, guard_id);
            *GlobalLockGuards.GetValue(guard_id) = GlobalMutex.GetLock();
            print(ID << ": locked: " << expanded_guard_id);
        }
        catch (std::exception& e) // SmartMutex::DeadLockException
        {
            print(ID << ": exception on lock attempt, " << expanded_guard_id << ": " << e.what());
        }
    }

    /// @brief Implementation of commands of type CommandType::GlobalUnlock
    void GlobalUnlock(std::string guard_id)
    {
        std::string expanded_guard_id = "global-" + guard_id;
        try
        {
            GlobalLockGuards.GetValue(guard_id)->Unlock();
            print(ID << ": unlocked: " << expanded_guard_id);
        }
        catch (std::exception& e) // std::domain_error("Key not found.") thrown by GlobalLockGuards
        {
            print(ID << ": exception on unlock attempt, " << expanded_guard_id << ": " << e.what());
            print(ID << ": hint: Are you unlocking an existing guard? "
                        "Global LockGuards are created when you lock globally (gl or gtl).");
        }
    }

    /// @brief Implementation of commands of type CommandType::GlobalSharedLock
    void GlobalSharedLock(std::string guard_id)
    {
        std::string expanded_guard_id = "global-shared-" + guard_id;
        EnsureGuardExistence(GlobalSharedLockGuards, guard_id);
        *GlobalSharedLockGuards.GetValue(guard_id) = GlobalMutex.GetSharedLock();
        print(ID << ": shared-locked: " << expanded_guard_id);
    }

    /// @brief Implementation of commands of type CommandType::GlobalSharedUnlock
    void GlobalSharedUnlock(std::string guard_id)
    {
        std::string expanded_guard_id = "global-shared-" + guard_id;
        try
        {
            GlobalSharedLockGuards.GetValue(guard_id)->Unlock();
            print(ID << ": shared-unlocked: " << expanded_guard_id);
        }
        catch (std::exception& e) // std::domain_error("Key not found.") thrown by GlobalSharedLockGuards
        {
            print(ID << ": exception on shared-unlock attempt, " << expanded_guard_id << ": " << e.what());
            print(ID << ": hint: Are you unlocking an existing guard? "
                        "Global SharedLockGuards are created when you shared-lock globally (gsl or gtsl).");
        }
    }

    /// @brief Implementation of commands of type CommandType::GlobalTryLock
    void GlobalTryLock(std::string guard_id)
    {
        std::string expanded_guard_id = "global-" + guard_id;
        try
        {
            EnsureGuardExistence(GlobalLockGuards, guard_id);
            SmartMutex::LockGuard guard;
            if (GlobalMutex.TryGetLock(guard))
            {
                *GlobalLockGuards.GetValue(guard_id) = guard;
                print(ID << ": try-lock successful: " << expanded_guard_id);
            }
            else print(ID << ": try-lock failed: " << expanded_guard_id);
        }
        catch (std::exception& e) // SmartMutex::PossibleLivelockException
        {
            print(ID << ": exception on try-lock attempt, " << expanded_guard_id << ": " << e.what());
        }
    }

    /// @brief Implementation of commands of type CommandType::GlobalTrySharedLock
    void GlobalTrySharedLock(std::string guard_id)
    {
        std::string expanded_guard_id = "global-shared-" + guard_id;
        EnsureGuardExistence(GlobalSharedLockGuards, guard_id);
        SmartMutex::SharedLockGuard guard;
        if (GlobalMutex.TryGetSharedLock(guard))
        {
            *GlobalSharedLockGuards.GetValue(guard_id) = guard;
            print(ID << ": try-shared-lock successful: " << expanded_guard_id);
        }
        else print(ID << ": try-shared-lock failed: " << expanded_guard_id);
    }

    void ExecuteCommand(Command cmd)
    {
        switch (cmd.Type)
        {
            case CommandType::Sleep:               std::this_thread::sleep_for(cmd.SleepDuration * 1ms); break;
            // ------------------------------------
            case CommandType::Lock:                Lock(cmd.GuardId);               break;
            case CommandType::Unlock:              Unlock(cmd.GuardId);             break;
            case CommandType::SharedLock:          SharedLock(cmd.GuardId);         break;
            case CommandType::SharedUnlock:        SharedUnlock(cmd.GuardId);       break;
            case CommandType::GlobalLock:          GlobalLock(cmd.GuardId);         break;
            case CommandType::GlobalUnlock:        GlobalUnlock(cmd.GuardId);       break;
            case CommandType::GlobalSharedLock:    GlobalSharedLock(cmd.GuardId);   break;
            case CommandType::GlobalSharedUnlock:  GlobalSharedUnlock(cmd.GuardId); break;
            // ------------------------------------
            case CommandType::TryLock:             TryLock(cmd.GuardId);             break;
            case CommandType::TrySharedLock:       TrySharedLock(cmd.GuardId);       break;
            case CommandType::GlobalTryLock:       GlobalTryLock(cmd.GuardId);       break;
            case CommandType::GlobalTrySharedLock: GlobalTrySharedLock(cmd.GuardId); break;
        }
    }

public:
    /// @brief Constructs the TestThread by prompting the user to add the commands.
    TestThread()
    {
        ID = GetNextThreadID();
        while (true)
        {
            std::string str;
            double number;
            std::string guard_id;

            print("Command:");
            print("  s <milliseconds>           => Sleep");
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
                Commands.push_back(Command(number));
            }
            else
            {
                try
                {
                    CommandType Type = NameToCommandType(str);
                    input(str);
                    Commands.push_back(Command(Type, str));
                }
                catch (std::domain_error&) // thrown by NameToCommandType
                {
                    print("Invalid input.");
                }
            }
        }
    }

    /// @brief Starts the thread.
    void Start()
    {
        Thread = std::thread([&] {
            for (auto command : Commands)
                ExecuteCommand(command);
            print(ID << ": done, destroying all local guards...");
            LockGuards.ForEach([](std::string key, SmartMutex::LockGuard * value) { delete value; });
            LockGuards.Clear();
            SharedLockGuards.ForEach([](std::string key, SmartMutex::SharedLockGuard * value) { delete value; });
            SharedLockGuards.Clear();
        });
    }

    /// @brief Waits for the thread to join.
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
        if (Threads.size() == 0) print("No threads to execute.");
        for (auto t : Threads) t->Start();
        for (auto t : Threads) t->Join();
        GlobalLockGuards.ForEach([](std::string key, SmartMutex::LockGuard * value) { delete value; });
        GlobalLockGuards.Clear();
        GlobalSharedLockGuards.ForEach([](std::string key, SmartMutex::SharedLockGuard * value) { delete value; });
        GlobalSharedLockGuards.Clear();
    }
}
