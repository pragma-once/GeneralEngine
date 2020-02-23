#include "../../Engine/Engine.h"
#include <iostream>
#include <string>

// To check the compilation errors
template class Engine::Utilities::Collections::ResizableArray<int, true>;
template class Engine::Utilities::Collections::ResizableArray<int, false>;
template class Engine::Utilities::Collections::List<int, true>;
template class Engine::Utilities::Collections::List<int, false>;
template class Engine::Utilities::Collections::Stack<int, true>;
template class Engine::Utilities::Collections::Stack<int, false>;
template class Engine::Utilities::Collections::Queue<int, true>;
template class Engine::Utilities::Collections::Queue<int, false>;
template class Engine::Utilities::Collections::PriorityQueue<int, int, true, true>;
template class Engine::Utilities::Collections::PriorityQueue<int, int, true, false>;
template class Engine::Utilities::Collections::Dictionary<int, int, true>;
template class Engine::Utilities::Collections::Dictionary<int, int, false>;

#define print(context) (std::cout << context << '\n')
#define input(var) (std::cin >> var)
#define ITEMS_TYPE std::string
#define KEY_TYPE std::string
#define VALUE_TYPE std::string

void TestList();
void TestStack();
void TestQueue();
void TestPriorityQueue();
void TestDictionary();

void TestMultipleLists();
void TestMultipleStacks();
void TestMultipleQueues();
void TestMultiplePriorityQueues();
void TestMultipleDictionaries();

int main()
{
    while (true)
    {
        print("");
        print("l => Test List");
        print("s => Test Stack");
        print("q => Test Queue");
        print("p => Test PriorityQueue");
        print("d => Test Dictionary");
        print("");
        print("L => Test Multiple Lists");
        print("S => Test Multiple Stacks");
        print("Q => Test Multiple Queues");
        print("P => Test Multiple PriorityQueues");
        print("D => Test Multiple Dictionaries");
        print("");

        char option;
        input(option);

        switch (option)
        {
        case 'l':
            TestList();
            break;
        case 's':
            TestStack();
            break;
        case 'q':
            TestQueue();
            break;
        case 'p':
            TestPriorityQueue();
            break;
        case 'd':
            TestDictionary();
            break;
        case 'L':
            TestMultipleLists();
            break;
        case 'S':
            TestMultipleStacks();
            break;
        case 'Q':
            TestMultipleQueues();
            break;
        case 'P':
            TestMultiplePriorityQueues();
            break;
        case 'D':
            TestMultipleDictionaries();
            break;
        default:
            break;
        }
    }

    return 0;
}

void TestList()
{
    
    Engine::Utilities::Collections::List<ITEMS_TYPE> * list = new Engine::Utilities::Collections::List<ITEMS_TYPE>();
    while (true)
    {
        print("");
        print("a Item        => Add(Item)");
        print("p Item Index  => Add(Item, Index)");
        print("r Index       => RemoveByIndex(Index)");
        print("R Item        => Remove(Item)");
        print("s Index Value => SetItem(Index, Value)");
        print("c             => Clear()");
        print("g Index       => GetItem(Index)");
        print("C             => GetCount()");
        print("f Item From   => Find(Item, FromIndex=0)");
        print("e Item        => Contains(Item)");
        print("F             => ForEach([](Item) { print(Item); }");
        print("");
        print("A Item Times      => for Times: Add(Item)");
        print("d                 => delete list; list = new List()");
        print("n InitialCapacity => delete list; list = new List(InitialCapacity)");
        print("");
        print("E Space           => Expand(Space)");
        print("S AdditionalSpace => Shrink(AdditionalSpace)");
        print("L                 => GetCapacity()");
        print("");
        print("q => Quit List Test");
        print("");

        char func;
        int arg_int;
        ITEMS_TYPE arg_item;
        input(func);

        try
        {
            auto old_list = list;
            switch (func)
            {
            case 'a':
                input(arg_item);
                list->Add(arg_item);
                break;
            case 'p':
                input(arg_item);
                input(arg_int);
                list->Add(arg_item, arg_int);
                break;
            case 'r':
                input(arg_int);
                list->RemoveByIndex(arg_int);
                break;
            case 'R':
                input(arg_item);
                print(list->Remove(arg_item));
                break;
            case 's':
                input(arg_int);
                input(arg_item);
                list->SetItem(arg_int, arg_item);
                break;
            case 'c':
                list->Clear();
                break;
            case 'g':
                input(arg_int);
                print(list->GetItem(arg_int));
                break;
            case 'C':
                print(list->GetCount());
                break;
            case 'f':
                input(arg_item);
                input(arg_int);
                print(list->Find(arg_item, arg_int));
                break;
            case 'e':
                input(arg_item);
                print(list->Contains(arg_item));
                break;
            case 'F':
                list->ForEach([](ITEMS_TYPE Item) { print(Item); });
                break;
            case 'A':
                input(arg_item);
                input(arg_int);
                for (int i = 0; i < arg_int; i++) list->Add(arg_item);
                break;
            case 'd':
                delete list;
                list = new Engine::Utilities::Collections::List<ITEMS_TYPE>();
                break;
            case 'n':
                input(arg_int);
                list = new Engine::Utilities::Collections::List<ITEMS_TYPE>(arg_int);
                delete old_list;
                break;
            case 'E':
                input(arg_int);
                list->Expand(arg_int);
                break;
            case 'S':
                input(arg_int);
                list->Shrink(arg_int);
                break;
            case 'L':
                print(list->GetCapacity());
                break;
            case 'q':
                delete list;
                return;
            default:
                break;
            }
        }
        catch (std::exception& e)
        {
            print("Exception: " << e.what());
        }
    }
}

void TestStack()
{
    Engine::Utilities::Collections::Stack<ITEMS_TYPE> * stack = new Engine::Utilities::Collections::Stack<ITEMS_TYPE>();
    while (true) try
    {
        print("");
        print("p Item => Push(Item)");
        print("P      => Pop()");
        print("s Item => SetTop(Item)");
        print("c      => Clear()");
        print("");
        print("g      => GetTop()");
        print("d Item => GetDepthOf(Item)");
        print("e Item => Contains(Item)");
        print("C      => GetCount()");
        print("E      => IsEmpty()");
        print("");
        print("q => Quit Stack Test");
        print("");

        char func;
        ITEMS_TYPE arg;
        input(func);

        switch (func)
        {
        case 'p':
            input(arg);
            stack->Push(arg);
            break;
        case 'P':
            print(stack->Pop());
            break;
        case 's':
            input(arg);
            stack->SetTop(arg);
            break;
        case 'c':
            stack->Clear();
            break;
        case 'g':
            print(stack->GetTop());
            break;
        case 'd':
            input(arg);
            print(stack->GetDepthOf(arg));
            break;
        case 'e':
            input(arg);
            print(stack->Contains(arg));
            break;
        case 'C':
            print(stack->GetCount());
            break;
        case 'E':
            print(stack->IsEmpty());
            break;
        case 'q':
            delete stack;
            return;
        default:
            break;
        }
    }
    catch(std::exception& e) { print("Exception: " << e.what()); }
}

void TestQueue()
{
    Engine::Utilities::Collections::Queue<ITEMS_TYPE> * queue = new Engine::Utilities::Collections::Queue<ITEMS_TYPE>();
    while (true) try
    {
        print("");
        print("p Item => Push(Item)");
        print("P      => Pop()");
        print("c      => Clear()");
        print("");
        print("g      => GetFirst()");
        print("d Item => GetDepthOf(Item)");
        print("e Item => Contains(Item)");
        print("C      => GetCount()");
        print("E      => IsEmpty()");
        print("");
        print("q => Quit Queue Test");
        print("");

        char func;
        ITEMS_TYPE arg;
        input(func);

        switch (func)
        {
        case 'p':
            input(arg);
            queue->Push(arg);
            break;
        case 'P':
            print(queue->Pop());
            break;
        case 'c':
            queue->Clear();
            break;
        case 'g':
            print(queue->GetFirst());
            break;
        case 'd':
            input(arg);
            print(queue->GetDepthOf(arg));
            break;
        case 'e':
            input(arg);
            print(queue->Contains(arg));
            break;
        case 'C':
            print(queue->GetCount());
            break;
        case 'E':
            print(queue->IsEmpty());
            break;
        case 'q':
            delete queue;
            return;
        default:
            break;
        }
    }
    catch (std::exception& e) { print("Exception: " << e.what()); }
}

void TestPriorityQueue()
{
    Engine::Utilities::Collections::PriorityQueue<ITEMS_TYPE> * queue = new Engine::Utilities::Collections::PriorityQueue<ITEMS_TYPE>();
    while (true) try
    {
        print("");
        print("p Item Priority => Push(Item, Priority)");
        print("Pr              => Pop()");
        print("Po              => Pop(ItemOut, PriorityOut)");
        print("c               => Clear()");
        print("");
        print("g      => GetFirstItem()");
        print("G      => GetFirstPriority()");
        print("d Item => GetDepthOf(Item)");
        print("e Item => Contains(Item)");
        print("C      => GetCount()");
        print("E      => IsEmpty()");
        print("");
        print("q => Quit PriorityQueue Test");
        print("");

        char func;
        ITEMS_TYPE arg;
        int arg_int;
        input(func);

        switch (func)
        {
        case 'p':
            input(arg);
            input(arg_int);
            queue->Push(arg, arg_int);
            break;
        case 'P':
            input(func);
            if (func == 'r')
                print(queue->Pop());
            else if (func == 'o')
                print(
                    "return value: " << queue->Pop(arg, arg_int)
                    << "\nItemOut=" << arg
                    << "\nPriorityOut=" << arg_int
                );
            break;
        case 'c':
            queue->Clear();
            break;
        case 'g':
            print(queue->GetFirstItem());
            break;
        case 'G':
            print(queue->GetFirstPriority());
            break;
        case 'd':
            input(arg);
            print(queue->GetDepthOf(arg));
            break;
        case 'e':
            input(arg);
            print(queue->Contains(arg));
            break;
        case 'C':
            print(queue->GetCount());
            break;
        case 'E':
            print(queue->IsEmpty());
            break;
        case 'q':
            delete queue;
            return;
        default:
            break;
        }
    }
    catch (std::exception& e) { print("Exception: " << e.what()); }
}

void TestDictionary()
{
    Engine::Utilities::Collections::Dictionary<KEY_TYPE, VALUE_TYPE> * dict = new Engine::Utilities::Collections::Dictionary<KEY_TYPE, VALUE_TYPE>();
    while (true) try
    {
        print("");
        print("s Key Value => SetValue(Key, Value)");
        print("r Key       => Remove(Key)");
        print("c           => Clear()");
        print("");
        print("g Key => GetValue(Key)");
        print("e     => IsEmpty()");
        print("f     => ForEach([](Key) { print(Key); })");
        print("F     => ForEach([](Key, Value) { print(Key => Value); })");
        print("");
        print("q => Quit Dictionary Test");
        print("");

        char func;
        KEY_TYPE arg_key;
        VALUE_TYPE arg_value;
        input(func);

        switch (func)
        {
        case 's':
            input(arg_key);
            input(arg_value);
            dict->SetValue(arg_key, arg_value);
            break;
        case 'r':
            input(arg_key);
            dict->Remove(arg_key);
            break;
        case 'c':
            dict->Clear();
            break;
        case 'g':
            input(arg_key);
            print(dict->GetValue(arg_key));
            break;
        case 'e':
            print(dict->IsEmpty());
            break;
        case 'f':
            dict->ForEach([](KEY_TYPE Key) { print(Key); });
            break;
        case 'F':
            dict->ForEach([](KEY_TYPE Key, VALUE_TYPE Value) { print(Key << "\t=>\t" << Value); });
            break;
        case 'q':
            delete dict;
            return;
        default:
            break;
        }
    }
    catch (std::exception& e) { print("Exception: " << e.what()); }
}

void TestMultipleLists()
{
    Engine::Utilities::Collections::List<ITEMS_TYPE> lists[4];
    while (true) try
    {
        print("");
        print("a Index Item    => lists[Index].Add(Item)");
        print("c Index         => lists[Index].Clear()");
        print("f Index         => lists[Index].ForEach([](Item) { print(Item); })");
        print("s Index1 Index2 => lists[Index1] = lists[Index2]");
        print("");
        print("j Index1 Index2 Index3 => lists[Index1] = lists[Index2] + lists[Index3]");
        print("J Index1 Index2        => lists[Index1] += lists[Index2]");
        print("");
        print("q => Quit Multiple Lists Test");
        print("");

        char func;
        ITEMS_TYPE arg;
        int arg_int1;
        int arg_int2;
        int arg_int3;
        input(func);

        switch (func)
        {
        case 'a':
            input(arg_int1);
            input(arg);
            if (arg_int1 < 0 || arg_int1 >= 4)
                print("Index should be between 0-3");
            else
                lists[arg_int1].Add(arg);
            break;
        case 'c':
            input(arg_int1);
            if (arg_int1 < 0 || arg_int1 >= 4)
                print("Index should be between 0-3");
            else
                lists[arg_int1].Clear();
            break;
        case 'f':
            input(arg_int1);
            if (arg_int1 < 0 || arg_int1 >= 4)
                print("Index should be between 0-3");
            else
                lists[arg_int1].ForEach([](ITEMS_TYPE Item) { print(Item); });
            break;
        case 's':
            input(arg_int1);
            input(arg_int2);
            if (arg_int1 < 0 || arg_int1 >= 4)
                print("Index1 should be between 0-3");
            else if (arg_int2 < 0 || arg_int2 >= 4)
                print("Index2 should be between 0-3");
            else
                lists[arg_int1] = lists[arg_int2];
            break;
        case 'j':
            input(arg_int1);
            input(arg_int2);
            input(arg_int3);
            if (arg_int1 < 0 || arg_int1 >= 4)
                print("Index1 should be between 0-3");
            else if (arg_int2 < 0 || arg_int2 >= 4)
                print("Index2 should be between 0-3");
            else if (arg_int3 < 0 || arg_int3 >= 4)
                print("Index3 should be between 0-3");
            else
                lists[arg_int1] = lists[arg_int2] + lists[arg_int3];
            break;
        case 'J':
            input(arg_int1);
            input(arg_int2);
            if (arg_int1 < 0 || arg_int1 >= 4)
                print("Index1 should be between 0-3");
            else if (arg_int2 < 0 || arg_int2 >= 4)
                print("Index2 should be between 0-3");
            else
                lists[arg_int1] += lists[arg_int2];
            break;
        case 'q':
            return;
        }
    }
    catch (std::exception& e) { print("Exception: " << e.what()); }
}

void TestMultipleStacks()
{
    Engine::Utilities::Collections::Stack<ITEMS_TYPE> stacks[5];
    while (true) try
    {
        print("");
        print("p Index Item    => stacks[Index].Push(Item)");
        print("c Index         => stacks[Index].Clear()");
        print("P Index         => stacks[Index].Pop()");
        print("f Index         => print all items in stacks[Index]");
        print("s Index1 Index2 => stacks[Index1] = stacks[Index2]");
        print("");
        print("q => Quit Multiple Stacks Test");
        print("");

        char func;
        ITEMS_TYPE arg;
        int arg_int1;
        int arg_int2;
        input(func);

        switch (func)
        {
        case 'p':
            input(arg_int1);
            input(arg);
            if (arg_int1 < 0 || arg_int1 >= 4)
                print("Index should be between 0-3");
            else
                stacks[arg_int1].Push(arg);
            break;
        case 'c':
            input(arg_int1);
            if (arg_int1 < 0 || arg_int1 >= 4)
                print("Index should be between 0-3");
            else
                stacks[arg_int1].Clear();
            break;
        case 'P':
            input(arg_int1);
            if (arg_int1 < 0 || arg_int1 >= 4)
                print("Index should be between 0-3");
            else
                stacks[arg_int1].Pop();
            break;
        case 'f':
            input(arg_int1);
            if (arg_int1 < 0 || arg_int1 >= 4)
                print("Index should be between 0-3");
            else
            {
                while (!stacks[arg_int1].IsEmpty())
                    stacks[4].Push(stacks[arg_int1].Pop());
                while (!stacks[4].IsEmpty())
                {
                    print(stacks[4].GetTop());
                    stacks[arg_int1].Push(stacks[4].Pop());
                }
            }
            break;
        case 's':
            input(arg_int1);
            input(arg_int2);
            if (arg_int1 < 0 || arg_int1 >= 4)
                print("Index1 should be between 0-3");
            else if (arg_int2 < 0 || arg_int2 >= 4)
                print("Index2 should be between 0-3");
            else
                stacks[arg_int1] = stacks[arg_int2];
            break;
        case 'q':
            return;
        }
    }
    catch (std::exception& e) { print("Exception: " << e.what()); }
}

void TestMultipleQueues()
{
    Engine::Utilities::Collections::Queue<ITEMS_TYPE> queues[5];
    while (true) try
    {
        print("");
        print("p Index Item    => queues[Index].Push(Item)");
        print("c Index         => queues[Index].Clear()");
        print("P Index         => queues[Index].Pop()");
        print("f Index         => print all items in queues[Index]");
        print("s Index1 Index2 => queues[Index1] = queues[Index2]");
        print("");
        print("q => Quit Multiple Queues Test");
        print("");

        char func;
        ITEMS_TYPE arg;
        int arg_int1;
        int arg_int2;
        input(func);

        switch (func)
        {
        case 'p':
            input(arg_int1);
            input(arg);
            if (arg_int1 < 0 || arg_int1 >= 4)
                print("Index should be between 0-3");
            else
                queues[arg_int1].Push(arg);
            break;
        case 'c':
            input(arg_int1);
            if (arg_int1 < 0 || arg_int1 >= 4)
                print("Index should be between 0-3");
            else
                queues[arg_int1].Clear();
            break;
        case 'P':
            input(arg_int1);
            if (arg_int1 < 0 || arg_int1 >= 4)
                print("Index should be between 0-3");
            else
                print(queues[arg_int1].Pop());
            break;
        case 'f':
            input(arg_int1);
            if (arg_int1 < 0 || arg_int1 >= 4)
                print("Index should be between 0-3");
            else
            {
                while (!queues[arg_int1].IsEmpty())
                    queues[4].Push(queues[arg_int1].Pop());
                while (!queues[4].IsEmpty())
                {
                    print(queues[4].GetFirst());
                    queues[arg_int1].Push(queues[4].Pop());
                }
            }
            break;
        case 's':
            input(arg_int1);
            input(arg_int2);
            if (arg_int1 < 0 || arg_int1 >= 4)
                print("Index1 should be between 0-3");
            else if (arg_int2 < 0 || arg_int2 >= 4)
                print("Index2 should be between 0-3");
            else
                queues[arg_int1] = queues[arg_int2];
            break;
        case 'q':
            return;
        }
    }
    catch (std::exception& e) { print("Exception: " << e.what()); }
}

void TestMultiplePriorityQueues()
{
    Engine::Utilities::Collections::PriorityQueue<ITEMS_TYPE> queues[5];
    while (true) try
    {
        print("");
        print("p Index Item Priority => queues[Index].Push(Item, Priority)");
        print("c Index               => queues[Index].Clear()");
        print("P Index               => queues[Index].Pop()");
        print("f Index               => print all items and priorities in queues[Index]");
        print("s Index1 Index2       => queues[Index1] = queues[Index2]");
        print("");
        print("q => Quit Multiple PriorityQueues Test");
        print("");

        char func;
        ITEMS_TYPE arg;
        int arg_int1;
        int arg_int2;
        input(func);

        switch (func)
        {
        case 'p':
            input(arg_int1);
            input(arg);
            input(arg_int2);
            if (arg_int1 < 0 || arg_int1 >= 4)
                print("Index should be between 0-3");
            else
                queues[arg_int1].Push(arg, arg_int2);
            break;
        case 'c':
            input(arg_int1);
            if (arg_int1 < 0 || arg_int1 >= 4)
                print("Index should be between 0-3");
            else
                queues[arg_int1].Clear();
            break;
        case 'P':
            input(arg_int1);
            if (arg_int1 < 0 || arg_int1 >= 4)
                print("Index should be between 0-3");
            else
            {
                print(queues[arg_int1].GetFirstItem() << "/Priority=" << queues[arg_int1].GetFirstPriority());
                queues[arg_int1].Pop();
            }
            break;
        case 'f':
            input(arg_int1);
            if (arg_int1 < 0 || arg_int1 >= 4)
                print("Index should be between 0-3");
            else
            {
                while (!queues[arg_int1].IsEmpty())
                {
                    queues[4].Push(queues[arg_int1].GetFirstItem(), queues[arg_int1].GetFirstPriority());
                    queues[arg_int1].Pop();
                }
                while (!queues[4].IsEmpty())
                {
                    print(queues[4].GetFirstItem() << "/Priority=" << queues[4].GetFirstPriority());
                    queues[arg_int1].Push(queues[4].GetFirstItem(), queues[4].GetFirstPriority());
                    queues[4].Pop();
                }
            }
            break;
        case 's':
            input(arg_int1);
            input(arg_int2);
            if (arg_int1 < 0 || arg_int1 >= 4)
                print("Index1 should be between 0-3");
            else if (arg_int2 < 0 || arg_int2 >= 4)
                print("Index2 should be between 0-3");
            else
                queues[arg_int1] = queues[arg_int2];
            break;
        case 'q':
            return;
        }
    }
    catch (std::exception& e) { print("Exception: " << e.what()); }
}

void TestMultipleDictionaries()
{
    Engine::Utilities::Collections::Dictionary<KEY_TYPE, VALUE_TYPE> dicts[4];
    while (true) try
    {
        print("");
        print("a Index Key Value => dicts[Index].SetValue(Key, Value)");
        print("c Index           => dicts[Index].Clear()");
        print("f Index           => dicts[Index].ForEach([](Item) { print(Item); })");
        print("s Index1 Index2   => dicts[Index1] = dicts[Index2]");
        print("");
        print("q => Quit Multiple Dictionaries Test");
        print("");

        char func;
        KEY_TYPE arg_key;
        VALUE_TYPE arg_value;
        int arg_int1;
        int arg_int2;
        input(func);

        switch (func)
        {
        case 'a':
            input(arg_int1);
            input(arg_key);
            input(arg_value);
            if (arg_int1 < 0 || arg_int1 >= 4)
                print("Index should be between 0-3");
            else
                dicts[arg_int1].SetValue(arg_key, arg_value);
            break;
        case 'c':
            input(arg_int1);
            if (arg_int1 < 0 || arg_int1 >= 4)
                print("Index should be between 0-3");
            else
                dicts[arg_int1].Clear();
            break;
        case 'f':
            input(arg_int1);
            if (arg_int1 < 0 || arg_int1 >= 4)
                print("Index should be between 0-3");
            else
                dicts[arg_int1].ForEach([](KEY_TYPE Key, VALUE_TYPE Value) { print(Key << "\t=>\t" << Value); });
            break;
        case 's':
            input(arg_int1);
            input(arg_int2);
            if (arg_int1 < 0 || arg_int1 >= 4)
                print("Index1 should be between 0-3");
            else if (arg_int2 < 0 || arg_int2 >= 4)
                print("Index2 should be between 0-3");
            else
                dicts[arg_int1] = dicts[arg_int2];
            break;
        case 'q':
            return;
        }
    }
    catch (std::exception& e) { print("Exception: " << e.what()); }
}
