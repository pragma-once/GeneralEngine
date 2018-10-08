#pragma once

#include "../Engine.dec.h"

template <typename Type>
class Property<Type, true, true>
{
public:
    Property(const Property&) = delete;

    Property()
    {
        Setter = nullptr;
        Getter = nullptr;
    }

    Property(std::function<void(Type)> Setter, std::function<Type()> Getter)
    {
        this->Setter = Setter;
        this->Getter = Getter;
    }

    void SetSetter(std::function<void(Type)> Setter)
    {
        if (this->Setter != nullptr)
            throw std::logic_error("Cannot set the Setter more than once.");
        this->Setter = Setter;
    }

    void SetGetter(std::function<void(Type)> Getter)
    {
        if (this->Getter != nullptr)
            throw std::logic_error("Cannot set the Getter more than once.");
        this->Getter = Getter;
    }

    int operator=(const Property& Operand)
    {
        Type Value = Operand.Getter();
        Setter(Value);
        return Value;
    }

    int operator=(const Type& Value)
    {
        Setter(Value);
        return Value;
    }

    operator Type()
    {
        return Getter();
    }
private:
    std::function<void(Type)> Setter = nullptr;
    std::function<Type()> Getter = nullptr;
};