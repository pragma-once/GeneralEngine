#include "../../Engine/Engine.h"
#include <iostream>
#include <string>

#define print(context) (std::cout << context << '\n')
#define input(var) (std::cin >> var)

int main()
{
    Engine::Data::Collections::List<std::string> list;
    while (true)
    {
        print("a Item Index  => Add(Item, Index)");
        print("r Index       => Remove(Index)");
        print("s Index Value => SetItem(Index, Value)");
        print("g Index       => GetItem(Index)");
        char func;
        int arg_int;
        std::string arg_str;
        input(func);
        switch (func)
        {
        case 'a':
            input(arg_str);
            input(arg_int);
            list.Add(arg_str, arg_int);
            break;
        case 'r':
            input(arg_int);
            list.Remove(arg_int);
            break;
        case 's':
            input(arg_int);
            input(arg_str);
            list.SetItem(arg_int, arg_str);
            break;
        case 'g':
            input(arg_int);
            list.GetItem(arg_int);
            break;
        default:
            break;
        }
    }

    return 0;
}