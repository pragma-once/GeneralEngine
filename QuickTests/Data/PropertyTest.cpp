#include "../../Engine/Engine.h"
#include <iostream>
#include <string>

#define print(content) (std::cout << content << '\n')

#define TestType std::string

class TestClass
{
public:
    std::string Name = "Unknown";
    Engine::Data::Property<TestType> A;
    Engine::Data::Property<TestType, false, true> B;
    Engine::Data::Property<TestType, true, false> C;
    // Engine::Data::Property<TestType, false, false> D; //-> error

    TestClass(TestType A, TestType B, TestType C)
    {
        a = A;
        b = B;
        c = C;

        this->A.SetSetter([this](TestType Value) { a = Value; print(Name << ".A set to " << Value); });
        this->A.SetGetter([this]() { print("Getting " << Name << ".A: " << a); return a; });
        this->B.SetGetter([this]() { print("Getting " << Name << ".B: " << b); return b; });
        this->C.SetSetter([this](TestType Value) { c = Value; print(Name << ".C set to " << Value); });
    }
private:
    TestType a;
    TestType b;
    TestType c;
};

int main()
{
    TestClass test("AA", "BB", "CC");
    test.A = test.B;
    test.A = "AA";
    //test.A = test.C; //->error
    //test.B = test.A; //->error
    //test.B = test.C; //->error
    test.C = test.A;
    test.C = test.B;
    std::string b = test.B;
    //std::string c = test.C; //->error
    //test.B = "hey!";        //->error
    print(b);
    print((std::string)test.B);

    float f = 0;
    Engine::Data::Property<float> F(
        [&f](float val) { f = val; print("F set to " << val); },
        [&f]() { print("Getting F: " << f); return f; }
    );

    F = 4;
    F = F * 8;
    print(F);

    getchar();

    return 0;
}