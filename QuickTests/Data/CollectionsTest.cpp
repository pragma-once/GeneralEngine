#include "../../Engine/Engine.h"
#include <iostream>
#include <string>

#define print(context) (std::cout << context << '\n')
#define input(var) (std::cin >> var)
#define ITEMS_TYPE std::string

void TestList();
void TestStack();
void TestQueue();
void TestPriorityQueue();
void TestDictionary();

int main()
{
    while (true)
    {
        print("");
        print("L => Test List");
        print("S => Test Stack");
        print("Q => Test Queue");
        print("P => Test PriorityQueue");
        print("D => Test Dictionary");
        print("");

        char option;
        input(option);

        switch (option)
        {
        case 'L':
            TestList();
            break;
        case 'S':
            TestStack();
            break;
        case 'Q':
            TestQueue();
            break;
        case 'P':
            TestPriorityQueue();
            break;
        case 'D':
            TestDictionary();
            break;
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
        default:
            break;
        }
    }

    return 0;
}

void TestList()
{
    
    Engine::Data::Collections::List<ITEMS_TYPE> * list = new Engine::Data::Collections::List<ITEMS_TYPE>();
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
        print("e Item        => Exists(Item)");
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
                print(list->Add(arg_item));
                break;
            case 'p':
                input(arg_item);
                input(arg_int);
                print(list->Add(arg_item, arg_int));
                break;
            case 'r':
                input(arg_int);
                print(list->RemoveByIndex(arg_int));
                break;
            case 'R':
                input(arg_item);
                print(list->Remove(arg_item));
                break;
            case 's':
                input(arg_int);
                input(arg_item);
                print(list->SetItem(arg_int, arg_item));
                break;
            case 'c':
                print(list->Clear());
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
                print(list->Exists(arg_item));
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
                list = new Engine::Data::Collections::List<ITEMS_TYPE>();
                break;
            case 'n':
                input(arg_int);
                list = new Engine::Data::Collections::List<ITEMS_TYPE>(arg_int);
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
    Engine::Data::Collections::Stack<ITEMS_TYPE> * stack = new Engine::Data::Collections::Stack<ITEMS_TYPE>();
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
        print("e Item => Exists(Item)");
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
            print(stack->Exists(arg));
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
    Engine::Data::Collections::Queue<ITEMS_TYPE> * queue = new Engine::Data::Collections::Queue<ITEMS_TYPE>();
    while (true) try
    {
        print("");
        print("p Item => Push(Item)");
        print("P      => Pop()");
        print("c      => Clear()");
        print("");
        print("g      => GetFirst()");
        print("d Item => GetDepthOf(Item)");
        print("e Item => Exists(Item)");
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
            print(queue->Exists(arg));
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
    Engine::Data::Collections::PriorityQueue<ITEMS_TYPE> * queue = new Engine::Data::Collections::PriorityQueue<ITEMS_TYPE>();
    while (true) try
    {
        print("");
        print("p Item Priority => Push(Item, Priority)");
        print("Pr              => Pop()");
        print("Po              => Pop(ItemOut, PriorityOut)");
        print("c               => Clear()");
        print("");
        print("g      => GetFirst()");
        print("d Item => GetDepthOf(Item)");
        print("e Item => Exists(Item)");
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
            {
                print("return value: " << queue->Pop(arg, arg_int));
                print("ItemOut=" << arg << "\nPriorityOut=" << arg_int);
            }
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
            print(queue->Exists(arg));
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

#define KEY_TYPE std::string
#define VALUE_TYPE std::string

void TestDictionary()
{
    Engine::Data::Collections::Dictionary<KEY_TYPE, VALUE_TYPE> * dict = new Engine::Data::Collections::Dictionary<KEY_TYPE, VALUE_TYPE>();
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