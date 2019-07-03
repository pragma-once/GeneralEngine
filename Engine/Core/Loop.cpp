#include "../Engine.h"
#include <chrono>
#include <condition_variable>

namespace Engine
{
    namespace Core
    {
        Loop::Loop() : ZeroPriorityModulesStartIndex(0), ZeroPriorityModulesEndIndex(0),
                                 isRunning(false), Time(0), TimeDiff(0), TimeFloat(0), TimeDiffFloat(0),
                                 ShouldStop(false),
                                 Modules(

                // OnAdd
                [this](Data::Collections::List<Module*> * Parent, Module *& Item, int& Index)
                {
                    // Don't add a module if it already exists in the list
                    if (Parent->Contains(Item))
                        throw std::invalid_argument("The module is already added to the list.");

                    // Decide where to place the module in the list
                    if (Item->GetPriority() == 0) // 0 priority is more common
                    {
                        if (Index > ZeroPriorityModulesEndIndex)
                            Index = ZeroPriorityModulesEndIndex;
                        else if (Index < ZeroPriorityModulesStartIndex)
                            Index = ZeroPriorityModulesStartIndex;

                        ZeroPriorityModulesEndIndex++;
                    }
                    else // binary search
                    {
                        int s = Item->GetPriority() < 0 ? 0 : ZeroPriorityModulesEndIndex;
                        int e = Item->GetPriority() < 0 ? ZeroPriorityModulesStartIndex : Parent->GetCount();

                        if (Index < s || Index > e
                            || (Index != s && Parent->GetItem(Index - 1)->GetPriority() > Item->GetPriority())
                            || (Index != e && Item->GetPriority() > Parent->GetItem(Index)->GetPriority()))
                        {
                            while (s < e)
                            {
                                int c = (s + e) / 2;

                                if (c > s && Parent->GetItem(c - 1)->GetPriority() > Item->GetPriority())
                                    e = c - 1;
                                else if (Item->GetPriority() > Parent->GetItem(c)->GetPriority())
                                    s = c + 1;
                                else if (c > Index)
                                    e = c;
                                else if (c < Index)
                                    if (c == s)
                                        if (Parent->GetItem(c)->GetPriority() <= Item->GetPriority())
                                            s = e;
                                        else
                                            e = s;
                                    else
                                        s = c;
                                else
                                    s = e = c;
                            }

                            Index = s;
                        }

                        if (Item->GetPriority() < 0)
                        {
                            ZeroPriorityModulesStartIndex++;
                            ZeroPriorityModulesEndIndex++;
                        }
                    }

                    // Add the module
                    Parent->Add(Item, Index);

                    Item->Acquire(this);
                    auto guard = isRunning.Mutex.GetSharedLock();
                    if (isRunning)
                        Item->_Start();
                    guard.Unlock();
                },

                // OnSetItem
                [this](Data::Collections::List<Module*> * Parent, int& Index, Module *& Value)
                {
                    try
                    {
                        if ((Index == 0 || Parent->GetItem(Index - 1)->GetPriority() <= Value->GetPriority())
                            && (Index == Parent->GetCount() - 1 || Value->GetPriority() <= Parent->GetItem(Index + 1)->GetPriority()))
                        {
                            auto guard = isRunning.Mutex.GetSharedLock();
                            if (isRunning)
                                Parent->GetItem(Index)->_Stop();
                            guard.Unlock();
                            Parent->GetItem(Index)->Release();

                            Parent->SetItem(Index, Value);

                            Value->Acquire(this);
                            guard = isRunning.Mutex.GetSharedLock();
                            if (isRunning)
                                Value->_Start();
                            guard.Unlock();
                        }
                        else throw std::invalid_argument("Module's priority doesn't match the index.");
                    }
                    catch (std::exception& e) { throw e; } // Out of range Index
                },

                // OnRemove
                [this](Data::Collections::List<Module*> * Parent, int& Index)
                {
                    try
                    {
                        Module * Item = Parent->GetItem(Index);

                        auto guard = isRunning.Mutex.GetSharedLock();
                        if (isRunning)
                            Item->_Stop();
                        guard.Unlock();
                        Item->Release();

                        int Priority = Item->GetPriority();
                        Parent->RemoveByIndex(Index);
                        if (Priority <= 0)
                            ZeroPriorityModulesEndIndex--;
                        if (Priority < 0)
                            ZeroPriorityModulesStartIndex--;
                    }
                    catch (std::exception& e) { throw e; } // Out of range Index
                },

                // OnClear
                [this](Data::Collections::List<Module*> * Parent)
                {
                    auto guard = isRunning.Mutex.GetSharedLock();
                    if (isRunning) Parent->ForEach([](Module * Item) {
                        Item->_Stop();
                        Item->Release();
                    });
                    else Parent->ForEach([](Module * Item) {
                        Item->Release();
                    });
                    guard.Unlock();

                    Parent->Clear();
                    ZeroPriorityModulesStartIndex = 0;
                    ZeroPriorityModulesEndIndex = 0;
                }
            )
        {

        }

        void Loop::Run()
        {
            // Mutex lock to: 1. Not allowing more than one async starts.
            //                2. Provide thread safety for _Start and _Stop calls
            //                   on modifications on the Modules list.
            auto guard = isRunning.Mutex.GetLock();

            if (isRunning)
                throw std::logic_error("Cannot start twice.");

            auto StartTime = std::chrono::steady_clock::now();
            double PreviousTime = 0;
            Time = 0;
            TimeDiff = 0;
            TimeFloat = 0;
            TimeDiffFloat = 0;

            ShouldStop = false;
            Schedules.Clear();
            Schedules.SetAutoShrink(false);

            Data::Collections::List<Module*> copy_list = Modules;
            isRunning = true;
            copy_list.ForEach([](Module * Item) { Item->_Start(); }); // TODO: Analyze, What if some module gets removed on start
                                                                      // Potential fix: by scheduling Start and End calls when IsRunning?
                                                                      // Or just add them to Start/End priority*-queues to schedule
                                                                      // When removing a started module, it's not added to the ending priority-queue,
                                                                      // and removed from the starting priority-queue.
                                                                      // Also! SCHEDULE to add to or remove from a separate updating modules list
                                                                      // so that they can be used to decide what modules to update.
                                                                      // ALSO! do it for schedules!

            guard.Unlock();

            copy_list.Clear();

            // Threads initialization

            unsigned int threads_count = std::thread::hardware_concurrency();
            if (threads_count == 0) threads_count = 1;
            enum thread_state
            {
                /// @brief Should start working, set by the main thread
                ready,
                /// @brief Is working, set by each thread in the pool
                working,
                /// @brief Waiting for the main thread to continue, set by each thread
                passing,
                /// @brief Current priority done, set by each thread
                done,
                /// @brief Used to detect bugs
                error
            };
            Data::Collections::List<std::thread*, false> threads(threads_count);
            Data::Collections::List<thread_state> thread_states(threads_count);
            threads.SetAutoShrink(false);
            thread_states.SetAutoShrink(false);

            std::condition_variable condition;
            std::mutex condition_mutex;

            Data::Shared<int> CurrentPriority = -128; // Only set by the main thread
            Data::Shared<int, true> ModuleIndex = 0;
            Data::Shared<bool> ShouldTerminate = false;

            for (int i = 0; i < threads_count; i++)
            {
                thread_states.Add(done);
                // Update loop: thread pool
                threads.Add(new std::thread([&](int thread_index) {
                    while (true)
                    {
                        std::unique_lock<std::mutex> condition_guard(condition_mutex);
                        condition.wait(condition_guard, [&]() {
                            return thread_states.GetItem(thread_index) == thread_state::ready;
                        });
                        if (ShouldTerminate)
                        {
                            thread_states.SetItem(thread_index, thread_state::done);
                            return;
                        }
                        thread_states.SetItem(thread_index, thread_state::working);
                        condition_guard.unlock();
                        if (CurrentPriority == 0)
                        {
                            std::function<void()> func;
                            auto guard = ModuleIndex.Mutex.GetLock();
                            bool done_for_now = false;
                            while (!Schedules.IsEmpty())
                            {
                                func = nullptr;
                                if (Schedules.GetFirstPriority() <= Time)
                                    switch (Schedules.GetFirstItem().first)
                                    {
                                        case ExecutionType::FreeAsync:
                                            std::thread([&]() { Schedules.Pop().second(); }).detach();
                                            break;
                                        case ExecutionType::BoundedAsync:
                                            func = Schedules.Pop().second;
                                            break;
                                        case ExecutionType::SingleThreaded:
                                            condition_guard.lock();
                                            thread_states.SetItem(thread_index, thread_state::passing);
                                            condition_guard.unlock();
                                            condition.notify_all();
                                            done_for_now = true;
                                            break;
                                    }
                                else break;
                                guard.Unlock();
                                if (done_for_now) break;
                                if (func != nullptr) func();
                                guard = ModuleIndex.Mutex.GetLock();
                            }
                            // Wait for the main thread to continue to run the schedules.
                            // And do not continue to the modules yet.
                            if (done_for_now) continue;
                        }
                        {
                            int module_index;
                            auto guard = ModuleIndex.Mutex.GetLock();
                            bool done_for_now = false;
                            while (ModuleIndex < Modules.GetCount()
                                && CurrentPriority == Modules.GetItem(ModuleIndex)->GetPriority())
                            {
                                module_index = -1;
                                switch (Modules.GetItem(ModuleIndex)->GetExecutionType())
                                {
                                    case ExecutionType::FreeAsync:
                                        std::thread([&]() { Modules.GetItem(ModuleIndex)->OnUpdate(); }).detach();
                                        ModuleIndex = ModuleIndex + 1;
                                        break;
                                    case ExecutionType::BoundedAsync:
                                        module_index = ModuleIndex;
                                        ModuleIndex = ModuleIndex + 1;
                                        break;
                                    case ExecutionType::SingleThreaded:
                                        condition_guard.lock();
                                        thread_states.SetItem(thread_index, thread_state::passing);
                                        condition_guard.unlock();
                                        condition.notify_all();
                                        done_for_now = true;
                                        break;
                                }
                                guard.Unlock();
                                if (done_for_now) break;
                                if (module_index != -1) Modules.GetItem(module_index)->OnUpdate();
                                guard = ModuleIndex.Mutex.GetLock();
                            }
                            // Wait for the main thread to continue to run the modules.
                            // And do not set the state to 'done' yet.
                            if (done_for_now) continue;
                        }
                        condition_guard.lock();
                        thread_states.SetItem(thread_index, thread_state::done);
                        condition_guard.unlock();
                        condition.notify_all();
                    }
                }, i));
            }

            auto pool_process = [&]()->thread_state {
                // start
                std::unique_lock<std::mutex> condition_guard(condition_mutex);
                for (int i = 0; i < threads_count; i++)
                    thread_states.SetItem(i, thread_state::ready);
                condition_guard.unlock();
                condition.notify_all();
                // wait
                thread_state result = thread_state::error;
                condition_guard.lock();
                condition.wait(condition_guard, [&]() {
                    int done_count = 0;
                    int passing_count = 0;
                    for (int i = 0; i < threads_count; i++)
                        switch (thread_states.GetItem(i))
                        {
                            case thread_state::done: done_count++; break;
                            case thread_state::passing: passing_count++; break;
                            default: return false;
                        }
                    if (done_count == threads_count) result = thread_state::done;
                    else if (passing_count == threads_count) result = thread_state::passing;
                    else result = thread_state::error;
                    return true;
                });
                if (result == thread_state::error)
                    throw std::logic_error("This is a bug!");
                return result;
            };

            thread_state pool_state = thread_state::done;

            // Update loop: main thread
            while (!ShouldStop)
            {
                auto duration = std::chrono::steady_clock::now() - StartTime;
                PreviousTime = Time;
                Time = (double)std::chrono::duration_cast<std::chrono::microseconds>(duration).count() / 1000000.0;
                TimeDiff = Time - PreviousTime;
                TimeFloat = (float)Time;
                TimeDiffFloat = (float)TimeDiff;

                CurrentPriority = -128;
                auto guard = ModuleIndex.Mutex.GetLock();
                ModuleIndex = 0;
                guard.Unlock();

                while (true)
                {
                    // Set ModuleIndex
                    if (ModuleIndex >= Modules.GetCount())
                        if (CurrentPriority <= 0)
                            CurrentPriority = 0;
                        else break;
                    else if (CurrentPriority < Modules.GetItem(ModuleIndex)->GetPriority())
                        if (CurrentPriority <= 0 && Modules.GetItem(ModuleIndex)->GetPriority() > 0)
                            CurrentPriority = 0;
                        else CurrentPriority = Modules.GetItem(ModuleIndex)->GetPriority();
                    //  Normal process
                    if (CurrentPriority == 0)
                    {
                        std::function<void()> func;
                        auto guard = ModuleIndex.Mutex.GetLock();
                        bool pass_to_pool = false;
                        bool priority_done = false;
                        while (!Schedules.IsEmpty())
                        {
                            func = nullptr;
                            if (Schedules.GetFirstPriority() <= Time)
                                switch (Schedules.GetFirstItem().first)
                                {
                                    case ExecutionType::FreeAsync:
                                        std::thread([&]() { Schedules.Pop().second(); }).detach();
                                        break;
                                    case ExecutionType::SingleThreaded:
                                        func = Schedules.Pop().second;
                                        break;
                                    case ExecutionType::BoundedAsync:
                                        pass_to_pool = true;
                                        break;
                                }
                            else break;
                            guard.Unlock();
                            if (pass_to_pool)
                            {
                                priority_done = pool_process() == thread_state::done;
                                if (priority_done) break;
                                pass_to_pool = false;
                            }
                            if (func != nullptr) func();
                            guard = ModuleIndex.Mutex.GetLock();
                        }
                        if (priority_done)
                        {
                            // 0-priority is done now.
                            CurrentPriority = CurrentPriority + 1;
                            continue;
                        }
                    }
                    {
                        int module_index;
                        auto guard = ModuleIndex.Mutex.GetLock();
                        bool pass_to_pool = false;
                        bool priority_done = false;
                        while (ModuleIndex < Modules.GetCount()
                            && CurrentPriority == Modules.GetItem(ModuleIndex)->GetPriority())
                        {
                            module_index = -1;
                            switch (Modules.GetItem(ModuleIndex)->GetExecutionType())
                            {
                                case ExecutionType::FreeAsync:
                                    std::thread([&]() { Modules.GetItem(ModuleIndex)->OnUpdate(); }).detach();
                                    ModuleIndex = ModuleIndex + 1;
                                    break;
                                case ExecutionType::SingleThreaded:
                                    module_index = ModuleIndex;
                                    ModuleIndex = ModuleIndex + 1;
                                    break;
                                case ExecutionType::BoundedAsync:
                                    pass_to_pool = true;
                                    break;
                            }
                            guard.Unlock();
                            if (pass_to_pool)
                            {
                                priority_done = pool_process() == thread_state::done;
                                if (priority_done) break;
                                pass_to_pool = false;
                            }
                            if (module_index != -1) Modules.GetItem(module_index)->OnUpdate();
                            guard = ModuleIndex.Mutex.GetLock();
                        }
                        if (priority_done)
                        {
                            // 0-priority is done now.
                            CurrentPriority = CurrentPriority + 1;
                            continue;
                        }
                    }
                    CurrentPriority = CurrentPriority + 1;
                }

                if (Modules.GetCount() == 0)
                    break;
            }

            ShouldTerminate = true;
            pool_process();
            for (int i = 0; i < threads_count; i++)
            {
                threads.GetItem(i)->join();
                delete threads.GetItem(i);
            }

            copy_list = Modules;

            guard = isRunning.Mutex.GetLock();

            isRunning = false;
            copy_list.ForEach([](Module * Item) { Item->_Stop(); }); // TODO: Analyze
            copy_list.Clear();

            Schedules.SetAutoShrink(true);
            Schedules.Clear();

            Time = 0;
            TimeDiff = 0;
            TimeFloat = 0;
            TimeDiffFloat = 0;

            guard.Unlock();
        }

        void Loop::Stop()
        {
            ShouldStop = true;
        }

        bool Loop::IsRunning()
        {
            return isRunning;
        }

        void Loop::Schedule(std::function<void()> Func, double Time, ExecutionType ExecutionType)
        {
            Schedules.Push(std::pair(ExecutionType, Func), Time);
        }

        void Loop::Schedule(double Time, std::function<void()> Func, ExecutionType ExecutionType)
        {
            Schedules.Push(std::pair(ExecutionType, Func), Time);
        }

        void Loop::Schedule(double Time, ExecutionType ExecutionType, std::function<void()> Func)
        {
            Schedules.Push(std::pair(ExecutionType, Func), Time);
        }
    }
}
