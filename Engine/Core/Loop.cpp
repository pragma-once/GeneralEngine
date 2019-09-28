#include "../Engine.h"
#include <chrono>

namespace Engine
{
    namespace Core
    {
        Loop::Loop() : ZeroPriorityModulesStartIndex(0), ZeroPriorityModulesEndIndex(0),
                                 isRunning(false),
                                 StartTime(std::chrono::time_point<std::chrono::steady_clock>()),
                                 Time(0), TimeDiff(0), TimeAsFloat(0), TimeDiffAsFloat(0),
                                 ShouldStop(false),
                                 Modules(

                // OnAdd
                [this](Utilities::Collections::List<Module*> * Parent, Module *& Item, int& Index)
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
                    if (isRunning)
                        // This kind of std::tuple construction requires C++17
                        ToEditModules.Push(std::tuple(Add, Index, Item));
                },

                // OnSetItem
                [this](Utilities::Collections::List<Module*> * Parent, int& Index, Module *& Value)
                {
                    try
                    {
                        if ((Index == 0 || Parent->GetItem(Index - 1)->GetPriority() <= Value->GetPriority())
                            && (Index == Parent->GetCount() - 1 || Value->GetPriority() <= Parent->GetItem(Index + 1)->GetPriority()))
                        {
                            Parent->SetItem(Index, Value);
                            if (isRunning)
                                ToEditModules.Push(std::tuple(Replace, Index, Value));
                        }
                        else throw std::invalid_argument("Module's priority doesn't match the index.");
                    }
                    catch (std::exception& e) { throw e; } // Out of range Index
                },

                // OnRemove
                [this](Utilities::Collections::List<Module*> * Parent, int& Index)
                {
                    try
                    {
                        int Priority = Parent->GetItem(Index)->GetPriority();

                        Parent->RemoveByIndex(Index);
                        if (isRunning)
                            ToEditModules.Push(std::tuple(Remove, Index, nullptr));

                        if (Priority <= 0)
                            ZeroPriorityModulesEndIndex--;
                        if (Priority < 0)
                            ZeroPriorityModulesStartIndex--;
                    }
                    catch (std::exception& e) { throw e; } // Out of range Index
                },

                // OnClear
                [this](Utilities::Collections::List<Module*> * Parent)
                {
                    Parent->Clear();
                    if (isRunning)
                        ToEditModules.Push(std::tuple(Clear, -1, nullptr));
                    ZeroPriorityModulesStartIndex = 0;
                    ZeroPriorityModulesEndIndex = 0;
                }
            )
        {

        }

        void Loop::Run()
        {
            // Using Modules list lock to: 1. Prevent more than one async starts.
            //                             2. Provide thread safety for ToEditModules modifications.
            // NOTE: should not use the Modules list while locking isRunning to prevent deadlock
            //       as the Modules list uses this mutex inside (OnAdd, ...).
            Modules.LockAndDo([&] {                     // Lock #1
                auto guard = isRunning.Mutex.GetLock(); // Lock #2 => no deadlock, guaranteed
                if (isRunning)
                    throw std::logic_error("Cannot start twice.");
                ToEditModules.Clear(); // Just to be sure
                UpdatingModules = Modules;
                ToSchedule.Clear(); // Just to be sure
                ToSchedule.SetAutoShrink(false);
                isRunning = true;
                StartTime = std::chrono::steady_clock::now();
            });

            // No need to shared-lock the mutex on time updates
            std::chrono::time_point<std::chrono::steady_clock> StartTimeLocalCopy = StartTime;
            double PreviousTime = 0;
            Time = 0;
            TimeDiff = 0;
            TimeAsFloat = 0;
            TimeDiffAsFloat = 0;

            ShouldStop = false;
            Schedules.Clear();
            Schedules.SetAutoShrink(false);

            UpdatingModules.ForEach([this](Module * Item) { Item->Acquire(this); Item->_Start(); });

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
            Utilities::Collections::List<std::thread*, false> threads(threads_count);
            Utilities::Collections::List<thread_state> thread_states(threads_count);
            threads.SetAutoShrink(false);
            thread_states.SetAutoShrink(false);

            std::condition_variable condition;
            std::mutex condition_mutex;

            Utilities::Shared<int> CurrentPriority = -128; // Only set by the main thread
            Utilities::Shared<int, true> ModuleIndex = 0;
            Utilities::Shared<bool> ShouldTerminate = false;

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
                            condition_guard.unlock();
                            condition.notify_all();
                            return;
                        }
                        thread_states.SetItem(thread_index, thread_state::working);
                        condition_guard.unlock();
                        if (CurrentPriority == 0)
                        {
                            ScheduledJob job;
                            bool done_for_now = false;
                            auto guard = ModuleIndex.Mutex.GetLock();
                            while (!Schedules.IsEmpty())
                            {
                                job = ScheduledJob(nullptr, nullptr);
                                if (Schedules.GetFirstPriority() <= Time)
                                    switch (Schedules.GetFirstItem().first)
                                    {
                                        case ExecutionType::FreeAsync:
                                            std::thread([&](ScheduledJob job) {
                                                ExecuteScheduledJob(job);
                                            }, Schedules.Pop().second).detach();
                                            break;
                                        case ExecutionType::BoundedAsync:
                                            job = Schedules.Pop().second;
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
                                if (job.Task != nullptr) ExecuteScheduledJob(job);
                                guard = ModuleIndex.Mutex.GetLock();
                            }
                            // Wait for the main thread to continue to run the schedules.
                            // And do not continue to the modules yet.
                            if (done_for_now) continue;
                        }
                        {
                            Module * module;
                            bool done_for_now = false;
                            auto guard = ModuleIndex.Mutex.GetLock();
                            while (ModuleIndex < UpdatingModules.GetCount()
                                && CurrentPriority == UpdatingModules.GetItem(ModuleIndex)->GetPriority())
                            {
                                if (!UpdatingModules.GetItem(ModuleIndex)->isEnabled)
                                {
                                    ModuleIndex = ModuleIndex + 1;
                                    continue;
                                }
                                module = nullptr;
                                switch (UpdatingModules.GetItem(ModuleIndex)->GetExecutionType())
                                {
                                    case ExecutionType::FreeAsync:
                                        std::thread([&](Module * module) {
                                            ExecuteUpdate(module);
                                        }, UpdatingModules.GetItem(ModuleIndex)).detach();
                                        ModuleIndex = ModuleIndex + 1;
                                        break;
                                    case ExecutionType::BoundedAsync:
                                        module = UpdatingModules.GetItem(ModuleIndex);
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
                                if (module != nullptr) ExecuteUpdate(module);
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
                // Update Modules list changes
                while (!ToEditModules.IsEmpty())
                {
                    auto item = ToEditModules.Pop();
                    Module * to_remove;
                    switch(std::get<0>(item))
                    {
                        case Add:
                            std::get<2>(item)->Acquire(this);
                            std::get<2>(item)->_Start();
                            UpdatingModules.Add(std::get<2>(item), std::get<1>(item));
                            break;
                        case Replace:
                            to_remove = UpdatingModules.GetItem(std::get<1>(item));
                            to_remove->_Stop();
                            to_remove->Release();
                            std::get<2>(item)->Acquire(this);
                            std::get<2>(item)->_Start();
                            UpdatingModules.SetItem(std::get<1>(item), std::get<2>(item));
                            break;
                        case Remove:
                            to_remove = UpdatingModules.GetItem(std::get<1>(item));
                            to_remove->_Stop();
                            to_remove->Release();
                            UpdatingModules.RemoveByIndex(std::get<1>(item));
                            break;
                        case Clear:
                            UpdatingModules.ForEach([](Module * module) {
                                module->_Stop();
                                module->Release();
                            });
                            UpdatingModules.Clear();
                            break;
                    }
                }

                // Update Schedules
                while (!ToSchedule.IsEmpty())
                {
                    auto item = ToSchedule.Pop();
                    Schedules.Push(std::pair(std::get<0>(item), std::get<1>(item)), std::get<2>(item));
                }

                if (UpdatingModules.GetCount() == 0)
                    break;

                auto duration = std::chrono::steady_clock::now() - StartTimeLocalCopy;
                PreviousTime = Time;
                Time = (double)std::chrono::duration_cast<std::chrono::microseconds>(duration).count() / 1000000.0;
                TimeDiff = Time - PreviousTime;
                TimeAsFloat = (float)Time;
                TimeDiffAsFloat = (float)TimeDiff;

                CurrentPriority = -128;
                ModuleIndex = 0;

                while (true)
                {
                    // Set ModuleIndex
                    if (ModuleIndex >= UpdatingModules.GetCount())
                        if (CurrentPriority <= 0)
                            CurrentPriority = 0;
                        else break;
                    else if (CurrentPriority < UpdatingModules.GetItem(ModuleIndex)->GetPriority())
                        if (CurrentPriority <= 0 && UpdatingModules.GetItem(ModuleIndex)->GetPriority() > 0)
                            CurrentPriority = 0;
                        else CurrentPriority = UpdatingModules.GetItem(ModuleIndex)->GetPriority();
                    //  Normal process
                    if (CurrentPriority == 0)
                    {
                        ScheduledJob job;
                        bool pass_to_pool = false;
                        bool priority_done = false;
                        auto guard = ModuleIndex.Mutex.GetLock();
                        while (!Schedules.IsEmpty())
                        {
                            job = ScheduledJob(nullptr, nullptr);
                            if (Schedules.GetFirstPriority() <= Time)
                                switch (Schedules.GetFirstItem().first)
                                {
                                    case ExecutionType::FreeAsync:
                                        std::thread([&](ScheduledJob job) {
                                            ExecuteScheduledJob(job);
                                        }, Schedules.Pop().second).detach();
                                        break;
                                    case ExecutionType::SingleThreaded:
                                        job = Schedules.Pop().second;
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
                            if (job.Task != nullptr) ExecuteScheduledJob(job);
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
                        Module * module;
                        bool pass_to_pool = false;
                        bool priority_done = false;
                        auto guard = ModuleIndex.Mutex.GetLock();
                        while (ModuleIndex < UpdatingModules.GetCount()
                            && CurrentPriority == UpdatingModules.GetItem(ModuleIndex)->GetPriority())
                        {
                            if (!UpdatingModules.GetItem(ModuleIndex)->isEnabled)
                            {
                                ModuleIndex = ModuleIndex + 1;
                                continue;
                            }
                            module = nullptr;
                            switch (UpdatingModules.GetItem(ModuleIndex)->GetExecutionType())
                            {
                                case ExecutionType::FreeAsync:
                                    std::thread([&](Module * module) {
                                        ExecuteUpdate(module);
                                    }, UpdatingModules.GetItem(ModuleIndex)).detach();
                                    ModuleIndex = ModuleIndex + 1;
                                    break;
                                case ExecutionType::SingleThreaded:
                                    module = UpdatingModules.GetItem(ModuleIndex);
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
                            if (module != nullptr) ExecuteUpdate(module);
                            guard = ModuleIndex.Mutex.GetLock();
                        }
                    }
                    CurrentPriority = CurrentPriority + 1;
                }
            }

            ShouldTerminate = true;
            pool_process();
            for (int i = 0; i < threads_count; i++)
            {
                threads.GetItem(i)->join();
                delete threads.GetItem(i);
            }

            UpdatingModules.ForEach([](Module * Item) { Item->_Stop(); Item->Release(); });
            UpdatingModules.Clear();

            Schedules.SetAutoShrink(true);
            Schedules.Clear();

            Time = 0;
            TimeDiff = 0;
            TimeAsFloat = 0;
            TimeDiffAsFloat = 0;

            Modules.LockAndDo([&] {
                auto guard = isRunning.Mutex.GetLock();
                ToSchedule.SetAutoShrink(true);
                ToSchedule.Clear();
                ToEditModules.Clear();
                isRunning = false;
            });
        }

        void Loop::Stop()
        {
            ShouldStop = true;
        }

        bool Loop::IsRunning()
        {
            return isRunning;
        }

        void Loop::Schedule(
                std::function<void()> Func,
                std::function<void(std::exception&)> ExceptionHandler,
                double Time, ExecutionType ExecutionType
        ) {
            auto guard = isRunning.Mutex.GetLock();
            if (isRunning)
                ToSchedule.Push(std::tuple(ExecutionType, ScheduledJob(Func, ExceptionHandler), Time));
        }

        void Loop::Schedule(
                double Time,
                std::function<void()> Func,
                std::function<void(std::exception&)> ExceptionHandler,
                ExecutionType ExecutionType
        ) {
            auto guard = isRunning.Mutex.GetLock();
            if (isRunning)
                ToSchedule.Push(std::tuple(ExecutionType, ScheduledJob(Func, ExceptionHandler), Time));
        }

        void Loop::Schedule(
                double Time, ExecutionType ExecutionType,
                std::function<void()> Func,
                std::function<void(std::exception&)> ExceptionHandler
        ) {
            auto guard = isRunning.Mutex.GetLock();
            if (isRunning)
                ToSchedule.Push(std::tuple(ExecutionType, ScheduledJob(Func, ExceptionHandler), Time));
        }

        Loop::ScheduledJob::ScheduledJob(
            std::function<void()> Task,
            std::function<void(std::exception&)> ExceptionHandler
        ) : Task(Task), ExceptionHandler(ExceptionHandler) {}

        inline void Loop::ExecuteScheduledJob(ScheduledJob& job)
        {
            try
            {
                job.Task();
            }
            catch (std::exception& e)
            {
                if (job.ExceptionHandler != nullptr) try
                {
                    job.ExceptionHandler(e);
                }
                catch (...) {} // ignore
            }
            catch (...)
            {
                if (job.ExceptionHandler != nullptr) try
                {
                    std::runtime_error e("Unknown exception (not derived from std::exception)");
                    job.ExceptionHandler(e);
                }
                catch (...) {} // ignore
            }
        }
        inline void Loop::ExecuteUpdate(Module * module)
        {
            try
            {
                module->OnUpdate();
            }
            catch (std::exception& e)
            {
                try { module->OnException(e); }
                catch (...) {} // ignore
            }
            catch (...)
            {
                try
                {
                    std::runtime_error e("Unknown exception (not derived from std::exception)");
                    module->OnException(e);
                }
                catch (...) {} // ignore
            }
        }
    }
}
