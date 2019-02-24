#include "../Engine.h"
#include <chrono>

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

        void Loop::Start()
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
            AsyncSchedules.Clear();
            Schedules.SetAutoShrink(false);
            AsyncSchedules.SetAutoShrink(false);

            Data::Collections::List<Module*> copy_list = Modules;
            isRunning = true;
            copy_list.ForEach([](Module * Item) { Item->_Start(); });

            guard.Unlock();

            copy_list.Clear();
            
            while (!ShouldStop)
            {
                auto duration = std::chrono::steady_clock::now() - StartTime;
                PreviousTime = Time;
                Time = (double)std::chrono::duration_cast<std::chrono::microseconds>(duration).count() / 1000000.0;
                TimeDiff = Time - PreviousTime;
                TimeFloat = (float)Time;
                TimeDiffFloat = (float)TimeDiff;

                while (!AsyncSchedules.IsEmpty())
                    if (AsyncSchedules.GetFirstPriority() <= Time)
                        std::thread(
                            [](Data::Collections::PriorityQueue<std::function<void()>, double> * AsyncSchedules) { AsyncSchedules->Pop()(); }
                            , &AsyncSchedules
                            ).detach();
                    else break;

                while (!Schedules.IsEmpty())
                    if (Schedules.GetFirstPriority() <= Time)
                        Schedules.Pop()();
                    else break;

                Modules.ForEach([](Module * Item) { if (Item->isActive) Item->OnUpdate(); });

                if (Modules.GetCount() == 0)
                    break;
            }

            copy_list = Modules;

            guard = isRunning.Mutex.GetLock();

            isRunning = false;
            copy_list.ForEach([](Module * Item) { Item->_Stop(); });
            copy_list.Clear();

            Schedules.SetAutoShrink(true);
            AsyncSchedules.SetAutoShrink(true);
            Schedules.Clear();
            AsyncSchedules.Clear();

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

        void Loop::Schedule(std::function<void()> Func, double Time, bool Async)
        {
            if (Async) AsyncSchedules.Push(Func, Time);
            else Schedules.Push(Func, Time);
        }

        void Loop::Schedule(double Time, std::function<void()> Func, bool Async)
        {
            if (Async) AsyncSchedules.Push(Func, Time);
            else Schedules.Push(Func, Time);
        }

        void Loop::Schedule(double Time, bool Async, std::function<void()> Func)
        {
            if (Async) AsyncSchedules.Push(Func, Time);
            else Schedules.Push(Func, Time);
        }
    }
}
