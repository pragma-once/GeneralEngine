#include "../../Engine/Engine.h"
#include <iostream>
#include <string>

#define print(context) (std::cout << context << '\n')
#define input(var) (std::cin >> var)

class QuitException {};

void Prompt(Engine::Core::Loop&);

class TestModule : public Engine::Core::Module
{
public:
    std::string Name;

    TestModule(std::string Name, int Priority) : Module(Priority)
    {
        this->Name = Name;
    }

    virtual void OnStart() override
    {
        print(GetTime() << ", " << GetTimeDiff() << ": Starting: " << Name);
    }

    virtual void OnUpdate() override
    {
        print(GetTime() << ", " << GetTimeDiff() << ": Updating: " << Name);
        Prompt(*GetLoop());
    }

    virtual void OnStop() override
    {
        print(GetTime() << ", " << GetTimeDiff() << ": Ending: " << Name);
    }

    virtual void OnEnable() override
    {
        print(GetTime() << ", " << GetTimeDiff() << ": Enabling: " << Name);
    }

    virtual void OnDisable() override
    {
        print(GetTime() << ", " << GetTimeDiff() << ": Disabling: " << Name);
    }

    virtual std::string GetName() override
    {
        return Name;
    }
};

void Prompt(Engine::Core::Loop& loop)
{
    print("");
    print("add Name Priority       => Add Module with Name and Priority");
    print("ADD Name Priority Index => Add Module with Name and Priority and Index");
    print("rem Name                => Remove Module with Name");
    print("a   Name                => Enable Module with Name");
    print("d   Name                => Disable Module with Name");
    print("");
    print("f => Loop.Modules.ForEach([](Item) { print(Item.GetName()); })");
    print("");
    print("s => Loop.Start()");
    print("e => Loop.Stop()");
    print("");
    print("[anything] => pass");
    print("q          => quit");
    print("");
    std::string option;
    input(option);
    
    try
    {
        if (option == "add")
        {
            int arg;
            input(option >> arg);
            loop.Modules.Add(new TestModule(option, arg));
        }
        else if (option == "ADD")
        {
            int arg1, arg2;
            input(option >> arg1 >> arg2);
            loop.Modules.Add(new TestModule(option, arg1), arg2);
        }
        else if (option == "rem")
        {
            input(option);
            try { loop.Modules.RemoveByIndex(loop.Modules.Find(
                [option](Engine::Core::Module * Item)->bool { return option == Item->GetName(); }
                )); }
            catch (std::out_of_range&) { print("Module with name '" << option << "' doesn't exist."); }
        }
        else if (option == "a")
        {
            input(option);
            try{ loop.Modules.GetItem(loop.Modules.Find(
                [option](Engine::Core::Module * Item)->bool { return option == Item->GetName(); }
                ))->Enable(); }
            catch (std::out_of_range&) { print("Module with name '" << option << "' doesn't exist."); }
        }
        else if (option == "d")
        {
            input(option);
            try{ loop.Modules.GetItem(loop.Modules.Find(
                [option](Engine::Core::Module * Item)->bool { return option == Item->GetName(); }
                ))->Disable(); }
            catch (std::out_of_range&) { print("Module with name '" << option << "' doesn't exist."); }
        }
        else if (option == "f")
        {
            loop.Modules.ForEach([](Engine::Core::Module * Item) { print(Item->GetName()); });
        }
        else if (option == "s")
        {
            loop.Start();
        }
        else if (option == "e")
        {
            loop.Stop();
        }
        else if (option == "q")
        {
            throw QuitException();
        }
    }
    catch (std::exception& e) { print("Exception: " << e.what()); }
}

int main()
{
    Engine::Core::Loop loop;
    while (true) try
    {
        Prompt(loop);
    }
    catch (QuitException&) { return 0; }
}
