#include "../Engine.h"
#include <chrono>

namespace Engine
{
    namespace Core
    {
        class ContainerEndNowException {};

        Container::Container() : ZeroPriorityModulesStartIndex(0), ZeroPriorityModulesEndIndex(0),
                                 isRunning(false), Time(0), TimeDiff(0), TimeFloat(0), TimeDiffFloat(0),
                                 ShouldEnd(false),
                                 Modules(

                // OnAdd
                [this](Data::Collections::List<Module*> * Parent, Module *& Item, int& Index)->bool
                {
                    if (Parent->Contains(Item))
                        return false;

                    if (Item->GetPriority() == 0)
                    {
                        if (Index > ZeroPriorityModulesEndIndex)
                            Index = ZeroPriorityModulesEndIndex;
                        else if (Index < ZeroPriorityModulesStartIndex)
                            Index = ZeroPriorityModulesStartIndex;

                        ZeroPriorityModulesEndIndex++;
                    }
                    else
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

                    Parent->Add(Item, Index);

                    Item->Acquire(this);
                    if (isRunning)
                        Item->_Start();

                    return true;
                },

                // OnSetItem
                [this](Data::Collections::List<Module*> * Parent, int& Index, Module *& Value)->bool
                {
                    try
                    {
                        if ((Index == 0 || Parent->GetItem(Index - 1)->GetPriority() <= Value->GetPriority())
                            && (Index == Parent->GetCount() - 1 || Value->GetPriority() <= Parent->GetItem(Index + 1)->GetPriority()))
                        {
                            if (isRunning)
                                Parent->GetItem(Index)->_End();
                            Parent->GetItem(Index)->Release();

                            Parent->SetItem(Index, Value);

                            Value->Acquire(this);
                            if (isRunning)
                                Value->_Start();

                            return true;
                        }
                    }
                    catch (std::exception& e) { throw e; }
                },

                // OnRemove
                [this](Data::Collections::List<Module*> * Parent, int& Index)->bool
                {
                    try
                    {
                        Module * Item = Parent->GetItem(Index);

                        if (isRunning)
                            Item->_End();
                        Item->Release();

                        int Priority = Item->GetPriority();
                        Parent->RemoveByIndex(Index);
                        if (Priority <= 0)
                            ZeroPriorityModulesEndIndex--;
                        if (Priority < 0)
                            ZeroPriorityModulesStartIndex--;
                        return true;
                    }
                    catch (std::exception& e) { throw e; }
                },

                // OnClear
                [this](Data::Collections::List<Module*> * Parent)->bool
                {
                    if (isRunning) Parent->ForEach([](Module * Item) {
                        Item->_End();
                        Item->Release();
                    });
                    else Parent->ForEach([](Module * Item) {
                        Item->Release();
                    });

                    Parent->Clear();
                    ZeroPriorityModulesStartIndex = 0;
                    ZeroPriorityModulesEndIndex = 0;
                    return true;
                }
            )
        {

        }

        void Container::Start()
        {
            if (isRunning)
                throw std::logic_error("Cannot start twice.");

            auto StartTime = std::chrono::steady_clock::now();
            double PreviousTime = 0;
            Time = 0;
            TimeDiff = 0;
            TimeFloat = 0;
            TimeDiffFloat = 0;

            ShouldEnd = false;
            Schedules.Clear();
            AsyncSchedules.Clear();

            Data::Collections::List<Module*> copy_list = Modules;
            isRunning = true;
            copy_list.ForEach([](Module * Item) { Item->_Start(); });
            copy_list.Clear();
            
            while (!ShouldEnd)
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

                Modules.ForEach([](Module * Item) { if (Item->isActive) Item->Update(); });

                if (Modules.GetCount() == 0)
                    break;
            }

            copy_list = Modules;
            isRunning = false;
            copy_list.ForEach([](Module * Item) { Item->_End(); });
            copy_list.Clear();

            Time = 0;
            TimeDiff = 0;
            TimeFloat = 0;
            TimeDiffFloat = 0;
        }

        void Container::End()
        {
            ShouldEnd = true;
        }

        bool Container::IsRunning()
        {
            return isRunning;
        }

        void Container::Schedule(std::function<void()> Func, double Time, bool Async)
        {
            if (Async) AsyncSchedules.Push(Func, Time);
            else Schedules.Push(Func, Time);
        }

        void Container::Schedule(double Time, std::function<void()> Func, bool Async)
        {
            if (Async) AsyncSchedules.Push(Func, Time);
            else Schedules.Push(Func, Time);
        }

        void Container::Schedule(double Time, bool Async, std::function<void()> Func)
        {
            if (Async) AsyncSchedules.Push(Func, Time);
            else Schedules.Push(Func, Time);
        }
    }
}
