#include "../../../Engine/Engine.h"
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
        print("L => Test List");
        print("S => Test Stack");
        print("Q => Test Queue");
        print("P => Test PriorityQueue");
        print("D => Test Dictionary");
        char option;
        input(option);

        switch (option)
        {
        case 'L':
            TestList();
            break;
        case 'S':
            break;
        case 'Q':
            break;
        case 'P':
            break;
        case 'D':
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
        char func;
        int arg_int;
        ITEMS_TYPE arg_str;
        input(func);

        try
        {
            auto old_list = list;
            switch (func)
            {
            case 'a':
                input(arg_str);
                print(list->Add(arg_str));
                break;
            case 'p':
                input(arg_str);
                input(arg_int);
                print(list->Add(arg_str, arg_int));
                break;
            case 'r':
                input(arg_int);
                print(list->RemoveByIndex(arg_int));
                break;
            case 'R':
                input(arg_str);
                print(list->Remove(arg_str));
                break;
            case 's':
                input(arg_int);
                input(arg_str);
                print(list->SetItem(arg_int, arg_str));
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
                input(arg_str);
                input(arg_int);
                print(list->Find(arg_str, arg_int));
                break;
            case 'e':
                input(arg_str);
                print(list->Exists(arg_str));
                break;
            case 'F':
                list->ForEach([](ITEMS_TYPE Item) { print(Item); });
                break;
            case 'A':
                input(arg_str);
                input(arg_int);
                for (int i = 0; i < arg_int; i++) list->Add(arg_str);
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