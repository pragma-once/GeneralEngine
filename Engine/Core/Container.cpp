#include "../Engine.h"

namespace Engine
{
    namespace Core
    {
        Container::Container() : ZeroPriorityBehaviorsStartIndex(0), ZeroPriorityBehaviorsEndIndex(0), Behaviors(
            new Data::Collections::List<Behavior*>(

                // OnAdd
                [this](Data::Collections::List<Behavior*> * Parent, Behavior *& Item, int& Index)->bool
                {
                    if (Parent->Contains(Item))
                        return false;

                    if (Item->GetPriority() == 0)
                    {
                        if (Index >= ZeroPriorityBehaviorsEndIndex)
                            Parent->Add(Item, ZeroPriorityBehaviorsEndIndex);
                        else if (Index <= ZeroPriorityBehaviorsStartIndex)
                            Parent->Add(Item, ZeroPriorityBehaviorsStartIndex);
                        else
                            Parent->Add(Item, Index);

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

                        Parent->Add(Item, Index);

                        if (Item->GetPriority() < 0)
                        {
                            ZeroPriorityBehaviorsStartIndex++;
                            ZeroPriorityBehaviorsEndIndex++;
                        }
                    }

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
                        int Priority = Parent->GetItem(Index)->GetPriority();
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
                    Parent->Clear();
                    ZeroPriorityBehaviorsStartIndex = 0;
                    ZeroPriorityBehaviorsEndIndex = 0;
                    return true;
                }
            ))
        {

        }
    }
}