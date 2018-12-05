#include "../../Engine/Engine.h"
#include <iostream>
#include <string>

#define print(context) (std::cout << context << '\n')
#define input(var) (std::cin >> var)

class QuitException {};

void Prompt(Engine::Core::Container&);

class TestModule : public Engine::Core::Module
{
public:
    std::string Name;

    TestModule(std::string Name, int Priority) : Module(Priority)
    {
        this->Name = Name;
    }

    virtual void Start() override
    {
        print(GetTime() << ", " << GetTimeDiff() << ": Starting: " << Name);
    }

    virtual void Update() override
    {
        print(GetTime() << ", " << GetTimeDiff() << ": Updating: " << Name);
        Prompt(*GetContainer());
    }

    virtual void End() override
    {
        print(GetTime() << ", " << GetTimeDiff() << ": Ending: " << Name);
    }

    virtual void OnActivate() override
    {
        print(GetTime() << ", " << GetTimeDiff() << ": Activating: " << Name);
    }

    virtual void OnDeactivate() override
    {
        print(GetTime() << ", " << GetTimeDiff() << ": Deavtivating: " << Name);
    }

    virtual std::string GetName() override
    {
        return Name;
    }
};

void Prompt(Engine::Core::Container& container)
{
    print("");
    print("add Name Priority       => Add Module with Name and Priority");
    print("ADD Name Priority Index => Add Module with Name and Priority and Index");
    print("rem Name                => Remove Module with Name");
    print("a   Name                => Activate Module with Name");
    print("d   Name                => Deactivate Module with Name");
    print("");
    print("f => Container.Modules.ForEach([](Item) { print(Item.GetName()); })");
    print("");
    print("s => Container.Start()");
    print("e => Container.End()");
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
            container.Modules.Add(new TestModule(option, arg));
        }
        else if (option == "ADD")
        {
            int arg1, arg2;
            input(option >> arg1 >> arg2);
            container.Modules.Add(new TestModule(option, arg1), arg2);
        }
        else if (option == "rem")
        {
            input(option);
            try { container.Modules.RemoveByIndex(container.Modules.Find(
                [option](Engine::Core::Module * Item)->bool { return option == Item->GetName(); }
                )); }
            catch (std::out_of_range&) { print("Module with name '" << option << "' doesn't exist."); }
        }
        else if (option == "a")
        {
            input(option);
            try{ container.Modules.GetItem(container.Modules.Find(
                [option](Engine::Core::Module * Item)->bool { return option == Item->GetName(); }
                ))->Activate(); }
            catch (std::out_of_range&) { print("Module with name '" << option << "' doesn't exist."); }
        }
        else if (option == "d")
        {
            input(option);
            try{ container.Modules.GetItem(container.Modules.Find(
                [option](Engine::Core::Module * Item)->bool { return option == Item->GetName(); }
                ))->Deactivate(); }
            catch (std::out_of_range&) { print("Module with name '" << option << "' doesn't exist."); }
        }
        else if (option == "f")
        {
            container.Modules.ForEach([](Engine::Core::Module * Item) { print(Item->GetName()); });
        }
        else if (option == "s")
        {
            container.Start();
        }
        else if (option == "e")
        {
            container.End();
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
    Engine::Core::Container container;
    while (true) try
    {
        Prompt(container);
    }
    catch (QuitException&) { return 0; }
}
