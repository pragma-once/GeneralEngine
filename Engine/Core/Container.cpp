#include "../Engine.h"
#include <chrono>

namespace Engine
{
    namespace Core
    {
        class ContainerEndNowException {};

        Container::Container() : ZeroPriorityBehaviorsStartIndex(0), ZeroPriorityBehaviorsEndIndex(0),
                                 isRunning(false), Time(0), TimeDiff(0), TimeFloat(0), TimeDiffFloat(0),
                                 ShouldEnd(false),
                                 Behaviors(

                // OnAdd
                [this](Data::Collections::List<Behavior*> * Parent, Behavior *& Item, int& Index)->bool
                {
                    if (Parent->Contains(Item))
                        return false;

                    if (Item->GetPriority() == 0)
                    {
                        if (Index > ZeroPriorityBehaviorsEndIndex)
                            Index = ZeroPriorityBehaviorsEndIndex;
                        else if (Index < ZeroPriorityBehaviorsStartIndex)
                            Index = ZeroPriorityBehaviorsStartIndex;

                        ZeroPriorityBehaviorsEndIndex++;
                    }
                    else
                    {
                        int s = Item->GetPriority() < 0 ? 0 : ZeroPriorityBehaviorsEndIndex;
                        int e = Item->GetPriority() < 0 ? ZeroPriorityBehaviorsStartIndex : Parent->GetCount();

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
                            ZeroPriorityBehaviorsStartIndex++;
                            ZeroPriorityBehaviorsEndIndex++;
                        }
                    }

                    Item->Acquire(this);
                    if (isRunning)
                        Item->_Start();

                    Parent->Add(Item, Index);

                    return true;
                },

                // OnSetItem
                [this](Data::Collections::List<Behavior*> * Parent, int& Index, Behavior *& Value)->bool
                {
                    try
                    {
                        if ((Index == 0 || Parent->GetItem(Index - 1)->GetPriority() <= Value->GetPriority())
                            && (Index == Parent->GetCount() - 1 || Value->GetPriority() <= Parent->GetItem(Index + 1)->GetPriority()))
                        {
                            if (isRunning)
                                Parent->GetItem(Index)->_End();
                            Parent->GetItem(Index)->Release();

                            Value->Acquire(this);
                            if (isRunning)
                                Value->_Start();

                            Parent->SetItem(Index, Value);
                            return true;
                        }
                    }
                    catch (std::exception& e) { throw e; }
                },

                // OnRemove
                [this](Data::Collections::List<Behavior*> * Parent, int& Index)->bool
                {
                    try
                    {
                        Behavior * Item = Parent->GetItem(Index);

                        if (isRunning)
                            Item->_End();
                        Item->Release();

                        int Priority = Item->GetPriority();
                        Parent->RemoveByIndex(Index);
                        if (Priority <= 0)
                            ZeroPriorityBehaviorsEndIndex--;
                        if (Priority < 0)
                            ZeroPriorityBehaviorsStartIndex--;
                        return true;
                    }
                    catch (std::exception& e) { throw e; }
                },

                // OnClear
                [this](Data::Collections::List<Behavior*> * Parent)->bool
                {
                    if (isRunning) Parent->ForEach([](Behavior * Item) {
                        Item->_End();
                        Item->Release();
                    });
                    else Parent->ForEach([](Behavior * Item) {
                        Item->Release();
                    });

                    Parent->Clear();
                    ZeroPriorityBehaviorsStartIndex = 0;
                    ZeroPriorityBehaviorsEndIndex = 0;
                    return true;
                }
            )
        {

        }

        void Container::Start()
        {
            auto StartTime = std::chrono::steady_clock::now();
            double PreviousTime = 0;
            Time = 0;
            TimeDiff = 0;
            TimeFloat = 0;
            TimeDiffFloat = 0;

            ShouldEnd = false;
            Schedules.Clear();
            AsyncSchedules.Clear();
            
            Data::Collections::List<Behavior*> copy_list = Behaviors;
            isRunning = true;
            copy_list.ForEach([](Behavior * Item) { Item->_Start(); });
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
                
                try { Behaviors.ForEach([](Behavior * Item) { Item->Update(); }); }
                catch (const ContainerEndNowException&) { break; }
            }

            copy_list = Behaviors;
            isRunning = false;
            copy_list.ForEach([](Behavior * Item) { Item->_End(); });
            copy_list.Clear();

            Time = 0;
            TimeDiff = 0;
            TimeFloat = 0;
            TimeDiffFloat = 0;
        }

        void Container::End(bool EndNow)
        {
            ShouldEnd = true;
            if (EndNow)
                throw ContainerEndNowException();
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